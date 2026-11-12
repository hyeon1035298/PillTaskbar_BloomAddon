// SPDX-License-Identifier: GPL-3.0-only
//
// Finds the XAML content a top-level window hosts, the taskbar's above all, with no
// symbols: XAML Diagnostics (InitializeXamlDiagnosticsEx, the documented API that
// Visual Studio's Live Visual Tree uses) reports every DesktopWindowXamlSource in
// the process, and IDesktopWindowXamlSourceNative names the window each one sits in.
// No address inside a system DLL is used, so a Windows update without published
// symbols leaves this path working.
//
// The mod DLL is its own TAP (the object XAML Diagnostics loads by path and CLSID):
// this header defines and exports DllGetClassObject, so include it in exactly one
// source file of a DLL that exports no other DllGetClassObject.
//
//   taskbar_xaml::TaskbarElementByXaml(hwnd)  on the taskbar thread: the content of
//       the taskbar window's XAML island, or nullptr while the diagnostics have not
//       reported it yet (the first call starts them; call again later).
//   taskbar_xaml::Shutdown()  from a thread that is not a XAML thread (Wh_ModUninit),
//       before the DLL is freed: stops the reports and drops every reference to XAML.
//
// Nothing here blocks a XAML thread. The start (InitializeXamlDiagnosticsEx and
// AdviseVisualTreeChange) runs on a thread of its own, since both wait for the XAML
// threads to answer.
//
// NOT USED BY ANY MOD YET, because of how system XAML treats connections (measured
// with tools/xaml-tap-test):
// - One live connection per process. Every InitializeXamlDiagnosticsEx, advised or
//   not, takes the tree-change reports from the connection before it for good; that
//   one keeps what it already has but hears of no new element again, and advising
//   anew does not bring the reports back. Another TAP in explorer (Windhawk's Taskbar
//   Styler, TranslucentTB, or a second mod using this header) stops seeing new
//   elements once we connect after it.
// - Each connection makes XAML LoadLibrary this DLL once more, and XAML never frees
//   it. Shutdown gives those references back once XAML has released all our objects,
//   so the DLL unmaps; otherwise it pins the DLL.

#pragma once

#include <windows.h>

#include <ocidl.h>
#include <unknwn.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <xamlom.h>

#include <atomic>
#include <cwchar>
#include <mutex>
#include <vector>

#undef GetCurrentTime  // winbase.h's macro breaks the C++/WinRT XAML headers
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#ifndef TASKBAR_XAML_LOG
#ifdef Wh_Log
#define TASKBAR_XAML_LOG(...) Wh_Log(__VA_ARGS__)
#else
#define TASKBAR_XAML_LOG(...) ((void)0)
#endif
#endif

namespace taskbar_xaml {

// Counters a test reads; the mod has no use for them.
struct Stats {
    std::atomic<long long> callbacks{0};   // OnVisualTreeChange calls
    std::atomic<long long> callbackQpc{0}; // QPC ticks spent inside them
    std::atomic<long> sources{0};          // DesktopWindowXamlSource adds seen
    std::atomic<long> starts{0};           // InitializeXamlDiagnosticsEx calls that succeeded
    std::atomic<long> objects{0};          // our COM objects XAML still holds
    std::atomic<DWORD> siteThread{0};      // the thread SetSite last ran on
    std::atomic<DWORD> callbackThread{0};  // the thread OnVisualTreeChange last ran on
    HRESULT lastError = S_OK;
};

namespace detail {

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;

constexpr PCWSTR kSourceType = L"Windows.UI.Xaml.Hosting.DesktopWindowXamlSource";
constexpr DWORD kSiteWaitMs = 5000;   // InitializeXamlDiagnosticsEx to SetSite
constexpr DWORD kStartWaitMs = 10000; // Shutdown's wait for the start thread
constexpr DWORD kReleaseWaitMs = 2000;// Shutdown's wait for XAML to let go of our objects
constexpr int kMaxStarts = 5;         // tries before the route gives up for this load
constexpr ULONGLONG kRetryMs = 5000;  // between tries
constexpr int kMaxEndpoints = 100;    // VisualDiagConnection names tried

enum class Phase { off, starting, ready, failed };

struct Island {
    InstanceHandle handle;
    winrt::weak_ref<wuxh::DesktopWindowXamlSource> source;
};

inline Stats g_stats;
inline std::mutex g_lock;  // everything below
inline Phase g_phase = Phase::off;
inline bool g_stopping = false;
inline int g_tries = 0;
inline ULONGLONG g_lastTry = 0;
inline CLSID g_clsid{};
inline HANDLE g_thread = nullptr;
inline HANDLE g_siteSet = nullptr;  // set by SetSite
inline IXamlDiagnostics* g_diag = nullptr;
inline IVisualTreeService* g_service = nullptr;
inline IVisualTreeServiceCallback2* g_callback = nullptr;
inline std::vector<Island> g_islands;
inline int g_xamlRefs = 0;  // references XAML took on this DLL, one per connection

inline HMODULE OwnModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OwnModule), &module);
    return module;
}

// Keeps this DLL mapped for good: the way out when XAML may still call into it.
inline void Pin(PCWSTR why) {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                       reinterpret_cast<LPCWSTR>(&OwnModule), &module);
    TASKBAR_XAML_LOG(L"XAML route: %s; the DLL stays mapped", why);
}

// A COM object of ours: counted, so Shutdown knows when XAML has let go of all of them.
template <class Self, class... Interfaces>
class Object : public Interfaces... {
public:
    Object() { g_stats.objects++; }
    virtual ~Object() { g_stats.objects--; }
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    STDMETHODIMP QueryInterface(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (iid == __uuidof(IUnknown)) {
            *out = static_cast<IUnknown*>(static_cast<typename First<Interfaces...>::type*>(this));
        } else if (!(... || Match<Interfaces>(iid, out))) {
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++refs_; }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG left = --refs_;
        if (!left) delete static_cast<Self*>(this);
        return left;
    }

private:
    template <class T, class...>
    struct First {
        using type = T;
    };
    template <class I>
    bool Match(REFIID iid, void** out) {
        if (iid != __uuidof(I)) return false;
        *out = static_cast<I*>(this);
        return true;
    }
    std::atomic<ULONG> refs_{1};
};

inline void ForgetIsland(InstanceHandle handle) {
    std::lock_guard lock(g_lock);
    std::erase_if(g_islands, [&](const Island& i) { return i.handle == handle; });
}

// XAML thread: called once per element while advising, then on every change.
class Callback final : public Object<Callback, IVisualTreeServiceCallback2> {
public:
    STDMETHODIMP QueryInterface(REFIID iid, void** out) override {
        if (out && iid == __uuidof(IVisualTreeServiceCallback)) {
            *out = static_cast<IVisualTreeServiceCallback*>(this);
            AddRef();
            return S_OK;
        }
        return Object::QueryInterface(iid, out);
    }
    STDMETHODIMP OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutation) override {
        LARGE_INTEGER t0, t1;
        QueryPerformanceCounter(&t0);
        bool source = element.Type && wcscmp(element.Type, kSourceType) == 0;
        if (source && mutation == Add) {
            Added(element.Handle);
        } else if (source) {
            ForgetIsland(element.Handle);
        }
        QueryPerformanceCounter(&t1);
        g_stats.callbacks++;
        g_stats.callbackThread = GetCurrentThreadId();
        g_stats.callbackQpc += t1.QuadPart - t0.QuadPart;
        return S_OK;
    }
    STDMETHODIMP OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) override { return S_OK; }

private:
    static void Added(InstanceHandle handle) {
        g_stats.sources++;
        IXamlDiagnostics* diag;
        {
            std::lock_guard lock(g_lock);
            diag = g_diag;
            if (!diag || g_stopping) return;
            diag->AddRef();
        }
        winrt::com_ptr<IInspectable> object;
        HRESULT hr = diag->GetIInspectableFromHandle(handle, object.put());
        diag->Release();
        if (FAILED(hr) || !object) return;
        try {
            auto source = object.as<wuxh::DesktopWindowXamlSource>();
            Island island{handle, winrt::make_weak(source)};
            std::lock_guard lock(g_lock);
            std::erase_if(g_islands, [&](const Island& i) { return i.handle == handle; });
            g_islands.push_back(std::move(island));
        } catch (...) {
        }
    }
};

// The TAP itself. XAML hands it the diagnostics site, on a thread of its choosing.
class Tap final : public Object<Tap, IObjectWithSite> {
public:
    STDMETHODIMP SetSite(IUnknown* site) override {
        g_stats.siteThread = GetCurrentThreadId();
        IXamlDiagnostics* diag = nullptr;
        if (site) site->QueryInterface(__uuidof(IXamlDiagnostics), reinterpret_cast<void**>(&diag));
        IVisualTreeService* service = nullptr;
        if (site) site->QueryInterface(__uuidof(IVisualTreeService3), reinterpret_cast<void**>(&service));
        if (site && !service) site->QueryInterface(__uuidof(IVisualTreeService), reinterpret_cast<void**>(&service));
        IXamlDiagnostics* oldDiag;
        IVisualTreeService* oldService;
        {
            std::lock_guard lock(g_lock);
            oldDiag = std::exchange(g_diag, diag);
            oldService = std::exchange(g_service, service);
        }
        if (oldDiag) oldDiag->Release();
        if (oldService) oldService->Release();
        if (site_) site_->Release();
        site_ = site;
        if (site_) site_->AddRef();
        if (g_siteSet) SetEvent(g_siteSet);
        return S_OK;
    }
    STDMETHODIMP GetSite(REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        return site_ ? site_->QueryInterface(iid, out) : E_FAIL;
    }
    ~Tap() override {
        if (site_) site_->Release();
    }

private:
    IUnknown* site_ = nullptr;
};

class Factory final : public Object<Factory, IClassFactory> {
public:
    STDMETHODIMP CreateInstance(IUnknown* outer, REFIID iid, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (outer) return CLASS_E_NOAGGREGATION;
        auto* tap = new (std::nothrow) Tap;
        if (!tap) return E_OUTOFMEMORY;
        HRESULT hr = tap->QueryInterface(iid, out);
        tap->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL) override { return S_OK; }
};

inline void EndStart(Phase phase, HRESULT hr) {
    std::lock_guard lock(g_lock);
    g_phase = phase;
    g_stats.lastError = hr;
}

inline DWORD WINAPI StartThread(void*) {
    using Init = HRESULT(WINAPI*)(PCWSTR, DWORD, PCWSTR, PCWSTR, CLSID, PCWSTR);
    HMODULE xaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
    auto init = xaml ? reinterpret_cast<Init>(GetProcAddress(xaml, "InitializeXamlDiagnosticsEx")) : nullptr;
    wchar_t path[MAX_PATH];
    DWORD length = GetModuleFileNameW(OwnModule(), path, ARRAYSIZE(path));
    if (!init || !length || length >= ARRAYSIZE(path)) {
        TASKBAR_XAML_LOG(L"XAML route: no XAML Diagnostics in this process");
        EndStart(Phase::failed, E_NOTIMPL);
        return 0;
    }
    CLSID clsid;
    {
        std::lock_guard lock(g_lock);
        clsid = g_clsid;
    }
    // XAML names its endpoints VisualDiagConnection1, 2, ...; the first one that exists
    // takes the TAP.
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (int i = 1; i <= kMaxEndpoints && hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND); i++) {
        wchar_t endpoint[64];
        swprintf(endpoint, ARRAYSIZE(endpoint), L"VisualDiagConnection%d", i);
        hr = init(endpoint, GetCurrentProcessId(), L"", path, clsid, nullptr);
        if (SUCCEEDED(hr)) TASKBAR_XAML_LOG(L"XAML route: connected through %s", endpoint);
    }
    if (FAILED(hr)) {
        TASKBAR_XAML_LOG(L"XAML route: InitializeXamlDiagnosticsEx failed (%08X)", hr);
        EndStart(Phase::failed, hr);
        return 0;
    }
    g_stats.starts++;
    {
        std::lock_guard lock(g_lock);
        g_xamlRefs++;
    }
    if (WaitForSingleObject(g_siteSet, kSiteWaitMs) != WAIT_OBJECT_0) {
        TASKBAR_XAML_LOG(L"XAML route: the diagnostics never called the TAP");
        EndStart(Phase::failed, HRESULT_FROM_WIN32(ERROR_TIMEOUT));
        return 0;
    }
    IVisualTreeService* service;
    {
        std::lock_guard lock(g_lock);
        service = g_service;
        if (service && !g_stopping) {
            service->AddRef();
            g_callback = new (std::nothrow) Callback;
        } else {
            service = nullptr;
        }
    }
    if (!service || !g_callback) {
        if (service) service->Release();
        EndStart(Phase::failed, E_NOINTERFACE);
        return 0;
    }
    // Reports every element there is before it returns, then every change.
    hr = service->AdviseVisualTreeChange(g_callback);
    service->Release();
    if (FAILED(hr)) TASKBAR_XAML_LOG(L"XAML route: AdviseVisualTreeChange failed (%08X)", hr);
    EndStart(SUCCEEDED(hr) ? Phase::ready : Phase::failed, hr);
    return 0;
}

// Any thread. Starts the diagnostics unless they run, are starting or gave up.
inline void EnsureStarted() {
    std::lock_guard lock(g_lock);
    if (g_stopping || g_phase == Phase::ready || g_phase == Phase::starting) return;
    if (g_phase == Phase::failed && (g_tries >= kMaxStarts || GetTickCount64() - g_lastTry < kRetryMs)) return;
    // A failed try may have left a site behind; the next one starts on a clean slate.
    if (g_thread) {
        if (WaitForSingleObject(g_thread, 0) != WAIT_OBJECT_0) return;
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_diag || g_service) return;  // a site from a failed try still around: not twice
    if (!g_siteSet) g_siteSet = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_siteSet || FAILED(CoCreateGuid(&g_clsid))) return;
    ResetEvent(g_siteSet);
    g_tries++;
    g_lastTry = GetTickCount64();
    g_phase = Phase::starting;
    g_thread = CreateThread(nullptr, 0, StartThread, nullptr, 0, nullptr);
    if (!g_thread) g_phase = Phase::failed;
}

// The window an island's content shows in.
inline HWND RootOf(const wuxh::DesktopWindowXamlSource& source) {
    HWND island = nullptr;
    if (auto native = source.try_as<IDesktopWindowXamlSourceNative>()) native->get_WindowHandle(&island);
    return island ? GetAncestor(island, GA_ROOT) : nullptr;
}

inline bool Contains(const wux::DependencyObject& e, PCWSTR className, int depth) {
    if (!e) return false;
    if (winrt::get_class_name(e) == className) return true;
    if (depth <= 0) return false;
    int n = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(e);
    for (int i = 0; i < n; i++)
        if (Contains(winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(e, i), className, depth - 1))
            return true;
    return false;
}

}  // namespace detail

inline Stats& GetStats() {
    return detail::g_stats;
}

// The thread that owns the island (the taskbar thread for a taskbar). The content of
// the one XAML island hosted in `window`, a top-level window. When the window hosts
// several, the one with an element of class `prefer` within a few levels of its
// content; nullptr when that does not settle it either, or before the diagnostics
// have reported the window's islands.
inline winrt::Windows::UI::Xaml::FrameworkElement ContentOf(HWND window, PCWSTR prefer = nullptr) {
    using namespace detail;
    if (!window) return nullptr;
    EnsureStarted();
    std::vector<winrt::weak_ref<wuxh::DesktopWindowXamlSource>> sources;
    {
        std::lock_guard lock(g_lock);
        if (g_phase != Phase::ready || g_stopping) return nullptr;
        for (auto& island : g_islands) sources.push_back(island.source);
    }
    std::vector<wux::FrameworkElement> found;
    for (auto& weak : sources) {
        try {
            auto source = weak.get();
            if (!source || RootOf(source) != window) continue;
            if (auto content = source.Content().try_as<wux::FrameworkElement>()) found.push_back(content);
        } catch (...) {
            // Another thread's island, or one closed while we looked.
        }
    }
    if (found.size() > 1 && prefer) {
        std::erase_if(found, [&](auto& e) {
            try {
                return !Contains(e, prefer, 4);
            } catch (...) {
                return true;
            }
        });
    }
    return found.size() == 1 ? found[0] : nullptr;
}

// Taskbar thread. The XAML content of a Shell_TrayWnd or Shell_SecondaryTrayWnd.
inline winrt::Windows::UI::Xaml::FrameworkElement TaskbarElementByXaml(HWND taskbar) {
    return ContentOf(taskbar, L"Taskbar.TaskbarFrame");
}

// Not on a XAML thread. Ends the reports and lets go of XAML; safe to call when
// nothing was started, and the route can start again afterwards. When XAML does
// not give our objects back in time, the DLL is pinned rather than freed under it.
inline void Shutdown() {
    using namespace detail;
    HANDLE thread;
    {
        std::lock_guard lock(g_lock);
        g_stopping = true;
        thread = std::exchange(g_thread, nullptr);
    }
    if (thread) {
        DWORD waited = WaitForSingleObject(thread, kStartWaitMs);
        CloseHandle(thread);
        if (waited != WAIT_OBJECT_0) {
            Pin(L"the start did not finish");
            return;
        }
    }
    IVisualTreeService* service;
    IXamlDiagnostics* diag;
    IVisualTreeServiceCallback2* callback;
    std::vector<Island> islands;
    {
        std::lock_guard lock(g_lock);
        service = std::exchange(g_service, nullptr);
        diag = std::exchange(g_diag, nullptr);
        callback = std::exchange(g_callback, nullptr);
        islands.swap(g_islands);
    }
    if (service && callback) service->UnadviseVisualTreeChange(callback);
    if (callback) callback->Release();
    if (service) service->Release();
    if (diag) diag->Release();
    islands.clear();
    // XAML drops the TAP and the callback on its own schedule.
    for (DWORD waited = 0; g_stats.objects > 0 && waited < kReleaseWaitMs; waited += 10) Sleep(10);
    int refs;
    {
        std::lock_guard lock(g_lock);
        g_phase = Phase::off;
        g_tries = 0;
        g_stopping = false;
        refs = std::exchange(g_xamlRefs, 0);
        if (g_siteSet) CloseHandle(std::exchange(g_siteSet, nullptr));
    }
    if (g_stats.objects > 0) {
        Pin(L"XAML still holds the TAP");
        return;
    }
    // Each connection made XAML load this DLL once more, and XAML never frees it. With
    // none of our objects left in XAML's hands, those references are only what keeps
    // the image mapped after the loader's own FreeLibrary; give them back.
    HMODULE self = OwnModule();
    for (int i = 0; i < refs && self; i++) FreeLibrary(self);
}

}  // namespace taskbar_xaml

// XAML Diagnostics asks the TAP DLL for its class by the CLSID the start passed.
// combaseapi.h declares it without the export.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
extern "C" __declspec(dllexport) HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID iid, void** out) {
    using namespace taskbar_xaml::detail;
    if (!out) return E_POINTER;
    *out = nullptr;
    CLSID current;
    {
        std::lock_guard lock(g_lock);
        current = g_clsid;
    }
    if (clsid != current || current == CLSID{}) return CLASS_E_CLASSNOTAVAILABLE;
    auto* factory = new (std::nothrow) Factory;
    if (!factory) return E_OUTOFMEMORY;
    HRESULT hr = factory->QueryInterface(iid, out);
    factory->Release();
    return hr;
}
#pragma clang diagnostic pop
