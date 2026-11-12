// SPDX-License-Identifier: GPL-3.0-only
//
// Function hooks on MinHook with Windhawk's meaning: creating a hook fills the
// trampoline at once and queues it; queued hooks go live together, either right
// after Wh_ModInit returns or when the mod calls Wh_ApplyHookOperations, which is
// refused before that and while unloading. HookSymbols reads the offset table the
// host built outside explorer instead of reading PDBs in here.

#include "internal.h"

#include <MinHook.h>

#include <algorithm>
#include <optional>
#include <vector>

namespace rt::hooks {

std::atomic<bool> g_symbolsFailed;

namespace {

SRWLOCK g_degradedLock = SRWLOCK_INIT;
std::vector<std::wstring> g_degraded;

// True when the module was not listed yet.
bool NoteDegraded(const std::wstring& file) {
    AcquireSRWLockExclusive(&g_degradedLock);
    bool added = std::find(g_degraded.begin(), g_degraded.end(), file) == g_degraded.end();
    if (added) {
        g_degraded.push_back(file);
    }
    ReleaseSRWLockExclusive(&g_degradedLock);
    return added;
}

SRWLOCK g_lock = SRWLOCK_INIT;
struct Hook {
    void* target;
    void* trampoline;  // MinHook's slot, which also holds the relay to the detour
};
std::vector<Hook> g_hooks;      // every hook the current mod made
std::vector<void*> g_toRemove;  // queued off by Wh_RemoveFunctionHook
std::atomic<bool> g_initialized;
std::atomic<bool> g_unloading;
// MinHook's own lock is not reentrant: a hook op reached again on the same thread
// (through a hooked call made while patching) must fail instead of hanging.
thread_local bool t_busy;

struct Busy {
    bool entered;
    Busy() : entered(!t_busy) { t_busy = true; }
    ~Busy() {
        if (entered) {
            t_busy = false;
        }
    }
};

bool ApplyLocked() {
    MH_STATUS status = MH_ApplyQueued();
    if (status != MH_OK && status != MH_ERROR_NOT_CREATED) {
        log::Line(L"MH_ApplyQueued: %S", MH_StatusToString(status));
        return false;
    }
    for (void* target : g_toRemove) {
        MH_RemoveHook(target);
        std::erase_if(g_hooks, [&](const Hook& h) { return h.target == target; });
    }
    g_toRemove.clear();
    return true;
}

// Under g_lock: takes out hooks of this mod load, queued or live.
void RemoveLocked(const std::vector<void*>& targets) {
    for (void* target : targets) {
        MH_STATUS status = MH_RemoveHook(target);
        if (status != MH_OK && status != MH_ERROR_NOT_CREATED) {
            log::Line(L"MH_RemoveHook %p: %S", target, MH_StatusToString(status));
            continue;
        }
        std::erase_if(g_hooks, [&](const Hook& h) { return h.target == target; });
        std::erase(g_toRemove, target);
    }
}

std::vector<void*> Targets() {
    std::vector<void*> targets;
    for (const Hook& h : g_hooks) {
        targets.push_back(h.target);
    }
    return targets;
}

}  // namespace

bool Initialize() {
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
        log::Line(L"MH_Initialize: %S", MH_StatusToString(status));
        return false;
    }
    return true;
}

void Uninitialize() {
    MH_Uninitialize();
}

bool BeginMod() {
    AcquireSRWLockExclusive(&g_lock);
    Busy busy;
    bool ok = g_hooks.empty();
    if (!ok) {
        log::Line(L"%zu hooks of the previous mod load are still there", g_hooks.size());
    } else {
        // Nothing of the previous load's hook table or trampoline slots carries over.
        MH_Uninitialize();
        MH_STATUS status = MH_Initialize();
        if (status != MH_OK) {
            log::Line(L"MH_Initialize: %S", MH_StatusToString(status));
            ok = false;
        }
    }
    g_toRemove.clear();
    g_initialized = false;
    g_symbolsFailed = false;
    AcquireSRWLockExclusive(&g_degradedLock);
    g_degraded.clear();
    ReleaseSRWLockExclusive(&g_degradedLock);
    g_unloading = !ok;
    ReleaseSRWLockExclusive(&g_lock);
    return ok;
}

void EndInit() {
    AcquireSRWLockExclusive(&g_lock);
    Busy busy;
    ApplyLocked();
    g_initialized = true;
    ReleaseSRWLockExclusive(&g_lock);
}

bool DisableAll() {
    g_unloading = true;
    AcquireSRWLockExclusive(&g_lock);
    Busy busy;
    bool ok = false;
    for (int attempt = 0; attempt < 2 && !ok; attempt++) {
        MH_STATUS status = MH_DisableHook(MH_ALL_HOOKS);
        ok = status == MH_OK || status == MH_ERROR_NOT_CREATED;
        if (!ok) {
            log::Line(L"MH_DisableHook: %S", MH_StatusToString(status));
        }
    }
    ReleaseSRWLockExclusive(&g_lock);
    return ok;
}

bool RemoveAll() {
    AcquireSRWLockExclusive(&g_lock);
    Busy busy;
    for (int attempt = 0; attempt < 2 && !g_hooks.empty(); attempt++) {
        RemoveLocked(Targets());
    }
    bool ok = g_hooks.empty();
    g_toRemove.clear();
    g_initialized = false;
    ReleaseSRWLockExclusive(&g_lock);
    return ok;
}

void AddTrampolines(std::vector<CodeRange>& out) {
    // A slot is 64 bytes on x64 (MEMORY_SLOT_SIZE in MinHook's buffer.h).
    constexpr uintptr_t kSlot = 64;
    AcquireSRWLockShared(&g_lock);
    for (const Hook& h : g_hooks) {
        auto start = reinterpret_cast<uintptr_t>(h.trampoline);
        out.push_back({start, start + kSlot});
    }
    ReleaseSRWLockShared(&g_lock);
}

BOOL Set(void* target, void* hook, void** original) {
    if (!target || !hook || g_unloading) {
        return FALSE;
    }
    Busy busy;
    if (!busy.entered) {
        return FALSE;
    }
    AcquireSRWLockExclusive(&g_lock);
    // Checked again under the lock: a thread that passed the check above just before
    // the unload began must not add a hook the unload no longer sees.
    MH_STATUS status = MH_ERROR_DISABLED;
    if (!g_unloading) {
        void* trampoline = nullptr;
        status = MH_CreateHook(target, hook, &trampoline);
        if (status == MH_OK) {
            status = MH_QueueEnableHook(target);
            if (status == MH_OK) {
                g_hooks.push_back({target, trampoline});
                if (original) {
                    *original = trampoline;
                }
            } else {
                MH_RemoveHook(target);
            }
        }
    }
    ReleaseSRWLockExclusive(&g_lock);
    if (status != MH_OK) {
        log::Line(L"hook %p: %S", target, MH_StatusToString(status));
        return FALSE;
    }
    return TRUE;
}

// ---- symbols --------------------------------------------------------------------

namespace {

struct ModuleId {
    std::wstring file;  // lower-case file name
    std::wstring key;   // GUID and age, as in the offsets file name
    DWORD stamp = 0;
    DWORD size = 0;
    DWORD headers = 0;  // SizeOfHeaders: no function starts below it
};

// The CodeView record of the image as loaded, so a DLL replaced on disk after the
// host read it does not match a stale table.
bool Identify(HMODULE module, ModuleId& id) {
    wchar_t path[MAX_PATH];
    DWORD n = GetModuleFileNameW(module, path, MAX_PATH);
    if (!n || n >= MAX_PATH) {
        return false;
    }
    std::wstring file = path;
    file = file.substr(file.find_last_of(L'\\') + 1);
    CharLowerBuffW(file.data(), (DWORD)file.size());
    id.file = file;

    auto* base = reinterpret_cast<const BYTE*>(module);
    auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    id.stamp = nt->FileHeader.TimeDateStamp;
    id.size = nt->OptionalHeader.SizeOfImage;
    id.headers = nt->OptionalHeader.SizeOfHeaders;
    const IMAGE_DATA_DIRECTORY& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    auto* entries = reinterpret_cast<const IMAGE_DEBUG_DIRECTORY*>(base + dir.VirtualAddress);
    for (DWORD i = 0; dir.VirtualAddress && i < dir.Size / sizeof(IMAGE_DEBUG_DIRECTORY); i++) {
        const IMAGE_DEBUG_DIRECTORY& e = entries[i];
        if (e.Type != IMAGE_DEBUG_TYPE_CODEVIEW || !e.AddressOfRawData || e.SizeOfData < 24) {
            continue;
        }
        const BYTE* cv = base + e.AddressOfRawData;
        if (*reinterpret_cast<const DWORD*>(cv) != 0x53445352) {  // "RSDS"
            continue;
        }
        GUID guid;
        memcpy(&guid, cv + 4, sizeof(guid));
        DWORD age = *reinterpret_cast<const DWORD*>(cv + 20);
        wchar_t key[64];
        swprintf(key, 64, L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%X", guid.Data1, guid.Data2, guid.Data3,
                 guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
                 guid.Data4[6], guid.Data4[7], age);
        id.key = key;
        return true;
    }
    return false;
}

using Offsets = std::unordered_map<std::wstring, std::optional<DWORD>>;

// 1#<module>#<stamp>-<size>#<symbol>#<rva>#... ; an empty rva means "not in this
// module". The whole table is dropped when anything in it disagrees with the image.
bool LoadOffsets(const ModuleId& id, Offsets& out) {
    std::wstring path = DataPath(addon::kOffsetsDir) + L"\\" + id.file + L"_" + id.key + L".txt";
    std::string bytes;
    if (!ReadFileBytes(path, bytes)) {
        log::Line(L"no offsets for %s (%s)", id.file.c_str(), id.key.c_str());
        return false;
    }
    while (!bytes.empty() && (bytes.back() == '\n' || bytes.back() == '\r')) {
        bytes.pop_back();
    }
    std::wstring text = Wide(bytes);
    std::vector<std::wstring> parts;
    size_t start = 0;
    for (;;) {
        size_t end = text.find(L'#', start);
        parts.push_back(text.substr(start, end == std::wstring::npos ? std::wstring::npos : end - start));
        if (end == std::wstring::npos) {
            break;
        }
        start = end + 1;
    }
    std::wstring identity = std::to_wstring(id.stamp) + L"-" + std::to_wstring(id.size);
    if (parts.size() < 3 || parts[0] != L"1" || parts[1] != id.file || parts[2] != identity ||
        (parts.size() - 3) % 2) {
        log::Line(L"offsets for %s do not match the loaded image", id.file.c_str());
        return false;
    }
    Offsets table;
    for (size_t i = 3; i + 1 < parts.size(); i += 2) {
        if (parts[i + 1].empty()) {
            table[parts[i]] = std::nullopt;
            continue;
        }
        // Decimal digits only: no sign, space or other prefix wcstoul would let through.
        const std::wstring& digits = parts[i + 1];
        bool numeric = digits.size() <= 10 &&
                       std::all_of(digits.begin(), digits.end(), [](wchar_t c) { return c >= L'0' && c <= L'9'; });
        unsigned long long rva = numeric ? wcstoull(digits.c_str(), nullptr, 10) : 0;
        if (!numeric || rva < id.headers || rva >= id.size) {
            log::Line(L"offsets for %s: bad offset", id.file.c_str());
            return false;
        }
        table[parts[i]] = (DWORD)rva;
    }
    out = std::move(table);
    return true;
}

}  // namespace

BOOL HookSymbols(HMODULE module, const WH_SYMBOL_HOOK* entries, size_t count) {
    ModuleId id;
    Offsets table;
    if (!module || !Identify(module, id)) {
        log::Line(L"HookSymbols: cannot identify module %p", module);
    } else {
        LoadOffsets(id, table);
    }
    auto* base = reinterpret_cast<BYTE*>(module);
    std::vector<void*> found(count);
    bool missingRequired = false;
    for (size_t i = 0; i < count; i++) {
        const WH_SYMBOL_HOOK& entry = entries[i];
        for (size_t j = 0; j < entry.symbolsCount && !found[i]; j++) {
            std::wstring name(entry.symbols[j].string, entry.symbols[j].length);
            auto it = table.find(name);
            if (it == table.end()) {
                if (!table.empty()) {
                    // symbols.txt was built from another mod source than this DLL
                    log::Line(L"not in the offset table: %s", name.c_str());
                }
            } else if (it->second) {
                found[i] = base + *it->second;
            }
        }
        if (!found[i] && !entry.optional) {
            missingRequired = true;
            std::wstring first(entry.symbols[0].string, entry.symbols[0].length);
            log::Line(L"required symbol missing in %s: %s", id.file.c_str(), first.c_str());
        }
    }
    // Address-only entries are filled even when the call fails, as Windhawk does.
    for (size_t i = 0; i < count; i++) {
        if (found[i] && !entries[i].hookFunction && entries[i].pOriginalFunction) {
            *entries[i].pOriginalFunction = found[i];
        }
    }
    if (missingRequired) {
        g_symbolsFailed = true;
        // Only seen when the mod goes on without this module (a later load of it, or a
        // module the mod treats as optional as a whole).
        if (NoteDegraded(id.file.empty() ? std::wstring(L"unknown") : id.file)) {
            RewriteRunningState();
        }
        return FALSE;
    }
    // A failed call leaves none of its own hooks behind.
    std::vector<void*> created;
    for (size_t i = 0; i < count; i++) {
        if (!found[i] || !entries[i].hookFunction) {
            continue;
        }
        if (Set(found[i], entries[i].hookFunction, entries[i].pOriginalFunction)) {
            created.push_back(found[i]);
        } else if (!entries[i].optional) {
            // created is empty when this thread is already inside a hook operation,
            // the one case where taking the lock again would hang.
            Busy busy;
            if (!created.empty() && busy.entered) {
                AcquireSRWLockExclusive(&g_lock);
                RemoveLocked(created);
                ReleaseSRWLockExclusive(&g_lock);
            }
            return FALSE;
        }
    }
    return TRUE;
}

std::vector<std::wstring> Degraded() {
    AcquireSRWLockShared(&g_degradedLock);
    std::vector<std::wstring> out = g_degraded;
    ReleaseSRWLockShared(&g_degradedLock);
    return out;
}

BOOL Remove(void* target) {
    if (!target || g_unloading) {
        return FALSE;
    }
    Busy busy;
    if (!busy.entered) {
        return FALSE;
    }
    AcquireSRWLockExclusive(&g_lock);
    MH_STATUS status = g_unloading ? MH_ERROR_DISABLED : MH_QueueDisableHook(target);
    if (status == MH_OK) {
        g_toRemove.push_back(target);
    }
    ReleaseSRWLockExclusive(&g_lock);
    return status == MH_OK ? TRUE : FALSE;
}

BOOL Apply() {
    if (!g_initialized || g_unloading) {
        return FALSE;
    }
    Busy busy;
    if (!busy.entered) {
        return FALSE;
    }
    AcquireSRWLockExclusive(&g_lock);
    bool ok = !g_unloading && ApplyLocked();
    ReleaseSRWLockExclusive(&g_lock);
    return ok ? TRUE : FALSE;
}

}  // namespace rt::hooks

extern "C" {

BOOL PillRt_SetFunctionHook(void* targetFunction, void* hookFunction, void** originalFunction) {
    return rt::hooks::Set(targetFunction, hookFunction, originalFunction);
}

BOOL PillRt_RemoveFunctionHook(void* targetFunction) {
    return rt::hooks::Remove(targetFunction);
}

BOOL PillRt_ApplyHookOperations(void) {
    return rt::hooks::Apply();
}

BOOL PillRt_HookSymbols(HMODULE module, const WH_SYMBOL_HOOK* symbolHooks, size_t symbolHooksCount,
                        const WH_HOOK_SYMBOLS_OPTIONS*) {
    return rt::hooks::HookSymbols(module, symbolHooks, symbolHooksCount);
}

}  // extern "C"
