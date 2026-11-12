// SPDX-License-Identifier: GPL-3.0-only
//
// Test app: several top-level windows with system XAML islands, and a script that
// loads test TAP DLLs (stand-ins for mod DLLs), maps windows to island content,
// shuts the TAP down, frees the DLL and repeats.
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <unknwn.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include <cstdio>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;

static HWND g_main;  // the first test window; runs work on the UI thread
static DWORD g_uiThread;
static RECT g_area;
static std::wstring g_dir;

struct TestWindow {
    HWND hwnd;
    std::vector<wuxh::DesktopWindowXamlSource> sources;
    std::vector<HWND> islands;
};
static std::vector<TestWindow> g_windows;
static wuxc::StackPanel g_bulk{nullptr};  // where element-count tests add children

static void Log(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    fputs(buffer, stdout);
    fputc('\n', stdout);
    fflush(stdout);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_APP) {
        (*(std::function<void()>*)lp)();
        return 0;
    }
    if (msg == WM_SIZE) {
        for (auto& w : g_windows) {
            if (w.hwnd != hwnd) continue;
            RECT r;
            GetClientRect(hwnd, &r);
            int n = (int)w.islands.size(), x = 0, each = n ? (r.right / n) : 0;
            for (HWND island : w.islands) {
                SetWindowPos(island, nullptr, x, 0, each, r.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
                x += each;
            }
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void RunOnUi(std::function<void()> work) {
    SendMessageW(g_main, WM_APP, 0, (LPARAM)&work);
}

// UI thread. A window with one island per name; content is a Grid named after it.
static HWND MakeWindow(int index, std::vector<std::wstring> names, bool textBox) {
    int x = g_area.left + 20 + (index % 4) * 330, y = g_area.top + 20 + (index / 4) * 240;
    wchar_t title[64];
    swprintf(title, 64, L"xaml-tap test %d", index);
    HWND hwnd = CreateWindowExW(WS_EX_NOACTIVATE, L"XamlTapTest", title, WS_OVERLAPPEDWINDOW, x, y, 320, 220, nullptr,
                                nullptr, GetModuleHandleW(nullptr), nullptr);
    TestWindow w{hwnd, {}, {}};
    for (size_t i = 0; i < names.size(); i++) {
        wuxh::DesktopWindowXamlSource source;
        auto native = source.as<IDesktopWindowXamlSourceNative>();
        winrt::check_hresult(native->AttachToWindow(hwnd));
        HWND island = nullptr;
        native->get_WindowHandle(&island);
        wuxc::Grid grid;
        grid.Name(names[i]);
        grid.Background(wux::Media::SolidColorBrush(winrt::Windows::UI::ColorHelper::FromArgb(255, 40, 40, 60)));
        wuxc::StackPanel panel;
        wuxc::TextBlock text;
        text.Text(names[i]);
        panel.Children().Append(text);
        if (textBox && i == 0) panel.Children().Append(wuxc::TextBox());
        grid.Children().Append(panel);
        if (index == 1 && i == 0) g_bulk = panel;
        source.Content(grid);
        w.sources.push_back(source);
        w.islands.push_back(island);
    }
    g_windows.push_back(w);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    SendMessageW(hwnd, WM_SIZE, 0, 0);
    return hwnd;
}

static void CloseTestWindow(HWND hwnd) {
    for (size_t i = 0; i < g_windows.size(); i++) {
        if (g_windows[i].hwnd != hwnd) continue;
        for (auto& s : g_windows[i].sources) s.Close();
        DestroyWindow(hwnd);
        g_windows.erase(g_windows.begin() + i);
        return;
    }
}

struct Tap {
    HMODULE module = nullptr;
    std::wstring path;
    void (*start)();
    int (*lookup)(HWND, const wchar_t*, wchar_t*, int);
    int (*phase)();
    void (*shutdown)();
    void (*stats)(long long*, long long*, long*, long*, DWORD*, DWORD*, HRESULT*, long*);
    bool Load(const wchar_t* file) {
        path = g_dir + file;
        module = LoadLibraryW(path.c_str());
        if (!module) return false;
        start = (decltype(start))GetProcAddress(module, "TestStart");
        lookup = (decltype(lookup))GetProcAddress(module, "TestLookup");
        phase = (decltype(phase))GetProcAddress(module, "TestPhase");
        shutdown = (decltype(shutdown))GetProcAddress(module, "TestShutdown");
        stats = (decltype(stats))GetProcAddress(module, "TestStats");
        return start && lookup && phase && shutdown && stats;
    }
    // UI thread lookup; name or "" (none) or "!" (error).
    std::wstring Lookup(HWND hwnd, const wchar_t* prefer = nullptr) {
        std::wstring result;
        RunOnUi([&] {
            wchar_t name[128] = L"";
            int r = lookup(hwnd, prefer, name, 128);
            result = r == 1 ? name : (r == 0 ? L"" : L"!");
        });
        return result;
    }
    bool Mapped() { return GetModuleHandleW(path.c_str()) != nullptr; }
};

static double Ms(LARGE_INTEGER a, LARGE_INTEGER b) {
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    return (b.QuadPart - a.QuadPart) * 1000.0 / f.QuadPart;
}

static double Now() {
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return t.QuadPart * 1000.0 / f.QuadPart;
}

// UI thread: adds and removes `count` TextBlocks in g_bulk, with layout; ms.
static double Churn(int count) {
    double ms = 0;
    RunOnUi([&] {
        double t0 = Now();
        auto children = g_bulk.Children();
        uint32_t base = children.Size();
        for (int i = 0; i < count; i++) {
            wuxc::TextBlock t;
            t.Text(L"x");
            children.Append(t);
        }
        g_bulk.UpdateLayout();
        while (children.Size() > base) children.RemoveAtEnd();
        g_bulk.UpdateLayout();
        ms = Now() - t0;
    });
    return ms;
}

static void AddPermanent(int count) {
    RunOnUi([&] {
        auto children = g_bulk.Children();
        for (int i = 0; i < count; i++) {
            wuxc::Border b;
            wuxc::TextBlock t;
            t.Text(L"e");
            b.Child(t);
            children.Append(b);
        }
        g_bulk.UpdateLayout();
    });
}

struct Usage {
    DWORD handles, threads, gdi, user;
    SIZE_T priv;
};
static Usage GetUsage() {
    Usage u{};
    GetProcessHandleCount(GetCurrentProcess(), &u.handles);
    PROCESS_MEMORY_COUNTERS_EX pmc{sizeof(pmc)};
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    u.priv = pmc.PrivateUsage;
    u.gdi = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    u.user = GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS);
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te{sizeof(te)};
    for (BOOL ok = Thread32First(snap, &te); ok; ok = Thread32Next(snap, &te))
        if (te.th32OwnerProcessID == GetCurrentProcessId()) u.threads++;
    CloseHandle(snap);
    return u;
}
static void LogUsage(const char* what) {
    Usage u = GetUsage();
    Log("%s: handles %lu, threads %lu, private %.1f MB, gdi %lu, user %lu", what, u.handles, u.threads,
        u.priv / 1048576.0, u.gdi, u.user);
}

// Starts the TAP and waits until it maps `hwnd`; ms or -1.
static double WaitReady(Tap& tap, HWND hwnd, const wchar_t* expected, DWORD timeoutMs = 10000) {
    double t0 = Now();
    for (;;) {
        std::wstring name = tap.Lookup(hwnd);  // the first call starts it
        if (name == expected) return Now() - t0;
        if (Now() - t0 > timeoutMs) {
            Log("  wait: phase %d, got '%ls'", tap.phase(), name.c_str());
            return -1;
        }
        Sleep(5);
    }
}

static int g_failures = 0;
static void Check(bool ok, const char* what) {
    if (!ok) g_failures++;
    Log("  %s %s", ok ? "ok  " : "FAIL", what);
}

static void Stats(Tap& tap, const char* label) {
    long long callbacks, qpc;
    long sources, objects, islands;
    DWORD siteThread, callbackThread;
    HRESULT err;
    tap.stats(&callbacks, &qpc, &sources, &objects, &siteThread, &callbackThread, &err, &islands);
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    Log("  %s: callbacks %lld (%.2f ms inside), sources %ld, islands %ld, objects %ld, site thread %s, callback "
        "thread %s, last hr %08X",
        label, callbacks, qpc * 1000.0 / f.QuadPart, sources, islands, objects,
        siteThread == g_uiThread ? "UI" : (siteThread ? "other" : "-"),
        callbackThread == g_uiThread ? "UI" : (callbackThread ? "other" : "-"), (unsigned)err);
}

// Checks the mapping of every test window through one TAP.
static void CheckMapping(Tap& tap) {
    for (int i = 0; i < 2; i++) {
        wchar_t expected[32];
        swprintf(expected, 32, L"Content%d", i);
        std::wstring got = tap.Lookup(g_windows[i].hwnd);
        char what[160];
        snprintf(what, sizeof(what), "window %d -> %ls (got '%ls')", i, expected, got.c_str());
        Check(got == expected, what);
    }
    std::wstring two = tap.Lookup(g_windows[2].hwnd, L"Windows.UI.Xaml.Controls.TextBox");
    char what[160];
    snprintf(what, sizeof(what), "window 2 (two islands) with prefer -> Content2 (got '%ls')", two.c_str());
    Check(two == L"Content2", what);
    std::wstring none = tap.Lookup(g_windows[2].hwnd);
    snprintf(what, sizeof(what), "window 2 (two islands) without prefer -> none (got '%ls')", none.c_str());
    Check(none.empty(), what);
    // An island's own child window and a window with no XAML map to nothing.
    Check(tap.Lookup(g_windows[0].islands[0]).empty(), "island child HWND -> none");
    Check(tap.Lookup(GetDesktopWindow()).empty(), "desktop window -> none");
}


static std::wstring g_mode = L"cycles";

static long long Callbacks(Tap& t) {
    long long cb, q;
    long src, obj, isl;
    DWORD st, ct;
    HRESULT err;
    t.stats(&cb, &q, &src, &obj, &st, &ct, &err, &isl);
    return cb;
}
static long Objects(Tap& t) {
    long long cb, q;
    long src, obj, isl;
    DWORD st, ct;
    HRESULT err;
    t.stats(&cb, &q, &src, &obj, &st, &ct, &err, &isl);
    return obj;
}
static int ExtraFrees(const std::wstring& path) {
    int extra = 0;
    while (GetModuleHandleW(path.c_str()) && extra < 10) {
        FreeLibrary(GetModuleHandleW(path.c_str()));
        extra++;
    }
    return extra;
}
static double Median(std::vector<double> v) {
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}
static double ChurnMedian(int count, int runs = 7) {
    std::vector<double> v;
    for (int i = 0; i < runs; i++) v.push_back(Churn(count));
    return Median(v);
}

static void ModeRefs() {
    Tap tap;
    tap.Load(L"tapA.dll");
    for (int i = 0; i < 3; i++) {
        double r = WaitReady(tap, g_windows[0].hwnd, L"Content0");
        Log("start %d ready %.1f ms", i, r);
        tap.shutdown();
        Log("  objects after shutdown %ld", Objects(tap));
    }
    FreeLibrary(tap.module);
    Log("after 3 starts in one load and our FreeLibrary: %d more FreeLibrary calls to unmap", ExtraFrees(tap.path));
}

static void ModeConcurrent() {
    Tap t[3];
    const wchar_t* files[3] = {L"tapA.dll", L"tapB.dll", L"tapC.dll"};
    bool on[3] = {};
    auto report = [&](const char* step) {
        long long before[3];
        for (int i = 0; i < 3; i++) before[i] = on[i] ? Callbacks(t[i]) : 0;
        Churn(100);
        Sleep(50);
        char line[256];
        int n = snprintf(line, sizeof(line), "%-34s live callbacks for 100 adds+removes:", step);
        for (int i = 0; i < 3; i++)
            n += snprintf(line + n, sizeof(line) - n, " %c=%s", 'A' + i,
                          on[i] ? std::to_string(Callbacks(t[i]) - before[i]).c_str() : "-");
        Log("%s", line);
    };
    auto connect = [&](int i) {
        t[i].Load(files[i]);
        double r = WaitReady(t[i], g_windows[0].hwnd, L"Content0");
        on[i] = r >= 0;
        Log("%c connected: %s (%.1f ms, %lld callbacks in its first report)", 'A' + i, on[i] ? "yes" : "NO", r,
            Callbacks(t[i]));
    };
    auto leave = [&](int i) {
        t[i].shutdown();
        on[i] = false;
        Log("%c shut down, objects left %ld", 'A' + i, Objects(t[i]));
    };
    auto seesNewWindow = [&](int index) {
        HWND late = nullptr;
        wchar_t name[32];
        swprintf(name, 32, L"Content%d", index);
        RunOnUi([&] { late = MakeWindow(index, {name}, false); });
        Sleep(200);
        for (int i = 0; i < 3; i++) {
            if (!on[i]) continue;
            std::wstring got = t[i].Lookup(late);
            char what[128];
            snprintf(what, sizeof(what), "%c maps the new window %d (got '%ls')", 'A' + i, index, got.c_str());
            Check(got == name, what);
        }
        RunOnUi([&] { CloseTestWindow(late); });
    };
    connect(0);
    report("A only");
    connect(1);
    report("A, then B");
    seesNewWindow(4);
    connect(2);
    report("A, B, then C");
    seesNewWindow(5);
    for (int i = 0; i < 3; i++) CheckMapping(t[i]);
    leave(2);
    report("C left");
    seesNewWindow(6);
    leave(0);
    report("A left");
    seesNewWindow(7);
    leave(1);
    for (int i = 0; i < 3; i++) FreeLibrary(t[i].module);
}

static void ModeCost() {
    const int bulk = 3000;
    AddPermanent(bulk);
    Log("tree: three windows, one with %d extra elements", bulk);
    Log("churn of %d elements, median of 7: baseline %.1f ms", bulk, ChurnMedian(bulk));
    Tap tap;
    tap.Load(L"tapA.dll");
    double r = WaitReady(tap, g_windows[0].hwnd, L"Content0");
    Stats(tap, "first report");
    Log("ready %.1f ms", r);
    Log("churn with the TAP advised: %.1f ms", ChurnMedian(bulk));
    Stats(tap, "after churn");
    tap.shutdown();
    Log("churn after Shutdown (unadvised): %.1f ms", ChurnMedian(bulk));
    FreeLibrary(tap.module);
    ExtraFrees(tap.path);
    Log("churn after the DLL is gone: %.1f ms", ChurnMedian(bulk));
}

static void ModeCycles() {
    const int cycles = 100;
    Usage first{};
    for (int c = 0; c < cycles; c++) {
        Tap tap;
        if (!tap.Load(L"tapA.dll")) {
            Check(false, "load tapA.dll");
            return;
        }
        double ready = WaitReady(tap, g_windows[c % 2].hwnd, c % 2 ? L"Content1" : L"Content0");
        if (ready < 0) {
            Check(false, "TAP ready");
            return;
        }
        CheckMapping(tap);
        double t0 = Now();
        tap.shutdown();
        double down = Now() - t0;
        long objects = Objects(tap);
        FreeLibrary(tap.module);
        int extra = ExtraFrees(tap.path);
        if (c < 2 || c == cycles - 1)
            Log("cycle %d: ready %.1f ms, shutdown %.1f ms, objects left %ld, mapped after FreeLibrary: %s", c, ready,
                down, objects, extra ? "yes" : "no");
        if (objects || extra) g_failures += 0;  // reported above
        if (c == 1) first = GetUsage();
        if (c % 20 == 0) { char l[64]; snprintf(l, 64, "usage after cycle %d", c); LogUsage(l); }
    }
    Usage last = GetUsage();
    Log("growth from cycle 1 to %d: handles %+ld, threads %+ld, private %+.1f MB", cycles - 1,
        (long)last.handles - (long)first.handles, (long)last.threads - (long)first.threads,
        ((double)last.priv - (double)first.priv) / 1048576.0);
}


static void ModeExplore() {
    auto live = [&](std::vector<Tap*> taps, const char* step) {
        std::vector<long long> before;
        for (auto* t : taps) before.push_back(Callbacks(*t));
        Churn(100);
        Sleep(50);
        std::string line = step;
        for (size_t i = 0; i < taps.size(); i++)
            line += " " + std::to_string(Callbacks(*taps[i]) - before[i]);
        Log("%s", line.c_str());
    };
    using PtrFn = void* (*)();
    using HrFn = HRESULT (*)();
    using HitFn = int (*)(RECT, wchar_t*, int);
    Tap a, b;
    a.Load(L"tapA.dll");
    b.Load(L"tapB.dll");
    WaitReady(a, g_windows[0].hwnd, L"Content0");
    WaitReady(b, g_windows[0].hwnd, L"Content0");
    auto sa = ((PtrFn)GetProcAddress(a.module, "TestService"))(), sb = ((PtrFn)GetProcAddress(b.module, "TestService"))();
    Log("E1 service pointers A %p B %p (%s)", sa, sb, sa == sb ? "same" : "different");
    live({&a, &b}, "E2 after A then B, live A/B:");
    HRESULT hr = ((HrFn)GetProcAddress(a.module, "TestReadvise"))();
    Log("E2 A advises again: %08X", (unsigned)hr);
    live({&a, &b}, "E2 after A re-advised, live A/B:");
    // E3: the inactive one (B now) shuts down: does A keep its reports?
    b.shutdown();
    live({&a}, "E3 after the inactive B unadvised, live A:");
    // E4: a TAP that connects but never advises.
    Tap n;
    n.Load(L"tapN.dll");
    n.lookup(nullptr, nullptr, nullptr, 0);  // starts it (null window: returns at once)
    RunOnUi([&] { wchar_t x[4]; n.lookup(g_windows[0].hwnd, nullptr, x, 4); });
    for (int i = 0; i < 200 && n.phase() != 2; i++) Sleep(10);
    Log("E4 no-advise TAP phase %d", n.phase());
    live({&a}, "E4 after a no-advise TAP connected, live A:");
    // E5: HitTest through the no-advise TAP.
    auto hit = (HitFn)GetProcAddress(n.module, "TestHitTest");
    for (int i = 0; i < 3; i++) {
        RECT r;
        GetWindowRect(g_windows[i].islands[0], &r);
        RECT small{r.left + 5, r.top + 5, r.left + 10, r.top + 10};
        wchar_t out[2048];
        int count = 0;
        RunOnUi([&] { count = hit(small, out, 2048); });
        Log("E5 HitTest window %d island rect corner on UI thread: %ls", i, out);
        count = hit(small, out, 2048);
        Log("E5   same from the script thread: %ls", out);
        RECT client{5, 5, 10, 10};
        RunOnUi([&] { count = hit(client, out, 2048); });
        Log("E5   client-coordinate rect (5,5,10,10) on UI: %ls", out);
    }
    a.shutdown();
    n.shutdown();
}


static void ModeExplore2() {
    auto live = [&](Tap& t, const char* step) {
        long long before = Callbacks(t);
        Churn(100);
        Sleep(50);
        Log("%s %lld", step, Callbacks(t) - before);
    };
    Tap a, n;
    a.Load(L"tapA.dll");
    WaitReady(a, g_windows[0].hwnd, L"Content0");
    live(a, "E4 A alone, live A:");
    n.Load(L"tapN.dll");
    RunOnUi([&] { wchar_t x[4]; n.lookup(g_windows[0].hwnd, nullptr, x, 4); });
    for (int i = 0; i < 200 && n.phase() != 2; i++) Sleep(10);
    Log("E4 no-advise TAP phase %d", n.phase());
    live(a, "E4 after a TAP connected without advising, live A:");
    n.shutdown();
    live(a, "E4 after it shut down, live A:");
    a.shutdown();
}


static std::vector<std::wstring> Modules() {
    std::vector<std::wstring> list;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    MODULEENTRY32W me{sizeof(me)};
    for (BOOL ok = Module32FirstW(snap, &me); ok; ok = Module32NextW(snap, &me)) list.push_back(me.szExePath);
    CloseHandle(snap);
    return list;
}

static void ModeExplore3() {
    auto live = [&](Tap& t, const char* step) {
        long long before = Callbacks(t);
        Churn(100);
        Sleep(50);
        Log("%s %lld", step, Callbacks(t) - before);
    };
    using HrFn = HRESULT (*)();
    auto before = Modules();
    Tap a, n;
    a.Load(L"tapA.dll");
    auto loaded = Modules();
    WaitReady(a, g_windows[0].hwnd, L"Content0");
    auto after = Modules();
    for (auto& m : after)
        if (std::find(loaded.begin(), loaded.end(), m) == loaded.end()) Log("module loaded by the first connection: %ls", m.c_str());
    n.Load(L"tapN.dll");
    RunOnUi([&] { wchar_t x[4]; n.lookup(g_windows[0].hwnd, nullptr, x, 4); });
    for (int i = 0; i < 200 && n.phase() != 2; i++) Sleep(10);
    live(a, "E6 A after N connected:");
    // A unadvises and advises again.
    {
        using namespace std;
        auto readvise = (HrFn)GetProcAddress(a.module, "TestReadvise");
        auto unadv = (HrFn)GetProcAddress(a.module, "TestUnadvise");
        HRESULT h1 = unadv ? unadv() : E_NOTIMPL;
        HRESULT h2 = readvise();
        Log("E6 A unadvise %08X, advise again %08X", (unsigned)h1, (unsigned)h2);
    }
    live(a, "E6 A after it advised again:");
    n.shutdown();
    a.shutdown();
    FreeLibrary(a.module);
    FreeLibrary(n.module);
    Log("E7 tapA mapped after our FreeLibrary: %s", a.Mapped() ? "yes" : "no");
    // E7: free XAML's reference too, then make new islands on this and a new thread.
    FreeLibrary(GetModuleHandleW(a.path.c_str()));
    FreeLibrary(GetModuleHandleW(n.path.c_str()));
    Log("E7 after one more FreeLibrary each: A %s, N %s", a.Mapped() ? "mapped" : "gone", n.Mapped() ? "mapped" : "gone");
    HWND late = nullptr;
    RunOnUi([&] { late = MakeWindow(5, {L"Content5"}, false); });
    Churn(500);
    Sleep(300);
    Log("E7 new island and churn after the TAP DLLs are gone: no crash");
    HANDLE th = CreateThread(nullptr, 0, [](void*) -> DWORD {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        auto m = wuxh::WindowsXamlManager::InitializeForCurrentThread();
        HWND h = CreateWindowExW(WS_EX_NOACTIVATE, L"XamlTapTest", L"xaml-tap thread 2", WS_OVERLAPPEDWINDOW,
                                 g_area.left + 700, g_area.top + 500, 300, 200, nullptr, nullptr, nullptr, nullptr);
        wuxh::DesktopWindowXamlSource src;
        src.as<IDesktopWindowXamlSourceNative>()->AttachToWindow(h);
        wuxc::Grid g;
        g.Children().Append(wuxc::TextBlock());
        src.Content(g);
        ShowWindow(h, SW_SHOWNOACTIVATE);
        ULONGLONG end = GetTickCount64() + 800;
        MSG msg;
        while (GetTickCount64() < end) {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) DispatchMessageW(&msg);
            Sleep(10);
        }
        src.Close();
        DestroyWindow(h);
        m.Close();
        return 0;
    }, nullptr, 0, nullptr);
    WaitForSingleObject(th, 10000);
    CloseHandle(th);
    Log("E7 XAML on a new thread after the TAP DLLs are gone: no crash");
    RunOnUi([&] { CloseTestWindow(late); });
}


typedef LONG(NTAPI* NtQueryObject_t)(HANDLE, int, PVOID, ULONG, PULONG);
struct UStr { USHORT Length, MaximumLength; PWSTR Buffer; };
static std::vector<std::wstring> NamedHandles() {
    auto q = (NtQueryObject_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryObject");
    std::vector<std::wstring> out;
    alignas(8) BYTE buf[4096];
    for (ULONG_PTR h = 4; h < 0x8000; h += 4) {
        ULONG len = 0;
        if (q((HANDLE)h, 2, buf, sizeof(buf), &len) < 0) continue;  // type
        std::wstring type(((UStr*)buf)->Buffer, ((UStr*)buf)->Length / 2);
        if (type != L"Section" && type != L"Event" && type != L"Mutant" && type != L"Semaphore" && type != L"ALPC Port")
            continue;
        if (q((HANDLE)h, 1, buf, sizeof(buf), &len) < 0) continue;  // name
        UStr* n = (UStr*)buf;
        if (!n->Length) continue;
        out.push_back(type + L" " + std::wstring(n->Buffer, n->Length / 2));
    }
    return out;
}
static void ModeHandles() {
    auto before = NamedHandles();
    Log("named handles before: %zu", before.size());
    for (auto& h : before) if (h.find(L"iag") != std::wstring::npos || h.find(L"Xaml") != std::wstring::npos) Log("  before: %ls", h.c_str());
    Tap a;
    a.Load(L"tapA.dll");
    WaitReady(a, g_windows[0].hwnd, L"Content0");
    auto after = NamedHandles();
    for (auto& h : after)
        if (std::find(before.begin(), before.end(), h) == before.end()) Log("  new: %ls", h.c_str());
    for (auto& h : before)
        if (std::find(after.begin(), after.end(), h) == after.end()) Log("  gone: %ls", h.c_str());
    a.shutdown();
}


static std::wstring g_teardownPath;
static void ModeTeardown() {
    Tap n;
    n.Load(L"tapN.dll");
    RunOnUi([&] { wchar_t x[4]; n.lookup(g_windows[0].hwnd, nullptr, x, 4); });
    for (int i = 0; i < 200 && n.phase() != 2; i++) Sleep(10);
    n.shutdown();
    FreeLibrary(n.module);
    Log("unbalanced TAP mapped after our FreeLibrary: %s", n.Mapped() ? "yes" : "no");
    g_teardownPath = n.path;
}

static DWORD WINAPI Script(void*) {
    LogUsage("usage before any TAP");
    if (g_mode == L"refs") ModeRefs();
    else if (g_mode == L"concurrent") ModeConcurrent();
    else if (g_mode == L"cost") ModeCost();
    else if (g_mode == L"explore") ModeExplore();
    else if (g_mode == L"explore2") ModeExplore2();
    else if (g_mode == L"explore3") ModeExplore3();
    else if (g_mode == L"handles") ModeHandles();
    else if (g_mode == L"teardown") ModeTeardown();
    else ModeCycles();
    LogUsage("usage at the end");
    Log("failures: %d", g_failures);
    PostThreadMessageW(g_uiThread, WM_QUIT, 0, 0);
    return 0;
}

int wmain(int argc, wchar_t** argv) {
    if (argc > 1) g_mode = argv[1];
    wchar_t exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    g_dir = exe;
    g_dir = g_dir.substr(0, g_dir.find_last_of(L'\\') + 1);
    g_uiThread = GetCurrentThreadId();
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    auto manager = wuxh::WindowsXamlManager::InitializeForCurrentThread();

    // The primary monitor, unless another one sits above it.
    MONITORINFO mi{sizeof(mi)};
    HMONITOR primary = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfoW(primary, &mi);
    g_area = mi.rcWork;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR m, HDC, LPRECT, LPARAM) -> BOOL {
            MONITORINFO i{sizeof(i)};
            GetMonitorInfoW(m, &i);
            Log("monitor (%ld,%ld)-(%ld,%ld)%s", i.rcMonitor.left, i.rcMonitor.top, i.rcMonitor.right,
                i.rcMonitor.bottom, (i.dwFlags & MONITORINFOF_PRIMARY) ? " primary" : "");
            if (i.rcWork.top < g_area.top) g_area = i.rcWork;
            return TRUE;
        },
        0);
    Log("test windows on the work area at (%ld,%ld)", g_area.left, g_area.top);

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"XamlTapTest";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);
    g_main = MakeWindow(0, {L"Content0"}, false);
    MakeWindow(1, {L"Content1"}, false);
    MakeWindow(2, {L"Content2", L"Other2"}, true);

    HANDLE script = CreateThread(nullptr, 0, Script, nullptr, 0, nullptr);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    WaitForSingleObject(script, 5000);
    while (!g_windows.empty()) CloseTestWindow(g_windows.back().hwnd);
    g_bulk = nullptr;
    manager.Close();
    manager = nullptr;
    if (!g_teardownPath.empty()) {
        MSG m;
        for (int i = 0; i < 50; i++) {
            while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) DispatchMessageW(&m);
            Sleep(10);
        }
        Log("after every island and WindowsXamlManager closed: TAP %s", GetModuleHandleW(g_teardownPath.c_str()) ? "still mapped (XAML never frees it)" : "unmapped by XAML");
    }
    return g_failures ? 1 : 0;
}
