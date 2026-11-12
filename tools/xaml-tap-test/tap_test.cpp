// SPDX-License-Identifier: GPL-3.0-only
//
// Test TAP DLL: stands in for a mod DLL that includes taskbar_xaml.h.
#include <windows.h>
#include <cstdio>
#include <cstdarg>

static void TestLog(const wchar_t* format, ...) {
    wchar_t buffer[512];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(buffer, _TRUNCATE, format, args);
    va_end(args);
    char utf8[1024];
    int n = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, utf8, sizeof(utf8) - 2, nullptr, nullptr);
    if (n > 0) {
        utf8[n - 1] = '\n';
        DWORD w;
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), "  [tap] ", 8, &w, nullptr);
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), utf8, n, &w, nullptr);
    }
}
#define TASKBAR_XAML_LOG(...) TestLog(__VA_ARGS__)

#include "taskbar_xaml.h"

extern "C" {

__declspec(dllexport) void TestStart() {
    taskbar_xaml::detail::EnsureStarted();
}

// UI thread. 1 and the content's Name, or 0.
__declspec(dllexport) int TestLookup(HWND window, const wchar_t* prefer, wchar_t* name, int size) {
    try {
        auto e = taskbar_xaml::ContentOf(window, prefer);
        if (!e) return 0;
        wcsncpy_s(name, size, e.Name().c_str(), _TRUNCATE);
        return 1;
    } catch (...) {
        return -1;
    }
}

__declspec(dllexport) int TestPhase() {
    std::lock_guard lock(taskbar_xaml::detail::g_lock);
    return (int)taskbar_xaml::detail::g_phase;
}

__declspec(dllexport) void TestShutdown() {
    taskbar_xaml::Shutdown();
}

__declspec(dllexport) void TestStats(long long* callbacks, long long* qpc, long* sources, long* objects,
                                     DWORD* siteThread, DWORD* callbackThread, HRESULT* lastError, long* islands) {
    auto& s = taskbar_xaml::GetStats();
    *callbacks = s.callbacks;
    *qpc = s.callbackQpc;
    *sources = s.sources;
    *objects = s.objects;
    *siteThread = s.siteThread;
    *callbackThread = s.callbackThread;
    *lastError = s.lastError;
    std::lock_guard lock(taskbar_xaml::detail::g_lock);
    *islands = (long)taskbar_xaml::detail::g_islands.size();
}

}

extern "C" {
// Re-advise the callback this DLL already has.
__declspec(dllexport) HRESULT TestReadvise() {
    using namespace taskbar_xaml::detail;
    IVisualTreeService* s; IVisualTreeServiceCallback2* c;
    { std::lock_guard lock(g_lock); s = g_service; c = g_callback; }
    if (!s || !c) return E_FAIL;
    return s->AdviseVisualTreeChange(c);
}
__declspec(dllexport) void* TestService() {
    std::lock_guard lock(taskbar_xaml::detail::g_lock);
    return taskbar_xaml::detail::g_service;
}
// HitTest a screen rect; for each returned handle, its type name and root window of its XamlRoot content.
__declspec(dllexport) int TestHitTest(RECT r, wchar_t* out, int size) {
    using namespace taskbar_xaml::detail;
    IXamlDiagnostics* d;
    { std::lock_guard lock(g_lock); d = g_diag; }
    if (!d) return -1;
    unsigned int count = 0;
    InstanceHandle* handles = nullptr;
    HRESULT hr = d->HitTest(r, &count, &handles);
    int n = swprintf(out, size, L"hr %08X count %u:", (unsigned)hr, count);
    for (unsigned i = 0; i < count && n < size - 100; i++) {
        winrt::com_ptr<IInspectable> o;
        d->GetIInspectableFromHandle(handles[i], o.put());
        std::wstring name = o ? std::wstring(winrt::get_class_name(o.as<winrt::Windows::Foundation::IInspectable>())) : L"?";
        std::wstring root;
        if (auto e = o.try_as<winrt::Windows::UI::Xaml::FrameworkElement>()) {
            try { auto c = e.XamlRoot() ? e.XamlRoot().Content().try_as<winrt::Windows::UI::Xaml::FrameworkElement>() : nullptr; if (c) root = c.Name(); } catch (...) { root = L"!"; }
        }
        if (i < 12) n += swprintf(out + n, size - n, L" %ls[%ls]", name.c_str() + (name.rfind(L'.') + 1), root.c_str());
    }
    if (handles) CoTaskMemFree(handles);
    return (int)count;
}
}
extern "C" __declspec(dllexport) HRESULT TestUnadvise() {
    using namespace taskbar_xaml::detail;
    IVisualTreeService* s; IVisualTreeServiceCallback2* c;
    { std::lock_guard lock(g_lock); s = g_service; c = g_callback; }
    if (!s || !c) return E_FAIL;
    return s->UnadviseVisualTreeChange(c);
}
