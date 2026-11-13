// SPDX-License-Identifier: GPL-3.0-only
//
// --self-test: the pure logic, no explorer and no network. The exit code is the
// number of failed checks. The symbol tests write only under %TEMP%.

#include "addon_protocol.h"
#include "args.h"
#include "autohide.h"
#include "commands.h"
#include "json.h"
#include "ported.h"
#include "state.h"
#include "symbols.h"
#include "taskbar.h"
#include "util.h"
#include "windhawk.h"

namespace host {

namespace {

int g_failures = 0;
int g_checks = 0;

void Check(bool ok, const wchar_t* what) {
    g_checks++;
    if (!ok) {
        g_failures++;
        Print(std::wstring(L"FAIL ") + what);
    }
}

bool Parses(const std::wstring& line, Args& a) {
    return ParseArgs(SplitCommandLine(line), a);
}

void TestArgs() {
    Args a;
    Check(Parses(L"--serve 1234 --bloom \"C:\\Program Files\\Bloom\\Bloom.exe\" --bloom-local \"D:\\a b\" --log", a) &&
              a.command == Command::Serve && a.bloomPid == 1234 && a.bloomExe == L"C:\\Program Files\\Bloom\\Bloom.exe" &&
              a.bloomLocal == L"D:\\a b" && a.log && !a.resetSafeMode && a.mod == addon::kModDll,
          L"args: --serve with quoted paths");
    Check(Parses(L"\"C:\\x y\\PillTaskbarHost.exe\" --stop", a) && a.command == Command::Stop,
          L"args: program path is skipped");
    Check(Parses(L"--serve 5 --bloom-local x --mod none --reset-safe-mode", a) && a.mod.empty() && a.resetSafeMode,
          L"args: --mod none");
    Check(Parses(L"--serve 5 --bloom-local x --mod Other.dll", a) && a.mod == L"Other.dll", L"args: --mod file");
    Check(!Parses(L"--serve 5 --bloom-local x --mod ..\\evil.dll", a), L"args: --mod rejects paths");
    Check(!Parses(L"--serve --bloom-local x", a), L"args: --serve needs a pid");
    Check(!Parses(L"--serve 0 --bloom-local x", a), L"args: pid 0 is rejected");
    Check(!Parses(L"--serve 12a --bloom-local x", a), L"args: pid must be digits");
    Check(!Parses(L"--serve 5", a), L"args: --serve needs --bloom-local");
    Check(!Parses(L"--stop --serve 5 --bloom-local x", a), L"args: two commands");
    Check(!Parses(L"--frobnicate", a), L"args: unknown flag");
    Check(!Parses(L"", a), L"args: no command");
    Check(!Parses(L"--stop --dry-run", a), L"args: --dry-run only with --serve");
    Check(Parses(L"--serve 5 --bloom-local x --dry-run", a) && a.dryRun, L"args: --dry-run");
    Check(Parses(L"--resolve-symbols --bloom-local x --no-download --no-windhawk-cache --dump y", a) &&
              a.command == Command::ResolveSymbols && a.noDownload && a.noWindhawkCache && a.dumpDir == L"y",
          L"args: --resolve-symbols flags");
    Check(!Parses(L"--resolve-symbols", a), L"args: --resolve-symbols needs --bloom-local");
    Check(Parses(L"--resolve-symbols --bloom-local x --simulate-404 taskbar.dll", a) && a.simulate404 == L"taskbar.dll",
          L"args: --simulate-404 with --resolve-symbols");
    Check(!Parses(L"--serve 5 --bloom-local x --simulate-404 taskbar.dll", a), L"args: --serve never simulates");
    Check(Parses(L"--self-test", a) && a.command == Command::SelfTest, L"args: --self-test");
    Check(DataDirFor(L"C:\\L\\Bloom") == L"C:\\L\\Bloom\\addons\\" ADDON_ID, L"args: data folder");
}

void TestCrashRules() {
    Check(IsCrashExitCode(0xC0000005), L"crash: access violation");
    Check(IsCrashExitCode(0xC0000409), L"crash: stack buffer overrun");
    Check(!IsCrashExitCode(0), L"crash: 0 is clean");
    Check(!IsCrashExitCode(1), L"crash: 1 is clean");
    Check(!IsCrashExitCode(0x40010004), L"crash: debugger termination is not");
    Check(!IsCrashExitCode(0x80000003), L"crash: warning codes are not");

    const uint64_t t0 = 133000000000000000ull, m = kTicksPerMinute;
    Check(CrashCounts(t0, t0 + 4 * m), L"crash: 4 min after injecting counts");
    Check(CrashCounts(t0, t0 + 5 * m), L"crash: 5 min after injecting counts");
    Check(!CrashCounts(t0, t0 + 6 * m), L"crash: 6 min after injecting does not");
    Check(!CrashCounts(0, t0), L"crash: nothing injected");
    Check(!CrashCounts(t0, t0 - m), L"crash: before injecting does not");
    Check(CrashCounts(t0, t0 + 37 * m, true), L"crash: in the add-on's DLL counts long after injecting");
    Check(!CrashCounts(t0, t0 + 37 * m, false), L"crash: in another module only within 5 min");
    Check(!CrashCounts(0, t0, true), L"crash: in the add-on's DLL, nothing injected");
    Check(!CrashCounts(t0, t0 - m, true), L"crash: in the add-on's DLL before injecting does not");

    const std::wstring name = ADDON_NAME;
    std::vector<std::wstring> dlls = {addon::kRuntimeDll, addon::kModDll, L""};
    Check(IsAddonModule(addon::kModDll, dlls) && IsAddonModule(addon::kRuntimeDll, dlls),
          L"crash module: the mod and the runtime");
    Check(IsAddonModule(Lower(addon::kModDll), dlls), L"crash module: any case");
    Check(IsAddonModule(name + L".r123456.dll", dlls), L"crash module: a reload copy of the mod");
    Check(!IsAddonModule(name + L".r.dll", dlls) && !IsAddonModule(name + L".r12x.dll", dlls),
          L"crash module: not a reload copy");
    Check(!IsAddonModule(L"Taskbar.View.dll", dlls) && !IsAddonModule(L"unknown", dlls) && !IsAddonModule(L"", dlls),
          L"crash module: other modules");
    Check(IsAddonModule(L"Other.dll", {addon::kRuntimeDll, L"Other.dll"}), L"crash module: a --mod file");

    HostState s;
    Check(!RecordCrash(s, t0, t0), L"crash: first one is not safe mode");
    Check(RecordCrash(s, t0 + 9 * m, t0 + 9 * m), L"crash: second within 10 min is safe mode");
    HostState s2;
    RecordCrash(s2, t0, t0);
    Check(!RecordCrash(s2, t0 + 11 * m, t0 + 11 * m) && s2.crashes.size() == 1,
          L"crash: 11 min apart drops the old one");
    std::vector<uint64_t> c = {t0 + 5 * m, t0, t0 + 20 * m};
    PruneCrashes(c, t0 + 21 * m);
    Check(c.size() == 1 && c[0] == t0 + 20 * m, L"crash: prune keeps the last 10 min");
}

void TestExplorerReady() {
    Check(!ExplorerReady(false, 240), L"taskbar: 240 ms after explorer started is too early");
    Check(!ExplorerReady(true, kExplorerSettleMs - 1), L"taskbar: island, still settling");
    Check(ExplorerReady(true, kExplorerSettleMs), L"taskbar: island and settled");
    Check(!ExplorerReady(false, kExplorerSettleMs), L"taskbar: no island yet");
    Check(ExplorerReady(false, kExplorerReadyMaxMs), L"taskbar: no island at the limit goes ahead");
    Check(ExplorerReady(false, UINT64_MAX), L"taskbar: unknown age goes ahead");
}

void TestTime() {
    uint64_t t = 0;
    Check(ParseIsoTime(L"2026-01-01T00:00:20Z", t) && IsoTime(t) == L"2026-01-01T00:00:20Z", L"time: round trip");
    Check(ParseIsoTime(L"2026-01-01T00:00:20.123Z", t) && IsoTime(t) == L"2026-01-01T00:00:20Z",
          L"time: milliseconds accepted");
    Check(!ParseIsoTime(L"yesterday", t), L"time: junk rejected");
}

void TestJson() {
    json::Value v;
    Check(json::Parse("{\"a\": [1, -2, 3.5, true, null, \"x\\\"y\\\\z\\u00e9\"], \"b\": {}}", v) &&
              v.Get(L"a").items.size() == 6 && v.Get(L"a").items[1].AsInt() == -2 &&
              v.Get(L"a").items[5].AsString() == L"x\"y\\z\u00e9",
          L"json: parse");
    Check(!json::Parse("{\"a\": }", v), L"json: broken input rejected");
    Check(!json::Parse("{} x", v), L"json: trailing text rejected");
    json::Value o = json::Value::Obj();
    o.Set(L"s", json::Value::Str(L"tab\there \u4e2d"));
    o.Set(L"n", json::Value::Int(133000000000000001ll));
    json::Value back;
    Check(json::Parse(json::Write(o), back) && back.Get(L"s").AsString() == L"tab\there \u4e2d" &&
              back.Get(L"n").AsInt() == 133000000000000001ll,
          L"json: write and read back");
}

void TestHostState() {
    HostState s;
    s.version = L"0.1.0";
    s.pid = 4321;
    s.phase = L"running";
    s.reason = L"";
    s.done = 123456789012ull;
    s.total = 223456789012ull;
    s.safeMode = true;
    s.loaded = LoadedRecord{1234, 133000000000000123ull, 0};
    ParseIsoTime(L"2026-01-01T00:00:00Z", s.loaded->at);
    uint64_t c1;
    ParseIsoTime(L"2026-01-01T00:00:20Z", c1);
    s.crashes = {c1};
    ParseIsoTime(L"2026-01-01T00:00:30Z", s.at);
    HostState r;
    Check(ParseHostState(SerializeHostState(s), r) && r.version == s.version && r.pid == s.pid &&
              r.phase == s.phase && r.done == s.done && r.total == s.total && r.safeMode && r.loaded &&
              r.loaded->pid == 1234 && r.loaded->created == 133000000000000123ull && r.loaded->at == s.loaded->at &&
              r.crashes.size() == 1 && r.crashes[0] == c1 && r.at == s.at,
          L"host-state: round trip");
    s.loaded.reset();
    std::string text = SerializeHostState(s);
    Check(ParseHostState(text, r) && !r.loaded && text.find("\"loaded\": null") != std::string::npos,
          L"host-state: loaded null");
    Check(text.find("symbolsSource") == std::string::npos && text.find("portedFrom") == std::string::npos,
          L"host-state: no symbol source until there is one");
    s.symbolsSource = L"ported";
    s.portedFrom = L"10.0.26100.9818";
    Check(ParseHostState(SerializeHostState(s), r) && r.symbolsSource == L"ported" &&
              r.portedFrom == L"10.0.26100.9818",
          L"host-state: symbolsSource and portedFrom");
    Check(ParseHostState("{ \"v\": 1, \"version\": \"0.0.9\" }", r) && r.version == L"0.0.9" && !r.safeMode &&
              r.crashes.empty(),
          L"host-state: missing fields default");
    Check(!ParseHostState("not json", r), L"host-state: junk rejected");

    // Through the file, the way the host reads and writes it.
    wchar_t tmp[MAX_PATH];
    GetTempPathW(MAX_PATH, tmp);
    std::wstring dir = JoinPath(tmp, Format(L"PillTaskbarHost-selftest-%lu", GetCurrentProcessId()));
    CreateDirs(dir);
    StateFile f;
    f.Init(dir, false);
    f.state().safeMode = true;
    f.state().loaded = LoadedRecord{77, 5, 133000000000000000ull};
    bool saved = f.Save();
    StateFile g;
    g.Init(dir, false);
    g.Load();
    Check(saved && g.state().safeMode && g.state().loaded && g.state().loaded->pid == 77 &&
              g.state().version == addon::kVersion,
          L"host-state: file round trip");
    RuntimeState rs;
    Check(ParseRuntimeState("{ \"v\": 1, \"version\": \"0.1.0\", \"pid\": 1234, \"state\": \"initFailed\", "
                            "\"reason\": \"symbols\", \"at\": \"2026-01-01T00:00:31Z\" }",
                            rs) &&
              rs.pid == 1234 && rs.state == L"initFailed" && rs.reason == L"symbols" && rs.at != 0,
          L"runtime-state: parse");
    Check(ParseRuntimeState("{ \"v\": 1, \"pid\": 1, \"state\": \"running\", \"reason\": \"\", "
                            "\"degraded\": [\"taskbar.view.dll\"], \"at\": \"2026-01-01T00:00:31Z\" }",
                            rs) &&
              rs.degraded.size() == 1 && rs.degraded[0] == L"taskbar.view.dll",
          L"runtime-state: degraded modules");
    Check(ParseRuntimeState("{ \"v\": 1, \"state\": \"running\" }", rs) && rs.degraded.empty(),
          L"runtime-state: no degraded field");
    DeleteFileW(JoinPath(dir, addon::kHostStateFile).c_str());
    RemoveDirectoryW(dir.c_str());
}

void TestOffsets() {
    OffsetsFile f;
    f.module = L"taskbar.dll";
    f.timeDateStamp = 1989904251;
    f.sizeOfImage = 3178496;
    f.entries = {{L"public: void __cdecl TrayUI::_StuckTrayChange(void)", 1094976},
                 {L"void __cdecl consume_general<a,b>(void)", std::nullopt},
                 {L"const TrayUI::`vftable'{for `IInspectable'}", 42}};
    std::string text = FormatOffsets(f);
    Check(text ==
              "1#taskbar.dll#1989904251-3178496#public: void __cdecl TrayUI::_StuckTrayChange(void)#1094976#"
              "void __cdecl consume_general<a,b>(void)##const TrayUI::`vftable'{for `IInspectable'}#42",
          L"offsets: format");
    OffsetsFile r;
    Check(ParseOffsets(text + "\r\n", r) && r.module == f.module && r.timeDateStamp == f.timeDateStamp &&
              r.sizeOfImage == f.sizeOfImage && r.entries.size() == 3 && r.entries[0].second == 1094976u &&
              !r.entries[1].second && r.entries[2].first == f.entries[2].first,
          L"offsets: parse");
    CodeView cv;
    cv.timeDateStamp = f.timeDateStamp;
    cv.sizeOfImage = f.sizeOfImage;
    std::vector<std::wstring> names = {f.entries[0].first, f.entries[1].first};
    Check(OffsetsMatch(r, L"taskbar.dll", cv, names), L"offsets: matches its build");
    Check(!OffsetsMatch(r, L"Taskbar.View.dll", cv, names), L"offsets: other module");
    names.push_back(L"a name added later");
    Check(!OffsetsMatch(r, L"taskbar.dll", cv, names), L"offsets: a new name forces a rebuild");
    cv.sizeOfImage = 100;
    Check(!OffsetsMatch(r, L"taskbar.dll", cv, {}), L"offsets: other build");
    Check(!ParseOffsets("2#taskbar.dll#1-2", r), L"offsets: unknown version");
    Check(!ParseOffsets("1#taskbar.dll#1-2#odd", r), L"offsets: odd field count");
    Check(!ParseOffsets("1#taskbar.dll#1-2#name#12x", r), L"offsets: bad RVA");

    cv.guid = {0xDAB69A79, 0xFEE0, 0x33F0, {0xD8, 0x1E, 0x02, 0x3F, 0xBF, 0x17, 0x2B, 0x51}};
    cv.age = 1;
    Check(PdbId(cv) == L"DAB69A79FEE033F0D81E023FBF172B511", L"offsets: PDB id");
    Check(OffsetsFileName(L"Taskbar.View.dll", cv) == L"taskbar.view.dll_DAB69A79FEE033F0D81E023FBF172B511.txt",
          L"offsets: file name");
}

void TestPtr64() {
    Check(FixPtr64(L"public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ * __ptr64,struct tagSIZE * "
                   L"__ptr64) __ptr64") ==
              L"public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *)",
          L"ptr64: plain pointers and method");
    Check(FixPtr64(L"public: struct TaskbarFrameMetrics __cdecl TaskbarFrame::GetMetrics(void)const __ptr64") ==
              L"public: struct TaskbarFrameMetrics __cdecl TaskbarFrame::GetMetrics(void)const ",
          L"ptr64: const method keeps its space");
    Check(FixPtr64(L"void __cdecl f(void)volatile __ptr64") == L"void __cdecl f(void)volatile ",
          L"ptr64: volatile method keeps its space");
    Check(FixPtr64(L"void __cdecl f(char const * __ptr64 const __ptr64)") == L"void __cdecl f(char const * const )",
          L"ptr64: const pointer");
    Check(FixPtr64(L"const TrayUI::`vftable'{for `IInspectable'}") == L"const TrayUI::`vftable'{for `IInspectable'}",
          L"ptr64: no change without __ptr64");
}

void TestIslandWanted() {
    json::Value schema, values;
    json::Parse("{\"groups\": [{\"items\": [{\"key\": \"autohide.enabled\", \"default\": true}]},"
                " {\"items\": [{\"key\": \"island.enabled\", \"default\": true}]}]}",
                schema);
    Check(IslandWanted(values, schema), L"island: no values.json takes the schema's defaults");
    json::Parse("{\"pill.glass\": false}", values);
    Check(IslandWanted(values, schema), L"island: keys left out are at their default");
    json::Parse("{\"island.enabled\": false, \"autohide.enabled\": false}", values);
    Check(!IslandWanted(values, schema), L"island: off in values.json");
    json::Parse("{\"autohide.enabled\": 0}", values);
    Check(!IslandWanted(values, schema), L"island: needs the auto-hide part too");
    Check(!IslandWanted(json::Value{}, json::Value{}), L"island: no default known is off");
}

void TestWindhawkRule() {
    WindhawkEngine none, running{true, false}, boot{false, true};
    Check(!WindhawkRule(false, false, running).any(), L"windhawk: not installed");
    Check(!WindhawkRule(false, true, running).any(), L"windhawk: mod disabled");
    Check(WindhawkRule(true, true, running).any(), L"windhawk: on while windhawk.exe runs");
    Check(WindhawkRule(true, true, boot).any(), L"windhawk: on with the service starting at boot");
    Check(!WindhawkRule(true, true, none).any(), L"windhawk: on, but nothing can load it");
    WindhawkConflict c = WindhawkRule(false, false, none);
    CheckWindhawkModules({L"C:\\x\\" + std::wstring(addon::kWindhawkMods[0]) + L"_1.0_12.dll"}, c);
    Check(c.any(), L"windhawk: the mod DLL is already in explorer");
}

// A scratch folder for one test, removed again by the caller.
std::wstring TestDir(const wchar_t* name) {
    wchar_t tmp[MAX_PATH];
    GetTempPathW(MAX_PATH, tmp);
    std::wstring dir = JoinPath(tmp, Format(L"PillTaskbarHost-selftest-%lu-%ls", GetCurrentProcessId(), name));
    CreateDirs(dir);
    return dir;
}

void RemoveTree(const std::wstring& dir) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(JoinPath(dir, L"*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            std::wstring n = fd.cFileName;
            if (n == L"." || n == L"..") continue;
            std::wstring p = JoinPath(dir, n);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                RemoveTree(p);
            else
                DeleteFileW(p.c_str());
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    RemoveDirectoryW(dir.c_str());
}

bool WriteText(const std::wstring& path, const std::string& text) {
    CreateDirs(DirName(path));
    return WriteFileAtomic(path, text);
}

void TestDownloadMapping() {
    Check(DownloadForStatus(200) == Download::Ok, L"download: 200 is a file");
    Check(DownloadForStatus(404) == Download::NotFound, L"download: 404 is not published");
    Check(DownloadForStatus(403) == Download::Failed && DownloadForStatus(500) == Download::Failed &&
              DownloadForStatus(0) == Download::Failed,
          L"download: other answers fail");
    Check(WorstExit(addon::kExitOk, addon::kExitDownload) == addon::kExitDownload &&
              WorstExit(addon::kExitDownload, addon::kExitOk) == addon::kExitDownload,
          L"symbols: a download failure beats ok");
    Check(WorstExit(addon::kExitDownload, addon::kExitNoSymbols) == addon::kExitNoSymbols &&
              WorstExit(addon::kExitNoSymbols, addon::kExitError) == addon::kExitError &&
              WorstExit(addon::kExitError, addon::kExitDownload) == addon::kExitError,
          L"symbols: worst exit order");
    Check(BinaryKey(1635664427, 3252224) == L"617E422B31A000" && BinaryKey(0x0ABC, 0x1000) == L"00000ABC1000",
          L"symbols: msdl binary key");
    DWORD stamp = 0, size = 0;
    Check(ParseBinaryKey(L"E8E4B3D1308000", stamp, size) && stamp == 0xE8E4B3D1 && size == 0x308000 &&
              !ParseBinaryKey(L"E8E4B3D1", stamp, size) && !ParseBinaryKey(L"E8E4B3D13080x0", stamp, size),
          L"symbols: msdl binary key parsed back");
    // taskbar.dll 9549: SizeOfImage 0x308000, the file 3216656 bytes.
    Check(DllDownloadCap(0x308000) > 3216656 && DllDownloadCap(0x308000) < 6 * 1024 * 1024 &&
              kMaxPdbBytes >= 10 * 50ull * 1024 * 1024 && kDownloadDeadlineMs >= 10 * 60 * 1000,
          L"symbols: download caps leave room for the files measured");
    FileVersion a, b, c;
    Check(ParseFileVersion(L"10.0.26100.9818", a) && ParseFileVersion(L"10.0.26100.9974", b) &&
              ParseFileVersion(L"10.0.26300.7000", c) && a.Text() == L"10.0.26100.9818",
          L"version: parse");
    Check(a.SameBranch(b) && !a.SameBranch(c) && a.Value() < b.Value(), L"version: branch and order");
    Check(!ParseFileVersion(L"10.0.26100", a) && !ParseFileVersion(L"10.0.26100.9818x", a) &&
              !ParseFileVersion(L"10.0.70000.1", a),
          L"version: junk rejected");
    wchar_t sys[MAX_PATH];
    GetSystemDirectoryW(sys, MAX_PATH);
    Check(ReadFileVersion(JoinPath(sys, L"kernel32.dll"), a) && a.part[0] == 10, L"version: read from a DLL");
}

void TestSymbolsMeta() {
    std::vector<std::wstring> names = {L"a", L"b", L"c d=e", L"not listed"};
    std::vector<bool> r = ParseSymbolsMeta("a\treq\r\nb\topt\nc d=e\topt\nc d=e\treq\n", names);
    Check(r.size() == 4 && r[0] && !r[1] && r[2] && r[3], L"meta: req, opt, required anywhere wins, unlisted");
    r = ParseSymbolsMeta("", names);
    Check(r.size() == 4 && r[0] && r[1] && r[2] && r[3], L"meta: no file means every name is required");
}

void TestPortSources() {
    PortSources s;
    Check(ParsePortSources("# head\n"
                           "taskbar.dll 10.0.26100.9818 617e422b31a000 "
                           "A52F99F7BE8CC1EE360025D1C06C1480A489B1D309BEF230C05EB21A106CCF34\n"
                           "Taskbar.dll 10.0.26100.9711 769B877B308000  # no hash\n"
                           "taskbar.dll 10.0.26100.9444 8DBF8CD1304000\n"
                           "twinui.pcshell.dll 10.0.26100.9818 8E751D2B9D2000\n"
                           "deny taskbar.dll 769B877B308000\n",
                           s) &&
              s.lines.size() == 4 && s.deny.size() == 1 && s.lines[0].key == L"617E422B31A000" &&
              s.lines[0].sha256.size() == 64 && s.lines[0].sha256[0] == L'a' && s.lines[1].module == L"taskbar.dll" &&
              s.lines[1].sha256.empty(),
          L"port-sources: parse");
    PortSources bad;
    Check(!ParsePortSources("taskbar.dll 10.0.26100 617E422B31A000\n", bad) &&
              !ParsePortSources("taskbar.dll 10.0.26100.1 XYZ\n", bad) &&
              !ParsePortSources("deny taskbar.dll\n", bad),
          L"port-sources: broken lines rejected");

    FileVersion v9974, v9711, v26300;
    ParseFileVersion(L"10.0.26100.9974", v9974);
    ParseFileVersion(L"10.0.26100.9711", v9711);
    ParseFileVersion(L"10.0.26300.100", v26300);
    PortSources open = s;
    open.deny.clear();
    auto all = MergeCandidates({}, open, L"taskbar.dll");
    auto pick = FilterCandidates(all, open, L"taskbar.dll", v9974, 2);
    Check(all.size() == 3 && pick.size() == 2 && pick[0].version.part[3] == 9818 && pick[1].version.part[3] == 9711,
          L"port-sources: newest two older builds of the branch");
    pick = FilterCandidates(all, s, L"taskbar.dll", v9974, 2);
    Check(pick.size() == 2 && pick[0].version.part[3] == 9818 && pick[1].version.part[3] == 9444,
          L"port-sources: a denied build is skipped");
    pick = FilterCandidates(all, open, L"taskbar.dll", v9711, 2);
    Check(pick.size() == 1 && pick[0].version.part[3] == 9444, L"port-sources: only builds older than the new one");
    Check(FilterCandidates(all, open, L"taskbar.dll", v26300, 2).empty(), L"port-sources: another branch is none");
    Check(MergeCandidates({}, open, L"twinui.pcshell.dll").size() == 1, L"port-sources: per module");

    Candidate local1, local2;
    local1.key = L"769B877B308000";  // shipped: takes the line's version
    local1.local = true;
    local2.key = L"11111111300000";  // unknown until its DLL is read
    local2.local = true;
    all = MergeCandidates({local1, local2}, open, L"taskbar.dll");
    Check(all.size() == 4 && all[0].local && all[0].version.part[3] == 9711 && !all[1].version.valid &&
              !all[2].local,
          L"port-sources: local files first, shipped versions filled in");
    pick = FilterCandidates(all, open, L"taskbar.dll", v9974, 5);
    Check(pick.size() == 3 && pick[1].local, L"port-sources: an unknown version never passes the filter");
    Check(CandidateSet(pick) == L"617E422B31A000 769B877B308000 8DBF8CD1304000", L"port-sources: candidate set");

    std::wstring dir = TestDir(L"local");
    CodeView cur;
    cur.timeDateStamp = 0x617E422B;
    cur.sizeOfImage = 0x31A000;
    WriteText(JoinPath(dir, L"taskbar.dll_AAAA1.txt"), "1#taskbar.dll#1989904251-3178496#n#5");
    WriteText(JoinPath(dir, L"taskbar.dll_BBBB1.txt"), "1#taskbar.dll#1635664427-3252224#n#5");  // current build
    WriteText(JoinPath(dir, L"taskbar.dll_CCCC1.txt"), "1#taskbar.dll#1-2#n#5");
    WriteText(JoinPath(dir, L"taskbar.dll_CCCC1.ported"), "engine=1\n");  // ported: no source
    WriteText(JoinPath(dir, L"taskbar.view.dll_DDDD1.txt"), "1#taskbar.view.dll#3-4#n#5");
    auto loc = LocalCandidates(dir, L"taskbar.dll", cur);
    Check(loc.size() == 1 && loc[0].key == L"769B877B308000" && loc[0].local &&
              FileName(loc[0].localFile) == L"taskbar.dll_AAAA1.txt",
          L"port-sources: local offsets files, not this build, not ported ones");
    RemoveTree(dir);
}

void TestSidecar() {
    Sidecar s;
    s.engine = 1;
    s.hostVersion = L"0.2.15";
    s.sourceVersion = L"10.0.26100.9818";
    s.sourceKey = L"617E422B31A000";
    s.sourceKind = L"shipped";
    s.sources = L"617E422B31A000 769B877B308000 -8DBF8CD1304000";
    ParseIsoTime(L"2026-11-13T10:00:00Z", s.date);
    s.lastProbe = s.date;
    s.names = {{L"public: bool __cdecl X::operator==(int)const ", port::Outcome::Ported, 0x1a2b3c, L"anchor+vtslot"},
               {L"absent one", port::Outcome::Absent, 0, L""},
               {L"two", port::Outcome::Ambiguous, 0, L"twin\twith tab"},
               {L"three", port::Outcome::NotFound, 0, L""}};
    Sidecar r;
    Check(ParseSidecar(FormatSidecar(s), r) && r.engine == 1 && r.hostVersion == s.hostVersion &&
              r.sourceVersion == s.sourceVersion && r.sourceKey == s.sourceKey && r.sourceKind == L"shipped" &&
              r.sources == s.sources &&
              r.date == s.date && r.lastProbe == s.lastProbe && r.names.size() == 4 &&
              r.names[0].name == s.names[0].name && r.names[0].rva == 0x1a2b3c &&
              r.names[0].outcome == port::Outcome::Ported && r.names[1].outcome == port::Outcome::Absent &&
              r.names[2].outcome == port::Outcome::Ambiguous && r.names[2].evidence == L"twin with tab" &&
              r.names[3].outcome == port::Outcome::NotFound,
          L"sidecar: round trip");
    Check(!ParseSidecar("host=1\n", r), L"sidecar: needs its engine");
    const uint64_t h = 60 * kTicksPerMinute;
    Check(!ProbeDue(s, s.lastProbe + 5 * h) && ProbeDue(s, s.lastProbe + 6 * h) && ProbeDue(s, s.lastProbe - h),
          L"sidecar: probe every 6 h");
    s.lastProbe = 0;
    Check(ProbeDue(s, s.date), L"sidecar: never probed is due");
    Check(ParseSidecar(FormatSidecar(s), r) && r.lastProbe == 0 && ProbeDue(r, s.date),
          L"sidecar: lastProbe 0 (a simulated 404) reads back as due");
}

void TestWriteRule() {
    CodeView cv;
    cv.timeDateStamp = 7;
    cv.sizeOfImage = 0x100000;
    cv.sizeOfHeaders = 0x1000;
    std::vector<port::Want> wanted = {{L"A", true}, {L"B", false}, {L"C", true}, {L"D", true}};
    std::vector<port::OldSymbol> old = {{L"A", 0x2100}, {L"B", 0x2200}, {L"D", 0x2300}, {L"Z", 0x2400}};
    auto results = [](port::Outcome b, uint32_t d, port::Outcome dOut = port::Outcome::Ported) {
        return std::vector<port::Result>{{L"A", port::Outcome::Ported, 0x3100, L"x"},
                                         {L"B", b, b == port::Outcome::Ported ? 0x3200u : 0u, L""},
                                         {L"C", port::Outcome::Absent, 0, L""},
                                         {L"D", dOut, d, L""}};
    };
    OffsetsFile f;
    std::vector<PortedName> names;
    std::vector<std::wstring> missing;
    bool ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, old, results(port::Outcome::NotFound, 0x3300), f, names,
                                 missing);
    Check(ok && f.module == L"taskbar.dll" && f.timeDateStamp == 7 && f.entries.size() == 4 &&
              f.entries[0].second == 0x3100u && !f.entries[1].second && !f.entries[2].second &&
              f.entries[3].second == 0x3300u,
          L"write rule: required ported, optional and absent empty");
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, old,
                            results(port::Outcome::Ported, 0, port::Outcome::NotFound), f, names, missing);
    Check(!ok && missing.size() == 1 && missing[0] == L"D", L"write rule: a required name in the old PDB must port");
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, old, results(port::Outcome::Ported, 0x3100), f, names, missing);
    Check(!ok && missing.size() == 2 && names[0].outcome == port::Outcome::Ambiguous,
          L"write rule: two names on one new address that were apart lose it");
    std::vector<port::OldSymbol> folded = {{L"A", 0x2100}, {L"B", 0x2200}, {L"D", 0x2100}};
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, folded, results(port::Outcome::Ported, 0x3100), f, names,
                            missing);
    Check(ok && f.entries[3].second == 0x3100u, L"write rule: folded code may share its address");
    // ResolveAll gives a name once per OLD address, the host's first: only that one counts.
    std::vector<port::OldSymbol> twice = {{L"A", 0x2100}, {L"B", 0x2200}, {L"A", 0x2500}, {L"D", 0x2100}};
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, twice, results(port::Outcome::Ported, 0x3100), f, names,
                            missing);
    Check(ok && f.entries[0].second == 0x3100u && f.entries[3].second == 0x3100u,
          L"write rule: a name at several old addresses is judged by the host's (the first)");
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, old, results(port::Outcome::Ported, 0x800), f, names, missing);
    Check(!ok && missing.size() == 1 && missing[0] == L"D", L"write rule: an address in the headers is no port");
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, old, results(port::Outcome::Ported, 0x100000), f, names,
                            missing);
    Check(!ok, L"write rule: an address past the image is no port");
    auto shortList = results(port::Outcome::Ported, 0x3300);
    shortList.pop_back();
    Check(!BuildPortedOffsets(L"taskbar.dll", cv, wanted, old, shortList, f, names, missing),
          L"write rule: one result per wanted name");
    std::vector<port::OldSymbol> none = {{L"Z", 0x2400}};
    std::vector<port::Result> absent = {{L"A", port::Outcome::Absent, 0, L""},
                                        {L"B", port::Outcome::Absent, 0, L""},
                                        {L"C", port::Outcome::Absent, 0, L""},
                                        {L"D", port::Outcome::Absent, 0, L""}};
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, none, absent, f, names, missing);
    Check(ok && f.entries.size() == 4 && !f.entries[0].second, L"write rule: no required name in this module");
    std::vector<port::Result> liar = {{L"A", port::Outcome::Ported, 0x3100, L""},
                                      {L"B", port::Outcome::Absent, 0, L""},
                                      {L"C", port::Outcome::Ported, 0x3500, L""},
                                      {L"D", port::Outcome::Ported, 0x3300, L""}};
    ok = BuildPortedOffsets(L"taskbar.dll", cv, wanted, old, liar, f, names, missing);
    Check(ok && !f.entries[2].second, L"write rule: a name the old PDB lacks is never written");

    std::wstring dir = TestDir(L"write");
    std::vector<std::wstring> order;
    std::wstring off = JoinPath(dir, L"m_K.txt"), side = JoinPath(dir, L"m_K.ported");
    Check(WritePortedFiles(off, "offsets", side, "sidecar", &order) && order.size() == 2 && order[0] == side &&
              order[1] == off && FileExists(off) && FileExists(side),
          L"write order: sidecar, then offsets");
    order.clear();
    std::wstring off2 = JoinPath(JoinPath(dir, L"missing-folder"), L"m_L.txt"), side2 = JoinPath(dir, L"m_L.ported");
    Check(!WritePortedFiles(off2, "offsets", side2, "sidecar", &order) && order.size() == 1 && !FileExists(side2) &&
              !FileExists(off2),
          L"write order: a failed offsets write takes its sidecar back");
    RemoveTree(dir);

    // port-tmp: one folder per process; another running process's folder stays.
    std::wstring data = TestDir(L"porttmp");
    std::wstring root = JoinPath(data, kPortTmpDir);
    Check(PortTmpDir(data) == JoinPath(root, std::to_wstring(GetCurrentProcessId())), L"port-tmp: per process");
    std::wstring live = JoinPath(root, std::to_wstring(GetCurrentProcessId()));
    std::wstring dead = JoinPath(root, L"4294967292");  // a pid no process has
    std::wstring before = JoinPath(root, L"taskbar.dll_617E422B31A000");  // the layout before
    WriteText(JoinPath(live, L"a.dll"), "x");
    WriteText(JoinPath(dead, L"m_K\\a.dll.part"), "x");
    WriteText(JoinPath(before, L"taskbar.dll"), "x");
    CleanStalePortTmp(data);
    auto gone = [](const std::wstring& p) { return GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES; };
    Check(FileExists(JoinPath(live, L"a.dll")) && gone(dead) && gone(before),
          L"port-tmp: a dead process's folder and old leftovers go, a running one's stays");
    RemoveTree(data);

    OffsetsFile ported, real;
    ported.entries = {{L"a", 1}, {L"b", 2}, {L"c", std::nullopt}};
    real.entries = {{L"a", 1}, {L"b", 3}, {L"c", 9}};
    size_t differ = 0;
    std::wstring diff = PortDiff(ported, real, differ);
    Check(differ == 2 && diff.rfind(L"1 same, 2 differ: b (2, real 3), c (not ported)", 0) == 0,
          L"port diff: counts and names");
}

void TestPortFailAndGuard() {
    PortFail f{1, L"0.2.15", L"617E422B31A000 769B877B308000", 0, L"required names not ported: X"};
    ParseIsoTime(L"2026-11-13T10:00:00Z", f.at);
    PortFail r;
    Check(ParsePortFail(FormatPortFail(f), r) && r.engine == 1 && r.hostVersion == f.hostVersion &&
              r.sources == f.sources && r.at == f.at && r.why == f.why,
          L"portfail: round trip");
    const uint64_t h = 60 * kTicksPerMinute;
    Check(PortFailBlocks(r, 1, L"0.2.15", f.sources, f.at + 23 * h), L"portfail: blocks a retry within a day");
    Check(!PortFailBlocks(r, 1, L"0.2.15", f.sources, f.at + 25 * h), L"portfail: tried again after a day");
    Check(!PortFailBlocks(r, 2, L"0.2.15", f.sources, f.at + h) && !PortFailBlocks(r, 1, L"0.2.16", f.sources, f.at + h) &&
              !PortFailBlocks(r, 1, L"0.2.15", L"617E422B31A000", f.at + h),
          L"portfail: a new engine, host or source set tries again");
    PortFail probe = f;
    probe.retry = kPortProbeFailRetry;
    Check(FormatPortFail(f).find("retry=") == std::string::npos && ParsePortFail(FormatPortFail(probe), r) &&
              r.retry == kPortProbeFailRetry && PortFailBlocks(r, 1, L"0.2.15", f.sources, f.at + h) &&
              !PortFailBlocks(r, 1, L"0.2.15", f.sources, f.at + 3 * h),
          L"portfail: a failed version probe holds for two hours, not a day");
    Check(ParsePortFail(FormatPortFail(f) + "retry=0\nretry=99999\n", r) && r.retry == kPortFailRetry,
          L"portfail: a retry of none or over a day holds a day");

    std::wstring data = TestDir(L"guard");
    std::wstring off = JoinPath(data, addon::kOffsetsDir);
    WriteText(JoinPath(off, L"taskbar.dll_K1.txt"), "ported");
    WriteText(JoinPath(off, L"taskbar.dll_K1.ported"), "engine=1\nsource=10.0.26100.9818 617E422B31A000\n");
    WriteText(JoinPath(off, L"taskbar.view.dll_K2.txt"), "real");
    const uint64_t t0 = 133000000000000000ull;
    PortGuard g;
    Check(!ReadGuard(data, g), L"guard: not armed at first");
    Check(SetGuard(data, {L"taskbar.view.dll_K2", L"taskbar.dll_K1"}, t0) && ReadGuard(data, g) && g.keys.size() == 2 &&
              g.keys[0] == L"taskbar.dll_K1" && g.armedAt == t0,
          L"guard: armed");
    Check(SetGuard(data, {L"taskbar.dll_K1", L"taskbar.view.dll_K2"}, t0 + h) && ReadGuard(data, g) &&
              g.armedAt == t0,
          L"guard: re-arming the same keys keeps the time");
    auto q = QuarantineGuarded(data, L"explorer crashed", t0 + 2 * h);
    Check(q.size() == 1 && q[0] == L"taskbar.dll_K1" && !FileExists(JoinPath(off, L"taskbar.dll_K1.txt")) &&
              !FileExists(JoinPath(off, L"taskbar.dll_K1.ported")) && IsQuarantined(off, L"taskbar.dll_K1") &&
              FileExists(JoinPath(off, L"taskbar.view.dll_K2.txt")) && !IsQuarantined(off, L"taskbar.view.dll_K2") &&
              !ReadGuard(data, g),
          L"guard: a crash quarantines the ported keys only, then disarms");
    std::string text;
    Check(ReadFileBytes(JoinPath(off, L"taskbar.dll_K1.quarantined"), text) &&
              text.find("reason=explorer crashed") != std::string::npos &&
              text.find("source=10.0.26100.9818 617E422B31A000") != std::string::npos,
          L"guard: the quarantine says why and from what");
    Check(QuarantineGuarded(data, L"again", t0).empty(), L"guard: nothing armed, nothing quarantined");
    // An offsets file that cannot be deleted keeps its sidecar and the guard.
    WriteText(JoinPath(off, L"taskbar.dll_K3.txt"), "ported");
    WriteText(JoinPath(off, L"taskbar.dll_K3.ported"), "engine=1\n");
    SetGuard(data, {L"taskbar.dll_K3"}, t0);
    {
        Handle lock(CreateFileW(JoinPath(off, L"taskbar.dll_K3.txt").c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                OPEN_EXISTING, 0, nullptr));
        q = QuarantineGuarded(data, L"explorer crashed", t0);
        Check(lock && q.size() == 1 && IsQuarantined(off, L"taskbar.dll_K3") &&
                  FileExists(JoinPath(off, L"taskbar.dll_K3.txt")) &&
                  FileExists(JoinPath(off, L"taskbar.dll_K3.ported")) && ReadGuard(data, g) && g.keys.size() == 1 &&
                  g.keys[0] == L"taskbar.dll_K3",
              L"guard: offsets that cannot be deleted keep their sidecar and stay guarded");
    }
    q = QuarantineGuarded(data, L"explorer crashed", t0);
    Check(q.size() == 1 && !FileExists(JoinPath(off, L"taskbar.dll_K3.txt")) &&
              !FileExists(JoinPath(off, L"taskbar.dll_K3.ported")) && !ReadGuard(data, g),
          L"guard: the next look deletes them and disarms");
    SetGuard(data, {L"x"}, t0);
    Check(SetGuard(data, {}, t0) && !ReadGuard(data, g), L"guard: an empty list disarms");
    RemoveTree(data);
}

// A real module that lost a required name since an older build is the mod's problem,
// not the ported offsets' (serve: noSymbols, the ported offsets stay).
void TestLostRequired() {
    std::wstring data = TestDir(L"lost");
    std::wstring exe = JoinPath(data, L"exe");
    std::wstring off = JoinPath(data, addon::kOffsetsDir);
    WriteText(JoinPath(exe, kSymbolsMeta), "Gone\treq\nOther\treq\nMaybe\topt\n");
    auto file = [](DWORD stamp, std::vector<std::pair<std::wstring, std::optional<DWORD>>> e) {
        OffsetsFile f;
        f.module = L"taskbar.dll";
        f.timeDateStamp = stamp;
        f.sizeOfImage = 0x100000;
        f.entries = std::move(e);
        return FormatOffsets(f);
    };
    // Every module's file lists every name; Other is another module's (empty in both).
    WriteText(JoinPath(off, L"taskbar.dll_NEW1.txt"), file(2, {{L"Gone", {}}, {L"Other", {}}, {L"Maybe", {}}}));
    Check(LostRequiredName(data, exe, L"taskbar.dll_NEW1").empty(),
          L"lost: no older build, no proof that a name was the module's");
    WriteText(JoinPath(off, L"taskbar.dll_OLD1.txt"), file(1, {{L"Gone", 0x1000}, {L"Other", {}}, {L"Maybe", 0x2000}}));
    Check(LostRequiredName(data, exe, L"taskbar.dll_NEW1") == L"Gone",
          L"lost: a required name the older build had and this one lacks");
    WriteText(JoinPath(off, L"taskbar.dll_OLD1.ported"), "engine=2\n");
    Check(LostRequiredName(data, exe, L"taskbar.dll_NEW1").empty(), L"lost: an older build's ported offsets prove nothing");
    DeleteFileW(JoinPath(off, L"taskbar.dll_OLD1.ported").c_str());
    WriteText(JoinPath(exe, kSymbolsMeta), "Gone\topt\nOther\treq\nMaybe\topt\n");
    Check(LostRequiredName(data, exe, L"taskbar.dll_NEW1").empty(), L"lost: an optional name lost is no reason");
    WriteText(JoinPath(exe, kSymbolsMeta), "Gone\treq\nOther\treq\nMaybe\topt\n");
    WriteText(JoinPath(off, L"taskbar.dll_NEW1.ported"), "engine=2\n");
    Check(LostRequiredName(data, exe, L"taskbar.dll_NEW1").empty(), L"lost: ported offsets are never the real module");
    RemoveTree(data);
}

// Porting work runs on a worker while the calling thread keeps serving.
void TestWorker() {
    auto done = std::make_shared<std::atomic<bool>>(false);
    DWORD self = GetCurrentThreadId();
    auto other = std::make_shared<std::atomic<bool>>(false);
    int idles = 0;
    bool ok = RunOnWorker(
        [done, other, self](const std::atomic<bool>&) {
            Sleep(350);
            *other = GetCurrentThreadId() != self;
            *done = true;
        },
        [&] {
            idles++;
            return true;
        });
    Check(ok && *done && *other && idles >= 2, L"worker: runs off this thread while idle is called");

    auto sawCancel = std::make_shared<std::atomic<bool>>(false);
    ULONGLONG t0 = GetTickCount64();
    ok = RunOnWorker(
        [sawCancel](const std::atomic<bool>& cancel) {
            for (int i = 0; i < 300 && !cancel; i++) Sleep(10);
            *sawCancel = cancel.load();
        },
        [] { return false; });
    Check(!ok && *sawCancel && GetTickCount64() - t0 < 2500 && !PortWorkerLeftRunning(),
          L"worker: a stop sets the cancel flag and waits for the worker to end");
    ok = RunOnWorker([done](const std::atomic<bool>&) { *done = false; }, nullptr);
    Check(ok && !*done, L"worker: without idle the caller just waits");
}

// port::Run checks the NEW build before it reads code, and a set cancel flag stops it.
// System32's kernel32.dll stands in for both builds.
void TestPortIdentity() {
    wchar_t sys[MAX_PATH];
    GetSystemDirectoryW(sys, MAX_PATH);
    std::wstring dll = JoinPath(sys, L"kernel32.dll");
    CodeView cv;
    if (!ReadCodeView(dll, cv)) {
        Check(false, L"port identity: kernel32.dll has a CodeView record");
        return;
    }
    std::atomic<bool> stop{true};
    port::Input in;
    in.oldDll = in.newDll = dll;
    in.cancel = &stop;
    std::vector<port::Result> out;
    port::Stats stats;
    std::wstring error;
    in.expectNew = ExpectedNew(cv);
    bool ok = port::Run(in, out, stats, error);
    Check(!ok && error == L"stopped", L"port identity: the build it was told of passes, then the cancel flag stops it");
    in.expectNew.pdbGuid[0] ^= 1;
    ok = port::Run(in, out, stats, error);
    Check(!ok && error.find(L"another build") != std::wstring::npos, L"port identity: another RSDS GUID fails");
    in.expectNew = ExpectedNew(cv);
    in.expectNew.pdbAge++;
    ok = port::Run(in, out, stats, error);
    Check(!ok && error.find(L"another build") != std::wstring::npos, L"port identity: another age fails");
    in.expectNew = ExpectedNew(cv);
    in.expectNew.timeDateStamp ^= 1;
    ok = port::Run(in, out, stats, error);
    Check(!ok && error.find(L"another build") != std::wstring::npos, L"port identity: another TimeDateStamp fails");
}

// PrepareSymbols end to end without the network: modules that are missing, one whose
// PDB is not cached with downloads off, and one with --simulate-404.
void TestPrepare() {
    std::wstring data = TestDir(L"prepare");
    std::wstring exe = JoinPath(data, L"exe");
    WriteText(JoinPath(exe, addon::kSymbolList), "name one\nname two\n");
    wchar_t sys[MAX_PATH];
    GetSystemDirectoryW(sys, MAX_PATH);
    SymbolModule k32{L"kernel32.dll", JoinPath(sys, L"kernel32.dll")};
    CodeView cv;
    if (!ReadCodeView(k32.path, cv)) {
        Check(false, L"prepare: kernel32.dll has an RSDS record");
        RemoveTree(data);
        return;
    }
    SymbolOptions o;
    o.dataDir = data;
    o.symbolList = JoinPath(exe, addon::kSymbolList);
    o.noDownload = true;
    o.noWindhawkCache = true;
    o.modules = {{L"gone.dll", JoinPath(data, L"gone.dll")}, k32};
    SymbolReport rep;
    int r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitNoSymbols && rep.modules.size() == 2 && rep.modules[0].exit == addon::kExitNoSymbols &&
              rep.modules[1].exit == addon::kExitDownload && rep.reason.empty(),
          L"prepare: every module is tried, the worst result wins");

    // Downloads off: a listed source build is never cached, so porting asks the network
    // for nothing and concludes nothing (no .portfail).
    o.modules = {k32};
    o.simulate404 = L"kernel32.dll";
    std::wstring key = ModuleKey(L"kernel32.dll", cv);
    std::wstring off = JoinPath(data, addon::kOffsetsDir);
    FileVersion kv;
    if (ReadFileVersion(k32.path, kv) && kv.part[3] > 0) {
        kv.part[3]--;
        WriteText(JoinPath(exe, kPortSources), Narrow(L"kernel32.dll " + kv.Text() + L" 00000001200000\n"));
        r = PrepareSymbols(o, &rep);
        Check(r == addon::kExitDownload && rep.reason.empty() && !FileExists(JoinPath(off, key + kPortFailExt)) &&
                  !FileExists(JoinPath(off, key + L".txt")),
              L"prepare: --no-download downloads nothing, so it ports nothing");

        // A local build whose version probe fails is skipped; the listed one is still
        // tried, and the skipped one is never remembered as a failure.
        std::wstring local = JoinPath(off, L"kernel32.dll_LOCAL1.txt");
        WriteText(local, "1#kernel32.dll#1-4096#name one#");  // msdl key 000000011000
        WriteText(JoinPath(exe, kPortSources), Narrow(L"kernel32.dll " + kv.Text() + L" 00000002200000\n"));
        PortRequest req;
        req.dataDir = data;
        req.exeDir = exe;
        req.module = k32;
        req.cv = cv;
        req.names = {L"name one", L"name two"};
        req.noDownload = true;
        req.noWindhawkCache = true;
        PortReport pr;
        PortStatus st = PortModule(req, pr);
        Check(st == PortStatus::Failed && pr.detail.find(L"00000002200000") != std::wstring::npos &&
                  !FileExists(JoinPath(off, key + kPortFailExt)),
              L"port: a failed version probe skips that build, the listed one is still tried");
        DeleteFileW(JoinPath(exe, kPortSources).c_str());
        st = PortModule(req, pr);
        Check(st == PortStatus::Failed && pr.detail.find(L"000000011000") != std::wstring::npos &&
                  !FileExists(JoinPath(off, key + kPortFailExt)),
              L"port: a build skipped for a failed probe is not remembered");
        DeleteFileW(local.c_str());
    } else {
        Check(false, L"prepare: kernel32.dll has a file version to list an older build by");
    }

    o.noDownload = false;  // nothing is downloaded: no source build is listed
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitDownload && rep.reason == L"pdbMissing" && FileExists(JoinPath(off, key + kPortFailExt)) &&
              !FileExists(JoinPath(off, key + L".txt")),
          L"prepare: a 404 no port covers is pdbMissing, remembered in .portfail");
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitDownload && rep.reason == L"pdbMissing", L"prepare: the remembered failure stays");
    WriteText(JoinPath(off, key + kQuarantinedExt), "reason=test\n");
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitDownload && rep.reason == L"portCrashed", L"prepare: a quarantined build is portCrashed");

    // Ported offsets in place: used until the PDB is asked for again.
    OffsetsFile f;
    f.module = L"kernel32.dll";
    f.timeDateStamp = cv.timeDateStamp;
    f.sizeOfImage = cv.sizeOfImage;
    f.entries = {{L"name one", cv.sizeOfHeaders}, {L"name two", std::nullopt}};
    Sidecar sc;
    sc.engine = port::kEngineVersion;
    sc.sourceVersion = L"10.0.1.2";
    sc.sourceKey = L"00000001200000";
    sc.date = sc.lastProbe = NowTicks();
    DeleteFileW(JoinPath(off, key + kQuarantinedExt).c_str());
    WritePortedFiles(JoinPath(off, key + L".txt"), FormatOffsets(f), JoinPath(off, key + kPortedExt),
                     FormatSidecar(sc));
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitOk && rep.modules.size() == 1 && rep.modules[0].ported && rep.PortedKeys().size() == 1 &&
              rep.PortedKeys()[0] == key && rep.PortedFrom() == L"10.0.1.2",
          L"prepare: ported offsets are used and reported");
    sc.lastProbe = NowTicks() - 7 * 60 * kTicksPerMinute;
    WriteText(JoinPath(off, key + kPortedExt), FormatSidecar(sc));
    r = PrepareSymbols(o, &rep);
    std::string text;
    Sidecar after;
    Check(r == addon::kExitOk && rep.modules[0].ported && ReadFileBytes(JoinPath(off, key + kPortedExt), text) &&
              ParseSidecar(text, after) && IsoTime(after.lastProbe) == IsoTime(sc.lastProbe),
          L"prepare: a due probe under --simulate-404 keeps the port and is not noted as a probe");

    // Ported offsets the host no longer trusts are dropped and the module ported again
    // (here: no source, so pdbMissing).
    auto stale = [&](const std::string& sidecar, const wchar_t* what) {
        WritePortedFiles(JoinPath(off, key + L".txt"), FormatOffsets(f), JoinPath(off, key + kPortedExt), sidecar);
        int res = PrepareSymbols(o, &rep);
        Check(res == addon::kExitDownload && rep.reason == L"pdbMissing" && !rep.modules[0].ported &&
                  !FileExists(JoinPath(off, key + L".txt")) && !FileExists(JoinPath(off, key + kPortedExt)),
              what);
    };
    Sidecar other = sc;
    other.lastProbe = NowTicks();
    other.engine = port::kEngineVersion + 1;
    stale(FormatSidecar(other), L"prepare: another engine's ported offsets are ported again");
    stale("not a sidecar\n", L"prepare: ported offsets with a broken sidecar are ported again");
    other.engine = port::kEngineVersion;
    WriteText(JoinPath(exe, kPortSources), "deny kernel32.dll 00000001200000\n");
    stale(FormatSidecar(other), L"prepare: ported offsets from a now denied build are ported again");
    DeleteFileW(JoinPath(exe, kPortSources).c_str());
    WritePortedFiles(JoinPath(off, key + L".txt"), FormatOffsets(f), JoinPath(off, key + kPortedExt),
                     FormatSidecar(other));
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitOk && rep.modules[0].ported && FileExists(JoinPath(off, key + L".txt")),
          L"prepare: this engine's ported offsets from an allowed build stay");
    DropPorted(off, key);

    // The runtime would not start on ported offsets: they go and a .portfail keeps the
    // build from being ported again, so the next start is 14 pdbMissing.
    DeleteFileW(JoinPath(off, key + kPortFailExt).c_str());
    WritePortedFiles(JoinPath(off, key + L".txt"), FormatOffsets(f), JoinPath(off, key + kPortedExt),
                     FormatSidecar(other));  // sources "": no candidates, as the porter would say here
    SetGuard(data, {key}, NowTicks());
    auto gone = RetractPorted(data, {key, L"real.dll_K"}, L"symbols missing (test)", NowTicks());
    PortGuard guard;
    PortFail pf;
    text.clear();
    Check(gone.size() == 1 && gone[0] == key && !FileExists(JoinPath(off, key + L".txt")) &&
              !FileExists(JoinPath(off, key + kPortedExt)) && !ReadGuard(data, guard) &&
              ReadFileBytes(JoinPath(off, key + kPortFailExt), text) && ParsePortFail(text, pf) &&
              pf.why == L"symbols missing (test)",
          L"retract: the ported offsets go, a .portfail stays, the guard is off");
    r = PrepareSymbols(o, &rep);
    text.clear();
    Check(r == addon::kExitDownload && rep.reason == L"pdbMissing" &&
              ReadFileBytes(JoinPath(off, key + kPortFailExt), text) && ParsePortFail(text, pf) &&
              pf.why == L"symbols missing (test)",
          L"retract: the next start waits for the PDB without porting again");
    DeleteFileW(JoinPath(off, key + kPortFailExt).c_str());

    // Ported offsets a quarantine could not delete (the sidecar went) are never used.
    WriteText(JoinPath(off, key + L".txt"), FormatOffsets(f));
    WriteText(JoinPath(off, key + kQuarantinedExt), "reason=test\n");
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitDownload && rep.reason == L"portCrashed" && !FileExists(JoinPath(off, key + L".txt")),
          L"prepare: offsets beside a quarantine are dropped");

    // A quarantine holds only for the engine that wrote it.
    WriteText(JoinPath(off, key + kQuarantinedExt), Narrow(Format(L"reason=test\nengine=%d\n", port::kEngineVersion)));
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitDownload && rep.reason == L"portCrashed", L"prepare: this engine's quarantine holds");
    WriteText(JoinPath(off, key + kQuarantinedExt),
              Narrow(Format(L"reason=test\nengine=%d\n", port::kEngineVersion + 1)));
    r = PrepareSymbols(o, &rep);
    Check(r == addon::kExitDownload && rep.reason == L"pdbMissing" && !IsQuarantined(off, key),
          L"prepare: another engine's quarantine does not block porting");
    RemoveTree(data);
}

}  // namespace

int RunSelfTest() {
    TestArgs();
    TestCrashRules();
    TestExplorerReady();
    TestTime();
    TestJson();
    TestHostState();
    TestOffsets();
    TestPtr64();
    TestWindhawkRule();
    TestIslandWanted();
    TestDownloadMapping();
    TestSymbolsMeta();
    TestPortSources();
    TestSidecar();
    TestWriteRule();
    TestPortFailAndGuard();
    TestLostRequired();
    TestWorker();
    TestPortIdentity();
    TestPrepare();
    Print(Format(L"self-test: %d checks, %d failed", g_checks, g_failures));
    return g_failures;
}

}  // namespace host
