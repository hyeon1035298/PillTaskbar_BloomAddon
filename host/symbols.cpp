// SPDX-License-Identifier: GPL-3.0-only

#include "symbols.h"

#include <dbghelp.h>
#include <winhttp.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <unordered_map>
#include <unordered_set>

#include "addon_protocol.h"
#include "util.h"

#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif

namespace host {

// --- RSDS ---

namespace {

// A read-only view of a file, so bounds can be checked against its real size.
struct FileView {
    Handle file, mapping;
    const BYTE* base = nullptr;
    uint64_t size = 0;
    ~FileView() {
        if (base) UnmapViewOfFile(base);
    }
    bool Open(const std::wstring& path) {
        file.Reset(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                               OPEN_EXISTING, 0, nullptr));
        LARGE_INTEGER li;
        if (!file || !GetFileSizeEx(file.get(), &li) || li.QuadPart == 0) return false;
        size = (uint64_t)li.QuadPart;
        mapping.Reset(CreateFileMappingW(file.get(), nullptr, PAGE_READONLY, 0, 0, nullptr));
        if (!mapping) return false;
        base = (const BYTE*)MapViewOfFile(mapping.get(), FILE_MAP_READ, 0, 0, 0);
        return base != nullptr;
    }
    template <typename T>
    const T* At(uint64_t off, uint64_t count = 1) const {
        if (off > size || sizeof(T) * count > size - off) return nullptr;
        return (const T*)(base + off);
    }
};

}  // namespace

bool ReadCodeView(const std::wstring& path, CodeView& out) {
    FileView f;
    if (!f.Open(path)) return false;
    auto* dos = f.At<IMAGE_DOS_HEADER>(0);
    if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
    auto* nt = f.At<IMAGE_NT_HEADERS64>((uint64_t)dos->e_lfanew);
    if (!nt || nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        return false;
    out.timeDateStamp = nt->FileHeader.TimeDateStamp;
    out.sizeOfImage = nt->OptionalHeader.SizeOfImage;
    uint64_t secOff = (uint64_t)dos->e_lfanew + offsetof(IMAGE_NT_HEADERS64, OptionalHeader) +
                      nt->FileHeader.SizeOfOptionalHeader;
    auto* sec = f.At<IMAGE_SECTION_HEADER>(secOff, nt->FileHeader.NumberOfSections);
    if (!sec) return false;
    auto rvaToOff = [&](DWORD rva) -> uint64_t {
        for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
            DWORD span = std::max(sec[i].Misc.VirtualSize, sec[i].SizeOfRawData);
            if (rva >= sec[i].VirtualAddress && rva < sec[i].VirtualAddress + span)
                return (uint64_t)rva - sec[i].VirtualAddress + sec[i].PointerToRawData;
        }
        return 0;
    };
    if (nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_DEBUG) return false;
    auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    uint64_t off = rvaToOff(dir.VirtualAddress);
    size_t count = dir.Size / sizeof(IMAGE_DEBUG_DIRECTORY);
    auto* dbg = off ? f.At<IMAGE_DEBUG_DIRECTORY>(off, count) : nullptr;
    if (!dbg) return false;
    for (size_t i = 0; i < count; i++) {
        if (dbg[i].Type != IMAGE_DEBUG_TYPE_CODEVIEW || dbg[i].SizeOfData < 25) continue;
        const char* cv = f.At<char>(dbg[i].PointerToRawData, dbg[i].SizeOfData);
        if (!cv || memcmp(cv, "RSDS", 4) != 0) continue;
        memcpy(&out.guid, cv + 4, sizeof(GUID));
        memcpy(&out.age, cv + 20, sizeof(DWORD));
        std::string name(cv + 24, strnlen(cv + 24, dbg[i].SizeOfData - 24));
        out.pdbName = FileName(Widen(name));
        return !out.pdbName.empty();
    }
    return false;
}

std::wstring PdbId(const CodeView& cv) {
    const GUID& g = cv.guid;
    return Format(L"%08lX%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%lX", g.Data1, g.Data2, g.Data3, g.Data4[0],
                  g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7], cv.age);
}

std::wstring FixPtr64(std::wstring name) {
    // msdia keeps the space after a member function's cv-qualifier ("(void)const "),
    // so those two lose only "__ptr64"; every other " __ptr64" goes with its space.
    ReplaceAll(name, L"const __ptr64", L"const ");
    ReplaceAll(name, L"volatile __ptr64", L"volatile ");
    ReplaceAll(name, L" __ptr64", L"");
    return name;
}

// --- offsets files ---

std::string FormatOffsets(const OffsetsFile& f) {
    std::wstring s = Format(L"1#%ls#%lu-%lu", f.module.c_str(), f.timeDateStamp, f.sizeOfImage);
    for (auto& [name, rva] : f.entries) {
        s += L'#';
        s += name;
        s += L'#';
        if (rva) s += std::to_wstring(*rva);
    }
    return Narrow(s);
}

bool ParseOffsets(const std::string& utf8, OffsetsFile& out) {
    std::wstring text = Widen(utf8);
    while (!text.empty() && (text.back() == L'\n' || text.back() == L'\r')) text.pop_back();
    std::vector<std::wstring> parts;
    for (size_t start = 0;;) {
        size_t p = text.find(L'#', start);
        parts.push_back(text.substr(start, p == std::wstring::npos ? std::wstring::npos : p - start));
        if (p == std::wstring::npos) break;
        start = p + 1;
    }
    if (parts.size() < 3 || parts[0] != L"1" || (parts.size() - 3) % 2 != 0) return false;
    OffsetsFile f;
    f.module = parts[1];
    unsigned long stamp, size;
    wchar_t tail;
    if (swscanf(parts[2].c_str(), L"%lu-%lu%lc", &stamp, &size, &tail) != 2) return false;
    f.timeDateStamp = stamp;
    f.sizeOfImage = size;
    for (size_t i = 3; i + 1 < parts.size(); i += 2) {
        std::optional<DWORD> rva;
        if (!parts[i + 1].empty()) {
            wchar_t* end = nullptr;
            unsigned long long v = wcstoull(parts[i + 1].c_str(), &end, 10);
            if (!end || *end || v > 0xFFFFFFFFull) return false;
            rva = (DWORD)v;
        }
        f.entries.emplace_back(parts[i], rva);
    }
    out = std::move(f);
    return true;
}

std::wstring OffsetsFileName(const std::wstring& module, const CodeView& cv) {
    return Lower(module) + L"_" + PdbId(cv) + L".txt";
}

bool OffsetsMatch(const OffsetsFile& f, const std::wstring& module, const CodeView& cv,
                  const std::vector<std::wstring>& names) {
    if (f.module != Lower(module) || f.timeDateStamp != cv.timeDateStamp || f.sizeOfImage != cv.sizeOfImage)
        return false;
    std::unordered_set<std::wstring> have;
    for (auto& [name, rva] : f.entries) {
        if (rva && *rva >= cv.sizeOfImage) return false;
        have.insert(name);
    }
    for (auto& n : names)
        if (!have.count(n)) return false;
    return true;
}

// --- module list ---

std::vector<SymbolModule> AddonModules(const std::vector<std::wstring>& explorerModules) {
    wchar_t sys[MAX_PATH], win[MAX_PATH];
    GetSystemDirectoryW(sys, MAX_PATH);
    GetWindowsDirectoryW(win, MAX_PATH);
    std::wstring coreDir = JoinPath(win, L"SystemApps\\MicrosoftWindows.Client.Core_cw5n1h2txyewy");
    std::vector<SymbolModule> mods;
    for (const auto& m : addon::kSymbolModules) {
        SymbolModule s{m.name, JoinPath(m.inPackage ? coreDir : std::wstring(sys), m.name)};
        // A System32 module always loads from there; a package module lives in a
        // folder whose name can change, so a loaded copy wins.
        if (m.inPackage)
            for (auto& p : explorerModules)
                if (EqualsNoCase(FileName(p), s.name)) {
                    s.path = p;
                    break;
                }
        mods.push_back(std::move(s));
    }
    return mods;
}

bool ReadSymbolList(const std::wstring& path, std::vector<std::wstring>& names) {
    std::string bytes;
    if (!ReadFileBytes(path, bytes)) return false;
    std::wstring text = Widen(bytes);
    if (!text.empty() && text[0] == 0xFEFF) text.erase(0, 1);
    names.clear();
    size_t start = 0;
    while (start <= text.size()) {
        size_t p = text.find(L'\n', start);
        std::wstring line = text.substr(start, p == std::wstring::npos ? std::wstring::npos : p - start);
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (!line.empty()) names.push_back(line);
        if (p == std::wstring::npos) break;
        start = p + 1;
    }
    return !names.empty();
}

// --- dbghelp ---

namespace {

// System32's dbghelp only: an app-local or PATH copy could be any version, and the
// name rules above were checked against this one. It is not thread safe, and the
// host calls it from its main thread only.
class DbgHelp {
public:
    bool Load() {
        if (mod_) return true;
        mod_ = LoadLibraryExW(L"dbghelp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!mod_) return false;
        return Get(setOptions, "SymSetOptions") && Get(initialize, "SymInitializeW") &&
               Get(loadModule, "SymLoadModuleExW") && Get(enumSymbols, "SymEnumSymbolsW") &&
               Get(moduleInfo, "SymGetModuleInfoW64") && Get(unloadModule, "SymUnloadModule64") &&
               Get(cleanup, "SymCleanup") && Get(undecorate, "UnDecorateSymbolNameW");
    }

    decltype(&SymSetOptions) setOptions = nullptr;
    decltype(&SymInitializeW) initialize = nullptr;
    decltype(&SymLoadModuleExW) loadModule = nullptr;
    decltype(&SymEnumSymbolsW) enumSymbols = nullptr;
    decltype(&SymGetModuleInfoW64) moduleInfo = nullptr;
    decltype(&SymUnloadModule64) unloadModule = nullptr;
    decltype(&SymCleanup) cleanup = nullptr;
    decltype(&UnDecorateSymbolNameW) undecorate = nullptr;

private:
    template <typename T>
    bool Get(T& fn, const char* name) {
        fn = (T)(void*)GetProcAddress(mod_, name);
        return fn != nullptr;
    }
    HMODULE mod_ = nullptr;
};

DbgHelp g_dbghelp;

struct EnumContext {
    DbgHelp* dbg;
    ULONG64 base;
    const std::unordered_set<std::wstring>* wanted;
    std::unordered_map<std::wstring, DWORD> found;  // first match wins, enumeration order
    std::vector<std::pair<DWORD, std::wstring>>* dump;
    std::unordered_set<std::wstring> dumped;
    size_t count = 0;
};

BOOL CALLBACK OnSymbol(PSYMBOL_INFOW si, ULONG, PVOID ctxPtr) {
    auto* ctx = (EnumContext*)ctxPtr;
    ctx->count++;
    std::wstring name(si->Name, si->NameLen);
    if (!name.empty() && name[0] == L'?') {
        // UNDNAME_32_BIT_DECODE | UNDNAME_NO_PTR64, what msdia's get_undecoratedName uses.
        wchar_t out[8192];
        if (!ctx->dbg->undecorate(name.c_str(), out, ARRAYSIZE(out), 0x20800)) return TRUE;
        name = FixPtr64(out);
    }
    DWORD rva = (DWORD)(si->Address - ctx->base);
    if (ctx->wanted->count(name)) ctx->found.emplace(name, rva);
    if (ctx->dump && ctx->dumped.insert(name).second) ctx->dump->emplace_back(rva, name);
    return TRUE;
}

bool Resolve(const SymbolModule& m, const CodeView& cv, const std::wstring& pdbPath,
             const std::unordered_set<std::wstring>& wanted, std::unordered_map<std::wstring, DWORD>& found,
             std::vector<std::pair<DWORD, std::wstring>>* dump) {
    if (!g_dbghelp.Load()) {
        Log(L"dbghelp.dll from System32 did not load (%lu)", GetLastError());
        return false;
    }
    DbgHelp& d = g_dbghelp;
    // Publics only: the mod's names are all public symbols, and this keeps Windhawk's
    // order (publics first) when one name has two addresses.
    d.setOptions(SYMOPT_PUBLICS_ONLY | SYMOPT_EXACT_SYMBOLS | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS |
                 SYMOPT_IGNORE_NT_SYMPATH | SYMOPT_DEFERRED_LOADS);
    // No live process: the handle is only a key for dbghelp's own bookkeeping.
    HANDLE proc = (HANDLE)(ULONG_PTR)0x50544831;
    if (!d.initialize(proc, DirName(pdbPath).c_str(), FALSE)) {
        Log(L"SymInitialize failed (%lu)", GetLastError());
        return false;
    }
    const ULONG64 base = 0x180000000ull;  // any base; RVAs are taken relative to it
    bool ok = false;
    if (d.loadModule(proc, nullptr, m.path.c_str(), nullptr, base, cv.sizeOfImage, nullptr, 0)) {
        EnumContext ctx{&d, base, &wanted, {}, dump, {}, 0};
        d.enumSymbols(proc, base, L"*", OnSymbol, &ctx);
        IMAGEHLP_MODULEW64 mi{};
        mi.SizeOfStruct = sizeof(mi);
        bool infoOk = d.moduleInfo(proc, base, &mi);
        bool pdbOk = infoOk && mi.SymType == SymPdb && memcmp(&mi.PdbSig70, &cv.guid, sizeof(GUID)) == 0 &&
                     mi.PdbAge == cv.age;
        if (!pdbOk) {
            Log(L"%ls: dbghelp did not load the matching PDB (type %d)", m.name.c_str(),
                infoOk ? (int)mi.SymType : -1);
        } else {
            Log(L"%ls: %zu public symbols, %zu of %zu names found", m.name.c_str(), ctx.count, ctx.found.size(),
                wanted.size());
            found = std::move(ctx.found);
            ok = true;
        }
        d.unloadModule(proc, base);
    } else {
        Log(L"%ls: SymLoadModuleEx failed (%lu)", m.name.c_str(), GetLastError());
    }
    d.cleanup(proc);
    return ok;
}

// --- download ---

struct Internet {
    HINTERNET h = nullptr;
    explicit Internet(HINTERNET v) : h(v) {}
    ~Internet() {
        if (h) WinHttpCloseHandle(h);
    }
    explicit operator bool() const { return h != nullptr; }
};

enum class Download { Ok, Failed, Stopped };

// Gets one PDB from the Microsoft symbol server into dest. The server answers with a
// redirect to a signed storage URL; WinHTTP follows it and nothing keeps that URL.
Download DownloadPdb(const std::wstring& pdb, const std::wstring& id, const std::wstring& dest,
                     uint64_t doneBefore, const ProgressFn& progress, uint64_t& bytes) {
    std::wstring agent = std::wstring(ADDON_NAME L"Host/") + addon::kVersion;
    Internet session(WinHttpOpen(agent.c_str(), WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
                                 WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session)  // Windows before 8.1 has no automatic proxy type
        session.h = WinHttpOpen(agent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
                                WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return Download::Failed;
    WinHttpSetTimeouts(session.h, 30000, 30000, 30000, 60000);
    DWORD policy = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
    WinHttpSetOption(session.h, WINHTTP_OPTION_REDIRECT_POLICY, &policy, sizeof(policy));
    Internet conn(WinHttpConnect(session.h, L"msdl.microsoft.com", INTERNET_DEFAULT_HTTPS_PORT, 0));
    if (!conn) return Download::Failed;
    std::wstring path = L"/download/symbols/" + pdb + L"/" + id + L"/" + pdb;
    Internet req(WinHttpOpenRequest(conn.h, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                    WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE));
    if (!req) return Download::Failed;
    if (!WinHttpSendRequest(req.h, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(req.h, nullptr)) {
        Log(L"%ls: request failed (%lu)", pdb.c_str(), GetLastError());
        return Download::Failed;
    }
    DWORD status = 0, len = sizeof(status);
    WinHttpQueryHeaders(req.h, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
                        &status, &len, WINHTTP_NO_HEADER_INDEX);
    if (status != 200) {
        Log(L"%ls: server answered %lu", pdb.c_str(), status);
        return Download::Failed;
    }
    uint64_t total = 0;
    wchar_t lenText[32];
    len = sizeof(lenText);
    if (WinHttpQueryHeaders(req.h, WINHTTP_QUERY_CONTENT_LENGTH, WINHTTP_HEADER_NAME_BY_INDEX, lenText, &len,
                            WINHTTP_NO_HEADER_INDEX))
        total = wcstoull(lenText, nullptr, 10);

    std::wstring part = dest + L".part";
    Download result = Download::Ok;
    {
        Handle file(CreateFileW(part.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!file) return Download::Failed;
        std::vector<char> buf(256 * 1024);
        bytes = 0;
        while (true) {
            DWORD got = 0;
            if (!WinHttpReadData(req.h, buf.data(), (DWORD)buf.size(), &got)) {
                Log(L"%ls: read failed after %llu bytes (%lu)", pdb.c_str(), bytes, GetLastError());
                result = Download::Failed;
                break;
            }
            if (got == 0) break;
            DWORD written = 0;
            if (!WriteFile(file.get(), buf.data(), got, &written, nullptr) || written != got) {
                result = Download::Failed;
                break;
            }
            bytes += got;
            if (progress && !progress(doneBefore + bytes, doneBefore + std::max(total, bytes))) {
                result = Download::Stopped;
                break;
            }
        }
        if (result == Download::Ok && total && bytes != total) {
            Log(L"%ls: got %llu of %llu bytes", pdb.c_str(), bytes, total);
            result = Download::Failed;
        }
    }
    if (result != Download::Ok || !MoveFileExW(part.c_str(), dest.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        DeleteFileW(part.c_str());
        return result == Download::Ok ? Download::Failed : result;
    }
    return Download::Ok;
}

// Windhawk keeps the same symbol-store layout; the host only ever reads it.
std::wstring WindhawkSymbolsDir() {
    wchar_t buf[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"ProgramData", buf, MAX_PATH);
    std::wstring root = n && n < MAX_PATH ? buf : L"C:\\ProgramData";
    return JoinPath(root, L"Windhawk\\Engine\\Symbols");
}

bool WriteDump(const std::wstring& dir, const std::wstring& module,
               std::vector<std::pair<DWORD, std::wstring>>& names) {
    std::sort(names.begin(), names.end());
    std::wstring text;
    for (auto& [rva, n] : names) text += Format(L"%lx\t", rva) + n + L"\n";
    CreateDirs(dir);
    return WriteFileAtomic(JoinPath(dir, L"names-" + module + L".txt"), Narrow(text));
}

}  // namespace

int PrepareSymbols(const SymbolOptions& opt) {
    std::vector<std::wstring> names;
    if (!ReadSymbolList(opt.symbolList, names)) {
        Log(L"%ls is missing or empty", addon::kSymbolList);
        return addon::kExitError;
    }
    std::unordered_set<std::wstring> wanted(names.begin(), names.end());
    std::wstring offsetsDir = JoinPath(opt.dataDir, addon::kOffsetsDir);
    std::wstring symbolsDir = JoinPath(opt.dataDir, addon::kSymbolsDir);
    if (!CreateDirs(offsetsDir)) {
        Log(L"could not create the offsets folder (%lu)", GetLastError());
        return addon::kExitError;
    }
    uint64_t downloaded = 0;

    for (const SymbolModule& m : opt.modules) {
        CodeView cv;
        if (!FileExists(m.path)) {
            Log(L"%ls: not found at %ls", m.name.c_str(), m.path.c_str());
            return addon::kExitNoSymbols;
        }
        if (!ReadCodeView(m.path, cv)) {
            Log(L"%ls: no RSDS record", m.name.c_str());
            return addon::kExitNoSymbols;
        }
        std::wstring id = PdbId(cv);
        std::wstring offPath = JoinPath(offsetsDir, OffsetsFileName(m.name, cv));
        std::string existing;
        OffsetsFile old;
        if (opt.dumpDir.empty() && ReadFileBytes(offPath, existing) && ParseOffsets(existing, old) &&
            OffsetsMatch(old, m.name, cv, names)) {
            Log(L"%ls: offsets for %ls already there", m.name.c_str(), id.c_str());
            continue;
        }

        // Own cache, then Windhawk's (read only), then the symbol server.
        std::wstring ownDir = JoinPath(JoinPath(symbolsDir, cv.pdbName), id);
        std::wstring pdbPath = JoinPath(ownDir, cv.pdbName);
        bool ours = FileExists(pdbPath);
        if (!ours) {
            std::wstring wh = JoinPath(JoinPath(JoinPath(WindhawkSymbolsDir(), cv.pdbName), id), cv.pdbName);
            if (!opt.noWindhawkCache && FileExists(wh)) {
                pdbPath = wh;
                Log(L"%ls: using Windhawk's cached %ls", m.name.c_str(), cv.pdbName.c_str());
            } else if (opt.noDownload) {
                Log(L"%ls: %ls/%ls not cached and downloads are off", m.name.c_str(), cv.pdbName.c_str(), id.c_str());
                return addon::kExitDownload;
            } else {
                CreateDirs(ownDir);
                auto t0 = std::chrono::steady_clock::now();
                uint64_t bytes = 0;
                Download r = DownloadPdb(cv.pdbName, id, pdbPath, downloaded, opt.progress, bytes);
                if (r == Download::Stopped) {
                    RemoveEmptyDirsUpTo(ownDir, opt.dataDir);
                    return kSymbolsStopped;
                }
                if (r != Download::Ok) {
                    RemoveEmptyDirsUpTo(ownDir, opt.dataDir);
                    return addon::kExitDownload;
                }
                downloaded += bytes;
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0);
                Log(L"%ls: downloaded %ls, %llu bytes in %lld ms", m.name.c_str(), cv.pdbName.c_str(), bytes,
                    (long long)ms.count());
                ours = true;
            }
        }

        std::unordered_map<std::wstring, DWORD> found;
        std::vector<std::pair<DWORD, std::wstring>> dump;
        bool resolved = Resolve(m, cv, pdbPath, wanted, found, opt.dumpDir.empty() ? nullptr : &dump);
        OffsetsFile out;
        if (resolved) {
            out.module = Lower(m.name);
            out.timeDateStamp = cv.timeDateStamp;
            out.sizeOfImage = cv.sizeOfImage;
            for (auto& n : names) {
                auto it = found.find(n);
                out.entries.emplace_back(n, it == found.end() ? std::nullopt : std::optional<DWORD>(it->second));
            }
            resolved = WriteFileAtomic(offPath, FormatOffsets(out));
            if (!resolved) Log(L"%ls: could not write its offsets file (%lu)", m.name.c_str(), GetLastError());
        }
        if (resolved && !opt.dumpDir.empty() && !WriteDump(opt.dumpDir, m.name, dump))
            Log(L"%ls: could not write the name dump", m.name.c_str());
        // A PDB in our own folder only ever came from the symbol server; once the
        // offsets exist it is not needed. A broken one goes too, so the next try
        // downloads it again.
        if (ours) {
            DeleteFileW(pdbPath.c_str());
            RemoveEmptyDirsUpTo(ownDir, opt.dataDir);
        }
        if (!resolved) return addon::kExitError;
    }
    if (opt.progress && downloaded) opt.progress(downloaded, downloaded);
    return addon::kExitOk;
}

}  // namespace host
