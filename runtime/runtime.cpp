// SPDX-License-Identifier: GPL-3.0-only
//
// Lifetime inside explorer. The host hooks the taskbar thread with
// PillTaskbarHookProc; its first call takes a reference to this DLL and starts the
// control thread. That thread loads the mod DLL, reloads it when values.json asks,
// and on the host's Unload event unloads everything and frees this DLL itself. Every
// mod callback runs on that one thread, in Windhawk's order.

#include "internal.h"

#include <psapi.h>  // the K32 functions, which live in kernel32
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>

#include <algorithm>
#include <exception>
#include <vector>

#include <winrt/base.h>  // only for winrt::hresult_error in CallMod

namespace rt {
HMODULE g_self;
addon::Config g_config;
}  // namespace rt

namespace {

using namespace rt;

// How long a new control thread waits for one that is still on its way out.
constexpr DWORD kMutexWaitMs = 5000;
// Rereads of values.json after a failed read, one a second.
constexpr int kRereads = 10;
constexpr DWORD kRereadMs = 1000;
// How often values.json is read while the data folder cannot be watched.
constexpr DWORD kPollMs = 5000;
// How long an unload waits for the mod's objects to go, within its budget, and how
// often it looks (WaitForObjects).
constexpr DWORD kObjectWaitMs = 2000;
constexpr DWORD kObjectPollMs = 10;

HANDLE g_unload;
HANDLE g_loaded;
HANDLE g_unloaded;

// Set again by each hook call that finds it false; the control thread clears it as
// its very last step, so a later injection into this same image starts afresh.
std::atomic<bool> g_started;
// Set by Begin when the host's Config cannot be used; the control thread reports it.
PCWSTR g_refusal;
// A mod image (or its hooks) had to stay mapped. For the rest of this image's life no
// mod is loaded again: the leftover image would clash with a new one.
std::atomic<bool> g_leaked;

struct LoadedMod {
    HMODULE module = nullptr;
    std::wstring file;  // the file actually loaded: the mod DLL or a fresh copy of it
    const PillAddonCallbacks* callbacks = nullptr;
    bool running = false;  // Init returned TRUE
};
LoadedMod g_mod;

std::wstring NamedObject(PCWSTR format) {
    wchar_t name[96];
    swprintf(name, 96, format, GetCurrentProcessId());
    return name;
}

bool StartsWithNoCase(std::wstring_view text, std::wstring_view prefix) {
    return text.size() >= prefix.size() &&
           CompareStringOrdinal(text.data(), (int)prefix.size(), prefix.data(), (int)prefix.size(), TRUE) ==
               CSTR_EQUAL;
}

// A module Windhawk loaded for one of addon::kWindhawkMods: "<id>_<version>_<n>.dll",
// or the same without the "local@" of a local build.
bool IsWindhawkCopy(std::wstring_view name) {
    constexpr std::wstring_view kLocal = L"local@";
    for (std::wstring_view id : addon::kWindhawkMods) {
        std::wstring_view bare = StartsWithNoCase(id, kLocal) ? id.substr(kLocal.size()) : id;
        if (StartsWithNoCase(name, std::wstring(id) + L"_") || StartsWithNoCase(name, std::wstring(bare) + L"_")) {
            return true;
        }
    }
    return false;
}

std::vector<HMODULE> ProcessModules() {
    std::vector<HMODULE> modules(512);
    DWORD needed = 0;
    for (;;) {
        DWORD bytes = (DWORD)(modules.size() * sizeof(HMODULE));
        if (!K32EnumProcessModules(GetCurrentProcess(), modules.data(), bytes, &needed)) {
            return {};
        }
        if (needed <= bytes) {
            break;
        }
        modules.resize(needed / sizeof(HMODULE) + 16);
    }
    modules.resize(needed / sizeof(HMODULE));
    return modules;
}

// Why this runtime must not load the mod into this explorer, or null.
PCWSTR RefusalReason() {
    if (g_leaked) {
        log::Line(L"a mod image of an earlier load is still mapped in this explorer");
        return L"restart";
    }
    for (HMODULE module : ProcessModules()) {
        wchar_t name[MAX_PATH];
        if (!K32GetModuleBaseNameW(GetCurrentProcess(), module, name, MAX_PATH)) {
            continue;
        }
        // The mod imports the runtime by name, so it would bind to whichever image of
        // that name the loader finds first.
        if (module != g_self && CompareStringOrdinal(name, -1, addon::kRuntimeDll, -1, TRUE) == CSTR_EQUAL) {
            log::Line(L"another %s is still mapped at %p", addon::kRuntimeDll, module);
            return L"restart";
        }
        // The Windhawk build of the same mod: both would patch the same functions.
        if (IsWindhawkCopy(name)) {
            log::Line(L"Windhawk's copy of the mod is loaded: %s", name);
            return L"windhawk";
        }
    }
    return nullptr;
}

std::wstring CopyPattern() {
    std::wstring path = g_config.modPath;
    return path.substr(0, path.size() - 4) + L".r*.dll";
}

// Copies made by LoadModFile that nothing maps any more.
void DeleteOldCopies() {
    std::wstring pattern = CopyPattern();
    std::wstring dir = pattern.substr(0, pattern.find_last_of(L'\\') + 1);
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileW(pattern.c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        DeleteFileW((dir + data.cFileName).c_str());
    } while (FindNextFileW(find, &data));
    FindClose(find);
}

// While a thread of the previous load still holds the mod DLL, loading the same file
// would hand back that image with its old globals. A copy gives a fresh one; without
// a copy nothing is loaded.
HMODULE LoadModFile(std::wstring& file) {
    file = g_config.modPath;
    HMODULE old = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, file.c_str(), &old)) {
        std::wstring copy = file.substr(0, file.size() - 4) + L".r" + std::to_wstring(GetTickCount64()) + L".dll";
        if (!CopyFileW(file.c_str(), copy.c_str(), FALSE)) {
            log::Line(L"the previous mod image is still mapped and cannot be copied (%u)", GetLastError());
            return nullptr;
        }
        log::Line(L"the previous mod image is still mapped; loading a copy");
        file = copy;
    }
    return LoadLibraryExW(file.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
}

void FreeMod() {
    std::wstring file = g_mod.file;
    FreeLibrary(g_mod.module);
    g_mod = {};
    // The mod may hold references of its own (a thread on its way out); its image goes
    // when they do. A copy is deleted once it is unmapped.
    for (DWORD waited = 0;; waited += 50) {
        HMODULE still = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, file.c_str(), &still)) {
            if (file != g_config.modPath) {
                DeleteFileW(file.c_str());
            }
            return;
        }
        if (waited >= addon::kRuntimeUnmapMs) {
            break;
        }
        Sleep(50);
    }
    log::Line(L"the mod image is still mapped after %u ms", addon::kRuntimeUnmapMs);
}

// When the unload that starts now has to be done by: the mod's callbacks and the
// wait for its threads share this budget. "$unloadWaitMs" in values.json can only
// shorten it (a test knob).
ULONGLONG UnloadDeadline() {
    int budget = std::min<int>(addon::kRuntimeUnloadMs, settings::RuntimeInt(L"$unloadWaitMs", addon::kRuntimeUnloadMs));
    return GetTickCount64() + (ULONGLONG)std::max(budget, 0);
}

// XAML can keep an element the mod took out of the tree, with the handlers the mod
// gave it, until the panel's unload transition ends (0.45 s, 27 frames, for
// AddDeleteThemeTransition); their last Release runs the mod's code. So the image stays
// until C++/WinRT counts none of the mod's objects alive. A wait of a frame or two, or
// of a fixed time, misses such a transition or slows every unload. Pinning the image
// would keep it, and through its imports this runtime, mapped for good: each reload
// would map a copy, and the next version's runtime would refuse to start beside this
// one. The count is polled on this thread, never through a UI thread, so a busy or
// stuck one only uses up the wait. An object alive after kObjectWaitMs has a holder
// that does not let go by itself (a handler on a long-lived element, a global of the
// mod's): the image then goes as it did before this wait, and the log says how many
// were left.
void WaitForObjects(ULONGLONG deadline) {
    DWORD (*live)() = g_mod.callbacks ? g_mod.callbacks->liveObjects : nullptr;
    DWORD first = live ? live() : 0;
    if (!first) {
        return;
    }
    ULONGLONG start = GetTickCount64();
    ULONGLONG end = std::min(deadline, start + kObjectWaitMs);
    DWORD left = first;
    while (left && GetTickCount64() < end) {
        Sleep(kObjectPollMs);
        left = live();
    }
    log::Line(L"waited %llu ms for %u objects of the mod: %u left", GetTickCount64() - start, first, left);
}

// Frees the mod DLL once no thread can still be in its code or in a trampoline. When
// that cannot be shown (a hook still on, a thread still inside at the deadline, a
// hook that will not go), the image, its trampolines and MinHook stay as they are:
// a leak rather than a crash. False then.
bool ReleaseMod(bool hooksOff, ULONGLONG deadline) {
    if (hooksOff) {
        // Before the thread check: an object's last Release runs the mod's code.
        WaitForObjects(deadline);
    }
    std::vector<CodeRange> code;
    threads::AddImageCode(g_mod.module, code);
    hooks::AddTrampolines(code);
    PCWSTR why = nullptr;
    if (!hooksOff) {
        why = L"a hook could not be switched off";
    } else if (!threads::WaitUntilOut(code, deadline)) {
        why = L"a thread may still be in its code";
    } else if (!hooks::RemoveAll()) {
        why = L"a hook could not be removed";
    }
    if (why) {
        log::Line(L"leaving %s mapped: %s", g_mod.file.c_str(), why);
        g_leaked = true;
        g_mod = {};
        return false;
    }
    FreeMod();
    return true;
}

// Runs one of the mod's callbacks. An exception leaving it would end the control
// thread and with it explorer, so it stops here, is logged, and the call counts as
// failed.
template <typename Fn>
bool CallMod(PCWSTR name, Fn&& fn) {
    try {
        fn();
        return true;
    } catch (const winrt::hresult_error& e) {
        log::Line(L"%s threw hresult_error 0x%08X", name, (unsigned)e.code().value);
    } catch (const std::exception& e) {
        log::Line(L"%s threw %S", name, e.what());
    } catch (...) {
        log::Line(L"%s threw an exception", name);
    }
    return false;
}

bool UnloadMod(ULONGLONG deadline);

bool LoadMod() {
    if (g_leaked) {
        WriteState(L"refused", L"restart");
        return false;
    }
    std::wstring file;
    HMODULE module = LoadModFile(file);
    if (!module) {
        log::Line(L"cannot load %s: %u", file.c_str(), GetLastError());
        WriteState(L"initFailed", L"init");
        return false;
    }
    auto entry = reinterpret_cast<PillAddonGetCallbacks_t>(GetProcAddress(module, PILL_ADDON_ENTRY_NAME));
    const PillAddonCallbacks* callbacks = entry ? entry() : nullptr;
    if (!callbacks || callbacks->size < sizeof(PillAddonCallbacks) ||
        callbacks->apiVersion != PILL_ADDON_API_VERSION) {
        log::Line(L"%s has no usable %S", file.c_str(), PILL_ADDON_ENTRY_NAME);
        FreeLibrary(module);
        WriteState(L"initFailed", L"init");
        return false;
    }
    settings::SetDefaults(callbacks->defaults, callbacks->defaultsCount);
    if (!hooks::BeginMod()) {
        FreeLibrary(module);
        WriteState(L"initFailed", L"init");
        return false;
    }
    g_mod = {module, file, callbacks, false};
    log::Line(L"loading %s (%s %s)", file.c_str(), callbacks->modId, callbacks->modVersion);
    BOOL started = FALSE;
    if (!CallMod(L"Wh_ModInit", [&] { started = callbacks->init(); }) || !started) {
        // As in Windhawk: no Uninit after a failed Init, and its queued hooks are dropped.
        // An Init that threw is treated the same.
        ReleaseMod(true, GetTickCount64() + addon::kRuntimeUnloadMs);
        WriteState(L"initFailed", hooks::g_symbolsFailed ? L"symbols" : L"init");
        return false;
    }
    hooks::EndInit();
    g_mod.running = true;
    if (!CallMod(L"Wh_ModAfterInit", [&] { callbacks->afterInit(); })) {
        // Half started: taken out again through its Uninit, and the start counts as failed.
        if (UnloadMod(UnloadDeadline())) {
            WriteState(L"initFailed", L"init");
        } else {
            WriteState(L"refused", L"restart");
        }
        return false;
    }
    WriteState(L"running");
    return true;
}

// False when the mod had to stay mapped.
bool UnloadMod(ULONGLONG deadline) {
    if (!g_mod.module) {
        return true;
    }
    bool hooksOff = true;
    if (g_mod.running) {
        // A callback that throws does not stop the unload; ReleaseMod still checks
        // that nothing is left in the mod's code before it is freed.
        const PillAddonCallbacks* callbacks = g_mod.callbacks;
        CallMod(L"Wh_ModBeforeUninit", [&] { callbacks->beforeUninit(); });
        hooksOff = hooks::DisableAll();
        // The mod's Uninit waits for calls still inside its hooks, which needs the
        // hooks off first.
        CallMod(L"Wh_ModUninit", [&] { callbacks->uninit(); });
    }
    return ReleaseMod(hooksOff, deadline);
}

void OnValuesChanged() {
    if (!g_config.modPath[0]) {
        return;
    }
    if (!g_mod.module) {
        LoadMod();  // it failed or turned itself off; a change gives it another try
        return;
    }
    BOOL reload = FALSE;
    BOOL keep = TRUE;
    const PillAddonCallbacks* callbacks = g_mod.callbacks;
    if (!CallMod(L"Wh_ModSettingsChanged", [&] { keep = callbacks->settingsChanged(&reload); })) {
        // Its settings may be half applied: start it afresh with the new values.
        reload = TRUE;
    }
    if (reload) {
        WriteState(L"reloading");
        if (UnloadMod(UnloadDeadline())) {
            LoadMod();
        } else {
            WriteState(L"refused", L"restart");
        }
    } else if (!keep) {
        if (UnloadMod(UnloadDeadline())) {
            WriteState(L"off");
        } else {
            WriteState(L"refused", L"restart");
        }
    }
}

// Watches the data folder for a new values.json.
class DirWatch {
   public:
    // Opens the folder when needed and starts a read unless one is pending. False
    // when the folder cannot be watched now; the caller then polls.
    bool Arm() {
        if (m_pending) {
            return true;
        }
        if (!m_overlapped.hEvent) {
            m_overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
            if (!m_overlapped.hEvent) {
                Fail(L"no event");
                return false;
            }
        }
        if (m_dir == INVALID_HANDLE_VALUE) {
            m_dir = CreateFileW(g_config.dataDir, FILE_LIST_DIRECTORY,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
            if (m_dir == INVALID_HANDLE_VALUE) {
                Fail(L"cannot open it");
                return false;
            }
        }
        ResetEvent(m_overlapped.hEvent);
        m_pending = ReadDirectoryChangesW(m_dir, m_buffer, sizeof(m_buffer), FALSE,
                                          FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE |
                                              FILE_NOTIFY_CHANGE_SIZE,
                                          nullptr, &m_overlapped, nullptr);
        if (!m_pending) {
            Fail(L"ReadDirectoryChangesW failed");
            CloseDir();  // opened again on the next try
            return false;
        }
        if (m_failing) {
            log::Line(L"watching the data folder again");
            m_failing = false;
        }
        return true;
    }

    bool Armed() const { return m_pending; }
    HANDLE Event() const { return m_overlapped.hEvent; }

    // After Event() fired: whether values.json was among the changes. Only that name
    // counts, so the runtime's and the host's own log and state writes wake this
    // thread but never reread anything.
    bool TakeValuesChanged() {
        m_pending = false;
        DWORD bytes = 0;
        if (!GetOverlappedResult(m_dir, &m_overlapped, &bytes, FALSE)) {
            CloseDir();  // the folder went away or the handle broke: reopen it
            return true;
        }
        if (!bytes) {
            return true;  // overflowed: read again to be safe
        }
        for (auto* entry = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(m_buffer);;) {
            int length = (int)(entry->FileNameLength / sizeof(WCHAR));
            if (CompareStringOrdinal(entry->FileName, length, addon::kValuesFile, -1, TRUE) == CSTR_EQUAL) {
                return true;
            }
            if (!entry->NextEntryOffset) {
                return false;
            }
            entry = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(reinterpret_cast<const BYTE*>(entry) +
                                                                     entry->NextEntryOffset);
        }
    }

    // The buffer lives in this DLL: the read must be finished before it unloads.
    void Stop() {
        CloseDir();
        if (m_overlapped.hEvent) {
            CloseHandle(m_overlapped.hEvent);
            m_overlapped.hEvent = nullptr;
        }
    }

   private:
    HANDLE m_dir = INVALID_HANDLE_VALUE;
    OVERLAPPED m_overlapped{};
    bool m_pending = false;
    bool m_failing = false;
    alignas(DWORD) BYTE m_buffer[8192];

    void Fail(PCWSTR what) {
        if (!m_failing) {
            log::Line(L"cannot watch the data folder (%s, %u); reading %s every %u s", what, GetLastError(),
                      addon::kValuesFile, kPollMs / 1000);
            m_failing = true;
        }
    }

    void CloseDir() {
        if (m_pending) {
            CancelIoEx(m_dir, &m_overlapped);
            DWORD bytes;
            GetOverlappedResult(m_dir, &m_overlapped, &bytes, TRUE);
            m_pending = false;
        }
        if (m_dir != INVALID_HANDLE_VALUE) {
            CloseHandle(m_dir);
            m_dir = INVALID_HANDLE_VALUE;
        }
    }
};

// Until Unload. firstReadFailed: values.json could not be read at start.
void Serve(DirWatch& watch, bool firstReadFailed) {
    int rereads = firstReadFailed ? kRereads : 0;
    bool waitFailed = false;
    for (;;) {
        bool watching = watch.Arm();
        DWORD timeout = rereads ? kRereadMs : watching ? INFINITE : kPollMs;
        HANDLE waits[2] = {g_unload, watch.Event()};
        DWORD result = WaitForMultipleObjects(watching ? 2 : 1, waits, FALSE, timeout);
        if (result == WAIT_OBJECT_0) {
            break;  // Unload
        }
        if (result == WAIT_FAILED) {
            // Not Unload: only that event ends the runtime.
            if (!waitFailed) {
                log::Line(L"wait failed (%u)", GetLastError());
                waitFailed = true;
            }
            Sleep(kRereadMs);
            continue;
        }
        if (result == WAIT_OBJECT_0 + 1) {
            if (!watch.TakeValuesChanged()) {
                continue;
            }
            // A writer may replace the file in more than one step; let it settle.
            if (WaitForSingleObject(g_unload, 100) == WAIT_OBJECT_0) {
                break;
            }
        }
        if (rereads) {
            rereads--;
        }
        switch (settings::Reload()) {
            case settings::Reread::kChanged:
                rereads = 0;
                OnValuesChanged();
                break;
            case settings::Reread::kSame:
                rereads = 0;
                break;
            case settings::Reread::kFailed:
                // A fresh failure starts the rereads; one during them counts down.
                if (!rereads && result != WAIT_TIMEOUT) {
                    rereads = kRereads;
                }
                break;
        }
    }
}

void CloseEvents() {
    for (HANDLE* event : {&g_unload, &g_loaded, &g_unloaded}) {
        if (*event) {
            CloseHandle(*event);
            *event = nullptr;
        }
    }
}

[[noreturn]] void Exit() {
    g_refusal = nullptr;
    g_started = false;
    FreeLibraryAndExitThread(g_self, 0);
}

// Runs the mod until Unload and takes it out again. A refusal or a failed start stays
// the last state written; otherwise the run ends with "stopped".
void RunAndUnload() {
    PCWSTR refusal = g_refusal ? g_refusal : RefusalReason();
    if (refusal) {
        // Loaded stays on and the reason stays in runtime-state.json until the host,
        // having read it, asks this runtime to go.
        WriteState(L"refused", refusal);
        WaitForSingleObject(g_unload, INFINITE);
        return;
    }
    if (!hooks::Initialize()) {
        WriteState(L"initFailed", L"init");
        WaitForSingleObject(g_unload, INFINITE);
        return;
    }
    // Watching starts before the first read, so a change made while the mod loads
    // (which can take seconds) is not lost.
    DirWatch watch;
    watch.Arm();
    bool readFailed = settings::Reload() == settings::Reread::kFailed;
    if (g_config.modPath[0]) {
        DeleteOldCopies();
        LoadMod();
    } else {
        WriteState(L"running");  // runtime only, for the injection test
    }
    Serve(watch, readFailed);
    watch.Stop();
    UnloadMod(UnloadDeadline());
    if (!g_leaked) {
        hooks::Uninitialize();
    }
    WriteState(L"stopped", g_leaked ? L"leaked" : L"");
}

DWORD WINAPI ControlThread(void*) {
    // A runtime that is on its way out releases the mutex before it sets Unloaded; one
    // that holds it longer serves this explorer, and its events and runtime-state.json
    // are not this one's to touch.
    HANDLE mutex = CreateMutexW(nullptr, FALSE, NamedObject(addon::kRuntimeMutex).c_str());
    DWORD owned = mutex ? WaitForSingleObject(mutex, kMutexWaitMs) : WAIT_FAILED;
    if (owned != WAIT_OBJECT_0 && owned != WAIT_ABANDONED) {
        OutputDebugStringW(ADDON_NAME L" runtime: another runtime already serves this explorer\n");
        if (mutex) {
            CloseHandle(mutex);
        }
        Exit();
    }
    settings::Reset();
    log::Open(DataPath(InPanel() ? addon::kPanelRuntimeLog : addon::kRuntimeLog));
    log::Line(L"runtime %s in %s %u, host %u", g_config.version, InPanel() ? L"ShellHost" : L"explorer",
              GetCurrentProcessId(), g_config.hostPid);

    DWORD access = SYNCHRONIZE | EVENT_MODIFY_STATE;
    g_unload = OpenEventW(access, FALSE, NamedObject(addon::kUnloadEvent).c_str());
    g_loaded = OpenEventW(access, FALSE, NamedObject(addon::kLoadedEvent).c_str());
    g_unloaded = OpenEventW(access, FALSE, NamedObject(addon::kUnloadedEvent).c_str());
    if (g_unload && g_loaded && g_unloaded) {
        ResetEvent(g_unloaded);
        // Written first: the host reads runtime-state.json once Loaded is on and must
        // not find the previous run's "stopped" there.
        WriteState(L"starting");
        SetEvent(g_loaded);
        RunAndUnload();
    } else {
        // Without these nobody could ever ask this DLL to leave.
        log::Line(L"the host's events are missing (%u)", GetLastError());
    }
    log::Line(L"runtime unloads");
    // Everything that names this run is let go before Unloaded, so a runtime injected
    // right after finds the mutex free and can open the log.
    log::Close();
    if (g_loaded) {
        ResetEvent(g_loaded);
    }
    ReleaseMutex(mutex);
    CloseHandle(mutex);
    if (g_unloaded) {
        SetEvent(g_unloaded);
    }
    CloseEvents();
    Exit();
}

// An absolute path of an existing folder: where a refusal may be reported even when
// the rest of the host's Config is not understood.
bool UsableDataDir(PCWSTR dir) {
    bool absolute = (dir[0] && dir[1] == L':' && dir[2] == L'\\') || (dir[0] == L'\\' && dir[1] == L'\\');
    DWORD attributes = absolute ? GetFileAttributesW(dir) : INVALID_FILE_ATTRIBUTES;
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

// On the taskbar thread, inside its message retrieval: only copy the host's
// configuration, keep this DLL loaded and start the control thread.
void Begin() {
    HANDLE mapping = OpenFileMappingW(FILE_MAP_READ, FALSE, NamedObject(addon::kConfigMapping).c_str());
    if (!mapping) {
        return;  // not loaded by our host
    }
    auto* view = static_cast<const BYTE*>(MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0));
    MEMORY_BASIC_INFORMATION region{};
    SIZE_T mapped = view && VirtualQuery(view, &region, sizeof(region)) ? region.RegionSize : 0;
    bool ok = false, report = false;
    if (mapped >= offsetof(addon::Config, modPath)) {
        g_config = {};
        memcpy(&g_config, view, std::min<SIZE_T>(mapped, sizeof(addon::Config)));
        g_config.version[ARRAYSIZE(g_config.version) - 1] = L'\0';
        g_config.dataDir[ARRAYSIZE(g_config.dataDir) - 1] = L'\0';
        g_config.modPath[ARRAYSIZE(g_config.modPath) - 1] = L'\0';
        if (g_config.size == sizeof(addon::Config) && g_config.protocol == addon::kProtocol &&
            mapped >= sizeof(addon::Config)) {
            size_t n = wcslen(g_config.modPath);
            ok = n == 0 ||
                 (n > 4 && CompareStringOrdinal(g_config.modPath + n - 4, 4, L".dll", 4, TRUE) == CSTR_EQUAL);
        }
        // The fields up to dataDir keep their place across protocol versions.
        report = !ok && UsableDataDir(g_config.dataDir);
    }
    if (view) {
        UnmapViewOfFile(view);
    }
    CloseHandle(mapping);
    if (!ok && !report) {
        return;
    }
    if (!ok) {
        g_config.modPath[0] = L'\0';
        g_refusal = L"protocol";
    }
    HMODULE self = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(&Begin), &self)) {
        return;
    }
    HANDLE thread = CreateThread(nullptr, 0, ControlThread, nullptr, 0, nullptr);
    if (!thread) {
        g_refusal = nullptr;
        FreeLibrary(self);
        return;
    }
    CloseHandle(thread);
}

}  // namespace

extern "C" __declspec(dllexport) LRESULT CALLBACK PillTaskbarHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (!g_started.exchange(true)) {
        Begin();
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_self = instance;
    }
    return TRUE;
}
