// SPDX-License-Identifier: GPL-3.0-only

#include "inject.h"

namespace host {

std::wstring ObjectName(const wchar_t* format, DWORD pid) {
    return Format(format, pid);
}

bool RuntimeLoadedIn(DWORD pid) {
    Handle ev(OpenEventW(SYNCHRONIZE, FALSE, ObjectName(addon::kLoadedEvent, pid).c_str()));
    return ev && WaitForSingleObject(ev.get(), 0) == WAIT_OBJECT_0;
}

bool OpenExplorer(const Taskbar& t, Explorer& e) {
    e = Explorer{};
    e.taskbar = t;
    e.process.Reset(OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, t.pid));
    if (!e.process) return false;
    e.created = ProcessCreationTime(e.process.get());
    return e.created != 0;
}

bool OpenEvents(Explorer& e, bool create) {
    const wchar_t* formats[] = {addon::kLoadedEvent, addon::kUnloadEvent, addon::kUnloadedEvent};
    Handle* slots[] = {&e.loaded, &e.unload, &e.unloaded};
    for (int i = 0; i < 3; i++) {
        std::wstring name = ObjectName(formats[i], e.taskbar.pid);
        if (create) {
            // Manual reset. An older runtime's events can outlive it under the same
            // names, so start them all off.
            slots[i]->Reset(CreateEventW(nullptr, TRUE, FALSE, name.c_str()));
            if (*slots[i]) ResetEvent(slots[i]->get());
        } else {
            slots[i]->Reset(OpenEventW(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, name.c_str()));
        }
        if (!*slots[i]) return false;
    }
    return true;
}

bool CreateConfig(Explorer& e, const addon::Config& config) {
    std::wstring name = ObjectName(addon::kConfigMapping, e.taskbar.pid);
    e.mapping.Reset(CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(addon::Config),
                                       name.c_str()));
    if (!e.mapping) return false;
    void* view = MapViewOfFile(e.mapping.get(), FILE_MAP_WRITE, 0, 0, sizeof(addon::Config));
    if (!view) return false;
    memcpy(view, &config, sizeof(config));
    UnmapViewOfFile(view);
    return true;
}

HookResult HookUntilLoaded(Explorer& e, const std::wstring& runtimePath, const WaitFn& wait, DWORD& error) {
    error = 0;
    // The hook procedure has to live in a DLL the system can map into explorer, so
    // the host loads the same file to hand SetWindowsHookEx its address.
    HMODULE runtime = LoadLibraryExW(runtimePath.c_str(), nullptr,
                                     LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!runtime) {
        error = GetLastError();
        return HookResult::HookFailed;
    }
    auto proc = (HOOKPROC)(void*)GetProcAddress(runtime, addon::kHookProcName);
    HHOOK hook = proc ? SetWindowsHookExW(WH_GETMESSAGE, proc, runtime, e.taskbar.tid) : nullptr;
    if (!hook) {
        error = proc ? GetLastError() : ERROR_PROC_NOT_FOUND;
        FreeLibrary(runtime);
        return HookResult::HookFailed;
    }
    HookResult result = HookResult::NotLoaded;
    DWORD start = GetTickCount();
    while (GetTickCount() - start < 10000) {
        // The hook only runs when the thread takes a message from its queue.
        PostMessageW(e.taskbar.tray, WM_NULL, 0, 0);
        int r = wait({e.loaded.get(), e.process.get()}, 500, true);
        if (r == 0) {
            result = HookResult::Loaded;
            break;
        }
        if (r == 1) {
            result = HookResult::ExplorerGone;
            break;
        }
        if (r == kWaitStop) {
            result = HookResult::Stopped;
            break;
        }
    }
    // The runtime holds its own reference once loaded, so dropping the hook and the
    // host's copy does not take it out of explorer.
    UnhookWindowsHookEx(hook);
    FreeLibrary(runtime);
    if (result == HookResult::Loaded) e.runtimeIn = true;
    // A stop during the wait still has to unload a runtime that arrived just then.
    if (result == HookResult::Stopped && WaitForSingleObject(e.loaded.get(), 0) == WAIT_OBJECT_0) e.runtimeIn = true;
    return result;
}

bool UnloadRuntime(Explorer& e, DWORD timeoutMs, const WaitFn& wait) {
    if (!e.unload || !e.unloaded) return false;
    SetEvent(e.unload.get());
    // Unloading is what a stop asks for, so a stop does not cut this wait short. An
    // explorer that exits takes the runtime with it.
    int r = wait({e.unloaded.get(), e.process.get()}, timeoutMs, false);
    if (r != 0 && r != 1) return false;
    e.runtimeIn = false;
    return true;
}

}  // namespace host
