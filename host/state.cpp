// SPDX-License-Identifier: GPL-3.0-only

#include "state.h"

#include <algorithm>

#include "addon_protocol.h"
#include "json.h"
#include "util.h"

namespace host {

using json::Value;

const wchar_t* ExitReason(int code) {
    switch (code) {
        case addon::kExitCrashes: return L"crashes";
        case addon::kExitNoSymbols: return L"noSymbols";
        case addon::kExitWindhawk: return L"windhawk";
        case addon::kExitBlocked: return L"blocked";
        case addon::kExitDownload: return L"download";
        case addon::kExitRestart: return L"restart";
        default: return L"error";
    }
}

std::string SerializeHostState(const HostState& s) {
    Value v = Value::Obj();
    v.Set(L"v", Value::Int(1));
    v.Set(L"version", Value::Str(s.version));
    v.Set(L"pid", Value::Int(s.pid));
    v.Set(L"phase", Value::Str(s.phase));
    v.Set(L"reason", Value::Str(s.reason));
    Value progress = Value::Obj();
    progress.Set(L"done", Value::Int((int64_t)s.done));
    progress.Set(L"total", Value::Int((int64_t)s.total));
    v.Set(L"progress", progress);
    v.Set(L"safeMode", Value::Bool(s.safeMode));
    if (s.loaded) {
        Value l = Value::Obj();
        l.Set(L"pid", Value::Int(s.loaded->pid));
        l.Set(L"created", Value::Int((int64_t)s.loaded->created));
        l.Set(L"at", Value::Str(IsoTime(s.loaded->at)));
        v.Set(L"loaded", l);
    } else {
        v.Set(L"loaded", Value::Null());
    }
    Value crashes = Value::Arr();
    for (uint64_t c : s.crashes) crashes.Push(Value::Str(IsoTime(c)));
    v.Set(L"crashes", crashes);
    if (!s.symbolsSource.empty()) v.Set(L"symbolsSource", Value::Str(s.symbolsSource));
    if (!s.portedFrom.empty()) v.Set(L"portedFrom", Value::Str(s.portedFrom));
    v.Set(L"at", Value::Str(IsoTime(s.at)));
    return json::Write(v);
}

bool ParseHostState(const std::string& utf8, HostState& out) {
    Value v;
    if (!json::Parse(utf8, v) || v.type != Value::Type::Object) return false;
    HostState s;
    s.version = v.Get(L"version").AsString();
    s.pid = (DWORD)v.Get(L"pid").AsInt();
    s.phase = v.Get(L"phase").AsString(L"stopped");
    s.reason = v.Get(L"reason").AsString();
    s.done = (uint64_t)v.Get(L"progress").Get(L"done").AsInt();
    s.total = (uint64_t)v.Get(L"progress").Get(L"total").AsInt();
    s.safeMode = v.Get(L"safeMode").AsBool();
    const Value& l = v.Get(L"loaded");
    if (l.type == Value::Type::Object) {
        LoadedRecord r;
        r.pid = (DWORD)l.Get(L"pid").AsInt();
        r.created = (uint64_t)l.Get(L"created").AsInt();
        ParseIsoTime(l.Get(L"at").AsString(), r.at);
        if (r.pid) s.loaded = r;
    }
    for (const Value& c : v.Get(L"crashes").items) {
        uint64_t t;
        if (ParseIsoTime(c.AsString(), t)) s.crashes.push_back(t);
    }
    s.symbolsSource = v.Get(L"symbolsSource").AsString();
    s.portedFrom = v.Get(L"portedFrom").AsString();
    ParseIsoTime(v.Get(L"at").AsString(), s.at);
    out = std::move(s);
    return true;
}

bool ParseRuntimeState(const std::string& utf8, RuntimeState& out) {
    Value v;
    if (!json::Parse(utf8, v) || v.type != Value::Type::Object) return false;
    out.version = v.Get(L"version").AsString();
    out.pid = (DWORD)v.Get(L"pid").AsInt();
    out.state = v.Get(L"state").AsString();
    out.reason = v.Get(L"reason").AsString();
    out.degraded.clear();
    for (const Value& d : v.Get(L"degraded").items)
        if (!d.AsString().empty()) out.degraded.push_back(d.AsString());
    out.at = 0;
    ParseIsoTime(v.Get(L"at").AsString(), out.at);
    return true;
}

void StateFile::Init(const std::wstring& dataDir, bool readOnly) {
    path_ = JoinPath(dataDir, addon::kHostStateFile);
    runtimePath_ = JoinPath(dataDir, addon::kRuntimeStateFile);
    panelRuntimePath_ = JoinPath(dataDir, addon::kPanelRuntimeStateFile);
    readOnly_ = readOnly;
}

void StateFile::Load() {
    std::string bytes;
    HostState s;
    if (ReadFileBytes(path_, bytes) && ParseHostState(bytes, s)) state_ = std::move(s);
}

bool StateFile::Save() {
    state_.version = addon::kVersion;
    state_.pid = GetCurrentProcessId();
    state_.at = NowTicks();
    PruneCrashes(state_.crashes, state_.at);
    if (readOnly_) return true;
    bool ok = WriteFileAtomic(path_, SerializeHostState(state_));
    if (!ok) Log(L"could not write %ls (%lu)", addon::kHostStateFile, GetLastError());
    return ok;
}

void StateFile::SetPhase(const std::wstring& phase, const std::wstring& reason) {
    state_.phase = phase;
    state_.reason = reason;
    if (phase != L"symbols") state_.done = state_.total = 0;
    Save();
}

bool StateFile::ReadRuntime(RuntimeState& out, bool panel) const {
    std::string bytes;
    return ReadFileBytes(panel ? panelRuntimePath_ : runtimePath_, bytes) && ParseRuntimeState(bytes, out);
}

bool IsCrashExitCode(DWORD code) {
    return (code & 0xF0000000u) == 0xC0000000u;
}

bool CrashCounts(uint64_t injectedAt, uint64_t diedAt, bool inAddon) {
    return injectedAt != 0 && diedAt >= injectedAt && (inAddon || diedAt - injectedAt <= kCrashAfterInjectWindow);
}

bool IsAddonModule(const std::wstring& module, const std::vector<std::wstring>& dlls) {
    const std::wstring ext = L".dll";
    for (const auto& dll : dlls) {
        if (dll.empty()) continue;
        if (EqualsNoCase(module, dll)) return true;
        // The runtime loads a copy "<stem>.r<tick>.dll" while an old image is still mapped.
        if (dll.size() <= ext.size() || !EqualsNoCase(dll.substr(dll.size() - ext.size()), ext)) continue;
        std::wstring prefix = dll.substr(0, dll.size() - ext.size()) + L".r";
        if (module.size() <= prefix.size() + ext.size() || !StartsWithNoCase(module, prefix) ||
            !EqualsNoCase(module.substr(module.size() - ext.size()), ext))
            continue;
        std::wstring tick = module.substr(prefix.size(), module.size() - prefix.size() - ext.size());
        if (std::all_of(tick.begin(), tick.end(), [](wchar_t c) { return c >= L'0' && c <= L'9'; })) return true;
    }
    return false;
}

void PruneCrashes(std::vector<uint64_t>& crashes, uint64_t now) {
    std::erase_if(crashes, [&](uint64_t t) { return t + kCrashCountWindow < now; });
    std::sort(crashes.begin(), crashes.end());
}

bool RecordCrash(HostState& s, uint64_t diedAt, uint64_t now) {
    s.crashes.push_back(diedAt);
    PruneCrashes(s.crashes, now);
    return s.crashes.size() >= kCrashesForSafeMode;
}

}  // namespace host
