// SPDX-License-Identifier: GPL-3.0-only
//
// Offline harness for the symbol porter (host/port.h), dev only.
//   port-check <oldDll> <oldPdb> <newDll> <symbols.txt> [--meta symbols-meta.txt] [--all] [--json out.json]
//     ports the names in symbols.txt (or, with --all, every public of the OLD PDB) from
//     the OLD build to the NEW one and prints one line per name.
//   port-check --dump <dll> <pdb> <out.json>
//     every public of a PDB as port::ResolveAll names it ({name: [rva, ...]}, the host's
//     address first), for comparing with other tools.
// symbols-meta.txt lines are "name<TAB>module<TAB>req|opt" (or "name<TAB>req|opt"); a
// line for another module is ignored.

#include <windows.h>

#include <cstdio>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "pe.h"
#include "port.h"
#include "symbols.h"
#include "util.h"

namespace {

void Out(const std::string& s) { fwrite(s.data(), 1, s.size(), stdout); }

std::string Json(const std::wstring& w) {
    std::string u = host::Narrow(w), o = "\"";
    for (unsigned char c : u) {
        if (c == '"' || c == '\\') o += '\\', o += (char)c;
        else if (c < 0x20) {
            char b[8];
            snprintf(b, sizeof b, "\\u%04x", c);
            o += b;
        } else o += (char)c;
    }
    return o + "\"";
}

const wchar_t* OutcomeName(port::Outcome o) {
    switch (o) {
        case port::Outcome::Ported: return L"ported";
        case port::Outcome::Absent: return L"absent";
        case port::Outcome::Ambiguous: return L"ambiguous";
        default: return L"notfound";
    }
}

std::wstring Kind(const port::PeImage& pe, const std::wstring& name, uint32_t rva) {
    if (pe.IsExec(rva)) return L"func";
    if (name.find(L"`vftable'") != std::wstring::npos) return L"vftable";
    const port::PeSection* s = pe.SectionOf(rva);
    if (s && (s->name == ".data" || s->name == ".rdata") && name.find(L"`vbtable'") == std::wstring::npos)
        return L"global";
    return L"other";
}

int Dump(const std::wstring& dll, const std::wstring& pdb, const std::wstring& outPath) {
    std::vector<port::OldSymbol> syms;
    std::wstring err;
    auto t0 = GetTickCount64();
    if (!port::ResolveAll(pdb, dll, syms, err)) {
        Out("error: " + host::Narrow(err) + "\n");
        return 1;
    }
    std::vector<std::wstring> order;
    std::unordered_map<std::wstring, std::string> rvas;
    for (auto& s : syms) {
        auto [it, fresh] = rvas.emplace(s.name, std::to_string(s.rva));
        if (fresh) order.push_back(s.name);
        else it->second += ", " + std::to_string(s.rva);
    }
    std::string j = "{\"names\": {";
    for (size_t i = 0; i < order.size(); i++) j += (i ? ", " : "") + Json(order[i]) + ": [" + rvas[order[i]] + "]";
    j += "}}\n";
    if (!host::WriteFileAtomic(outPath, j)) {
        Out("error: cannot write the output\n");
        return 1;
    }
    Out(std::to_string(syms.size()) + " names in " + std::to_string(GetTickCount64() - t0) + " ms\n");
    return 0;
}

}  // namespace

int wmain(int argc, wchar_t** argv) {
    std::vector<std::wstring> pos;
    std::wstring meta, json;
    bool all = false;
    if (argc == 5 && std::wstring(argv[1]) == L"--dump") return Dump(argv[2], argv[3], argv[4]);
    for (int i = 1; i < argc; i++) {
        std::wstring a = argv[i];
        if (a == L"--meta" && i + 1 < argc) meta = argv[++i];
        else if (a == L"--json" && i + 1 < argc) json = argv[++i];
        else if (a == L"--all") all = true;
        else pos.push_back(a);
    }
    if (pos.size() != 4) {
        Out("usage: port-check <oldDll> <oldPdb> <newDll> <symbols.txt> [--meta symbols-meta.txt] [--all] [--json out.json]\n"
            "       port-check --dump <dll> <pdb> <out.json>\n");
        return 2;
    }
    port::Input in;
    in.oldDll = pos[0];
    in.newDll = pos[2];
    std::wstring err;
    auto t0 = GetTickCount64();
    if (!port::ResolveAll(pos[1], pos[0], in.oldSymbols, err)) {
        Out("error: " + host::Narrow(err) + "\n");
        return 1;
    }
    uint64_t pdbMs = GetTickCount64() - t0;
    std::vector<std::wstring> names;
    if (all) {
        // A name with several OLD addresses comes once per address from ResolveAll.
        std::unordered_set<std::wstring> seen;
        for (auto& s : in.oldSymbols)
            if (seen.insert(s.name).second) names.push_back(s.name);
    } else if (!host::ReadSymbolList(pos[3], names)) {
        Out("error: cannot read the symbol list\n");
        return 1;
    }
    std::unordered_map<std::wstring, bool> required;
    if (!meta.empty()) {
        std::string bytes;
        if (!host::ReadFileBytes(meta, bytes)) {
            Out("error: cannot read the meta file\n");
            return 1;
        }
        std::wstring text = host::Widen(bytes), module = host::Lower(host::FileName(in.newDll));
        size_t start = 0;
        while (start < text.size()) {
            size_t e = text.find(L'\n', start);
            std::wstring line = text.substr(start, e == std::wstring::npos ? std::wstring::npos : e - start);
            start = e == std::wstring::npos ? text.size() : e + 1;
            if (!line.empty() && line.back() == L'\r') line.pop_back();
            std::vector<std::wstring> cols;
            for (size_t p = 0;;) {
                size_t t = line.find(L'\t', p);
                cols.push_back(line.substr(p, t == std::wstring::npos ? std::wstring::npos : t - p));
                if (t == std::wstring::npos) break;
                p = t + 1;
            }
            if (cols.size() == 3 && host::Lower(cols[1]) != module) continue;
            if (cols.size() >= 2) required[cols[0]] = cols.back() == L"req";
        }
    }
    for (auto& n : names) in.wanted.push_back({n, required.count(n) && required[n]});

    std::vector<port::Result> res;
    port::Stats st;
    if (!port::Run(in, res, st, err)) {
        Out("error: " + host::Narrow(err) + "\n");
        return 1;
    }
    port::PeImage oldPe, newPe;
    oldPe.Load(in.oldDll, err);
    newPe.Load(in.newDll, err);
    std::unordered_map<std::wstring, uint32_t> oldRva;
    for (auto& s : in.oldSymbols) oldRva.emplace(s.name, s.rva);

    size_t counts[4] = {};
    std::string j = "{\"engine\": " + std::to_string(port::kEngineVersion) + ", \"old\": " + Json(in.oldDll) +
                    ", \"new\": " + Json(in.newDll) + ", \"functions_old\": " + std::to_string(st.functionsOld) +
                    ", \"functions_new\": " + std::to_string(st.functionsNew) + ", \"ms\": " + std::to_string(st.ms) +
                    ", \"analyze_ms\": " + std::to_string(st.analyzeMs) + ", \"engine_ms\": " + std::to_string(st.engineMs) +
                    ", \"pdb_ms\": " + std::to_string(pdbMs) + ", \"results\": [\n";
    for (size_t i = 0; i < res.size(); i++) {
        auto& r = res[i];
        counts[(int)r.outcome]++;
        auto o = oldRva.find(r.name);
        std::wstring kind = o == oldRva.end() ? L"absent" : Kind(oldPe, r.name, o->second);
        j += std::string(i ? ",\n" : "") + "{\"name\": " + Json(r.name) + ", \"outcome\": \"" +
             host::Narrow(OutcomeName(r.outcome)) + "\", \"rva\": " + std::to_string(r.rva) +
             ", \"old_rva\": " + (o == oldRva.end() ? std::string("null") : std::to_string(o->second)) +
             ", \"kind\": \"" + host::Narrow(kind) + "\", \"required\": " + (in.wanted[i].required ? "true" : "false") +
             ", \"evidence\": " + Json(r.evidence) + "}";
        if (!all)
            Out(host::Narrow(host::Format(L"%-9ls %8lx  %ls  [%ls]\n", OutcomeName(r.outcome), r.rva, r.name.c_str(),
                                          r.evidence.c_str())));
    }
    j += "\n]}\n";
    Out(host::Narrow(host::Format(L"%zu names: %zu ported, %zu absent, %zu ambiguous, %zu not found; "
                                  L"%lu/%lu functions, %llu ms (analysis %llu, engine %llu), PDB %llu ms\n",
                                  res.size(), counts[0], counts[1], counts[2], counts[3], st.functionsOld,
                                  st.functionsNew, (unsigned long long)st.ms, (unsigned long long)st.analyzeMs,
                                  (unsigned long long)st.engineMs, (unsigned long long)pdbMs)));
    if (!json.empty() && !host::WriteFileAtomic(json, j)) {
        Out("error: cannot write the json\n");
        return 1;
    }
    (void)newPe;
    return 0;
}
