// SPDX-License-Identifier: GPL-3.0-only

#include "eventlog.h"

#include <winevt.h>

#include <cwchar>

#include "util.h"

namespace host {

namespace {

struct Evt {
    EVT_HANDLE h = nullptr;
    explicit Evt(EVT_HANDLE v = nullptr) : h(v) {}
    ~Evt() {
        if (h) EvtClose(h);
    }
    Evt(const Evt&) = delete;
    Evt& operator=(const Evt&) = delete;
    explicit operator bool() const { return h != nullptr; }
};

// Renders the values of an event for a render context into buf.
bool RenderValues(EVT_HANDLE context, EVT_HANDLE event, std::vector<BYTE>& buf, DWORD& count) {
    DWORD used = 0;
    count = 0;
    if (EvtRender(context, event, EvtRenderEventValues, (DWORD)buf.size(), buf.data(), &used, &count)) return true;
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return false;
    buf.resize(used);
    return EvtRender(context, event, EvtRenderEventValues, (DWORD)buf.size(), buf.data(), &used, &count);
}

// Application Error stores numbers as text such as "0x1a2b"; accept both forms. Its
// exception code and offset are hex without "0x" ("c0000005"): base 16 for those.
bool VariantNumber(const EVT_VARIANT& v, uint64_t& out, int base = 0) {
    switch (v.Type & EVT_VARIANT_TYPE_MASK) {
        case EvtVarTypeString:
            if (!v.StringVal) return false;
            out = wcstoull(v.StringVal, nullptr, base);
            return true;
        case EvtVarTypeUInt32:
        case EvtVarTypeHexInt32: out = v.UInt32Val; return true;
        case EvtVarTypeUInt64:
        case EvtVarTypeHexInt64: out = v.UInt64Val; return true;
        case EvtVarTypeFileTime: out = v.FileTimeVal; return true;
        default: return false;
    }
}

std::wstring RenderXml(EVT_HANDLE event) {
    DWORD used = 0, count = 0;
    EvtRender(nullptr, event, EvtRenderEventXml, 0, nullptr, &used, &count);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || used == 0) return {};
    std::wstring xml(used / sizeof(wchar_t) + 1, L'\0');
    if (!EvtRender(nullptr, event, EvtRenderEventXml, (DWORD)(xml.size() * sizeof(wchar_t)), xml.data(), &used,
                   &count))
        return {};
    xml.resize(wcslen(xml.c_str()));
    return xml;
}

// Runs a query and calls fn for each event until it returns true.
template <typename Fn>
bool ForEachEvent(const wchar_t* channel, const std::wstring& query, Fn fn) {
    Evt results(EvtQuery(nullptr, channel, query.c_str(), EvtQueryChannelPath | EvtQueryReverseDirection));
    if (!results) return false;
    EVT_HANDLE batch[16];
    DWORD got = 0;
    while (EvtNext(results.h, ARRAYSIZE(batch), batch, 2000, 0, &got)) {
        bool done = false;
        for (DWORD i = 0; i < got; i++) {
            if (!done) done = fn(batch[i]);
            EvtClose(batch[i]);
        }
        if (done) return true;
    }
    return false;
}

}  // namespace

bool FindAppCrashEvent(DWORD pid, uint64_t created, uint64_t from, uint64_t to, AppCrash& out) {
    std::wstring query = L"*[System[Provider[@Name='Application Error'] and EventID=1000 and TimeCreated[@SystemTime>='" +
                         IsoTime(from, true) + L"' and @SystemTime<='" + IsoTime(to, true) + L"']]]";
    // Event data: 3 = faulting module name, 6 = exception code, 7 = fault offset,
    // 8 = process id, 9 = process creation time.
    Evt user(EvtCreateRenderContext(0, nullptr, EvtRenderContextUser));
    Evt system(EvtCreateRenderContext(0, nullptr, EvtRenderContextSystem));
    if (!user || !system) return false;
    std::vector<BYTE> buf(4096), sysBuf(4096);
    return ForEachEvent(L"Application", query, [&](EVT_HANDLE ev) {
        DWORD count = 0;
        if (!RenderValues(user.h, ev, buf, count) || count < 10) return false;
        auto* v = (EVT_VARIANT*)buf.data();
        uint64_t evPid = 0, evCreated = 0;
        if (!VariantNumber(v[8], evPid) || evPid != pid) return false;
        if (created && VariantNumber(v[9], evCreated) && evCreated && evCreated != created) return false;
        out = AppCrash{};
        if ((v[3].Type & EVT_VARIANT_TYPE_MASK) == EvtVarTypeString && v[3].StringVal)
            out.module = FileName(v[3].StringVal);
        uint64_t code = 0;
        if (VariantNumber(v[6], code, 16)) out.code = (uint32_t)code;
        VariantNumber(v[7], out.offset, 16);
        out.time = to;
        if (RenderValues(system.h, ev, sysBuf, count) && count > EvtSystemTimeCreated) {
            auto* s = (EVT_VARIANT*)sysBuf.data();
            if ((s[EvtSystemTimeCreated].Type & EVT_VARIANT_TYPE_MASK) == EvtVarTypeFileTime)
                out.time = s[EvtSystemTimeCreated].FileTimeVal;
        }
        return true;
    });
}

bool FindCodeIntegrityBlock(const std::vector<std::wstring>& fileNames, std::wstring& which) {
    const wchar_t* query =
        L"*[System[(EventID=3033 or EventID=3076 or EventID=3077) and TimeCreated[timediff(@SystemTime) <= 60000]]]";
    return ForEachEvent(L"Microsoft-Windows-CodeIntegrity/Operational", query, [&](EVT_HANDLE ev) {
        std::wstring xml = Lower(RenderXml(ev));
        for (auto& f : fileNames) {
            if (!f.empty() && xml.find(Lower(f)) != std::wstring::npos) {
                which = f;
                return true;
            }
        }
        return false;
    });
}

}  // namespace host
