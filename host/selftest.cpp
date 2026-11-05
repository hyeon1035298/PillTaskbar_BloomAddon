// SPDX-License-Identifier: GPL-3.0-only
//
// --self-test: the pure logic, no explorer and no network. The exit code is the
// number of failed checks.

#include "addon_protocol.h"
#include "args.h"
#include "commands.h"
#include "json.h"
#include "state.h"
#include "symbols.h"
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

}  // namespace

int RunSelfTest() {
    TestArgs();
    TestCrashRules();
    TestTime();
    TestJson();
    TestHostState();
    TestOffsets();
    TestPtr64();
    TestWindhawkRule();
    Print(Format(L"self-test: %d checks, %d failed", g_checks, g_failures));
    return g_failures;
}

}  // namespace host
