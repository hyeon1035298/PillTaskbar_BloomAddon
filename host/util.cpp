// SPDX-License-Identifier: GPL-3.0-only

#include "util.h"

#include <cstdarg>
#include <cwchar>
#include <cwctype>

namespace host {

std::wstring Widen(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

std::string Narrow(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}

std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

bool EqualsNoCase(const std::wstring& a, const std::wstring& b) {
    return CompareStringOrdinal(a.data(), (int)a.size(), b.data(), (int)b.size(), TRUE) == CSTR_EQUAL;
}

bool StartsWithNoCase(const std::wstring& s, const std::wstring& prefix) {
    return s.size() >= prefix.size() && EqualsNoCase(s.substr(0, prefix.size()), prefix);
}

std::wstring Format(const wchar_t* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    va_list ap2;
    va_copy(ap2, ap);
    int n = _vscwprintf(fmt, ap);
    va_end(ap);
    std::wstring s(n > 0 ? n : 0, L'\0');
    if (n > 0) _vsnwprintf(s.data(), n + 1, fmt, ap2);
    va_end(ap2);
    return s;
}

void ReplaceAll(std::wstring& s, const std::wstring& from, const std::wstring& to) {
    for (size_t p = 0; (p = s.find(from, p)) != std::wstring::npos; p += to.size())
        s.replace(p, from.size(), to);
}

std::wstring JoinPath(const std::wstring& a, const std::wstring& b) {
    if (a.empty()) return b;
    if (a.back() == L'\\' || a.back() == L'/') return a + b;
    return a + L'\\' + b;
}

std::wstring DirName(const std::wstring& path) {
    size_t p = path.find_last_of(L"\\/");
    return p == std::wstring::npos ? std::wstring() : path.substr(0, p);
}

std::wstring FileName(const std::wstring& path) {
    size_t p = path.find_last_of(L"\\/");
    return p == std::wstring::npos ? path : path.substr(p + 1);
}

std::wstring ExeDir() {
    std::wstring buf(32768, L'\0');
    DWORD n = GetModuleFileNameW(nullptr, buf.data(), (DWORD)buf.size());
    buf.resize(n);
    return DirName(buf);
}

bool FileExists(const std::wstring& path) {
    DWORD a = GetFileAttributesW(path.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

bool CreateDirs(const std::wstring& path) {
    if (path.empty()) return false;
    DWORD a = GetFileAttributesW(path.c_str());
    if (a != INVALID_FILE_ATTRIBUTES) return (a & FILE_ATTRIBUTE_DIRECTORY) != 0;
    std::wstring parent = DirName(path);
    if (!parent.empty() && parent != path && !(parent.size() == 2 && parent[1] == L':'))
        CreateDirs(parent);
    return CreateDirectoryW(path.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS;
}

bool ReadFileBytes(const std::wstring& path, std::string& out) {
    Handle f(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         nullptr, OPEN_EXISTING, 0, nullptr));
    if (!f) return false;
    LARGE_INTEGER size;
    if (!GetFileSizeEx(f.get(), &size) || size.QuadPart > 64 * 1024 * 1024) return false;
    out.resize((size_t)size.QuadPart);
    DWORD read = 0;
    if (!out.empty() && (!ReadFile(f.get(), out.data(), (DWORD)out.size(), &read, nullptr) || read != out.size()))
        return false;
    return true;
}

bool WriteFileAtomic(const std::wstring& path, const std::string& bytes) {
    std::wstring tmp = path + L".tmp";
    {
        Handle f(CreateFileW(tmp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!f) return false;
        DWORD written = 0;
        if (!WriteFile(f.get(), bytes.data(), (DWORD)bytes.size(), &written, nullptr) || written != bytes.size())
            return false;
    }
    // A reader (Bloom) that opened the target without FILE_SHARE_DELETE blocks the
    // rename for a moment; try a few times before giving up.
    for (int i = 0; i < 10; i++) {
        if (MoveFileExW(tmp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) return true;
        Sleep(20);
    }
    DeleteFileW(tmp.c_str());
    return false;
}

void RemoveEmptyDirsUpTo(std::wstring dir, const std::wstring& stop) {
    while (!dir.empty() && !EqualsNoCase(dir, stop) && dir.size() > stop.size()) {
        if (!RemoveDirectoryW(dir.c_str())) break;
        dir = DirName(dir);
    }
}

uint64_t ToTicks(const FILETIME& ft) {
    return ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}

uint64_t NowTicks() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ToTicks(ft);
}

std::wstring IsoTime(uint64_t ticks, bool millis) {
    FILETIME ft{(DWORD)ticks, (DWORD)(ticks >> 32)};
    SYSTEMTIME st{};
    FileTimeToSystemTime(&ft, &st);
    if (millis)
        return Format(L"%04u-%02u-%02uT%02u:%02u:%02u.%03uZ", st.wYear, st.wMonth, st.wDay, st.wHour,
                      st.wMinute, st.wSecond, st.wMilliseconds);
    return Format(L"%04u-%02u-%02uT%02u:%02u:%02uZ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
                  st.wSecond);
}

bool ParseIsoTime(const std::wstring& s, uint64_t& ticks) {
    SYSTEMTIME st{};
    unsigned y, mo, d, h, mi, se;
    wchar_t z = 0;
    if (swscanf(s.c_str(), L"%4u-%2u-%2uT%2u:%2u:%2u%lc", &y, &mo, &d, &h, &mi, &se, &z) != 7) return false;
    if (z != L'Z' && z != L'.') return false;
    st.wYear = (WORD)y;
    st.wMonth = (WORD)mo;
    st.wDay = (WORD)d;
    st.wHour = (WORD)h;
    st.wMinute = (WORD)mi;
    st.wSecond = (WORD)se;
    FILETIME ft;
    if (!SystemTimeToFileTime(&st, &ft)) return false;
    ticks = ToTicks(ft);
    return true;
}

// --- output ---

namespace {
bool g_printEnabled = false;
HANDLE g_out = nullptr;
bool g_outIsConsole = false;

void InitOutput() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h && h != INVALID_HANDLE_VALUE && GetFileType(h) != FILE_TYPE_UNKNOWN) {
        DWORD mode;
        g_out = h;
        g_outIsConsole = GetConsoleMode(h, &mode) != 0;
        return;
    }
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        g_out = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                            OPEN_EXISTING, 0, nullptr);
        if (g_out == INVALID_HANDLE_VALUE) g_out = nullptr;
        g_outIsConsole = g_out != nullptr;
    }
}

Handle g_log;
}  // namespace

void EnablePrint() {
    if (g_printEnabled) return;
    g_printEnabled = true;
    InitOutput();
}

void Print(const std::wstring& text) {
    if (!g_printEnabled || !g_out) return;
    std::wstring line = text + L"\n";
    DWORD n = 0;
    if (g_outIsConsole) {
        WriteConsoleW(g_out, line.data(), (DWORD)line.size(), &n, nullptr);
    } else {
        std::string u = Narrow(line);
        WriteFile(g_out, u.data(), (DWORD)u.size(), &n, nullptr);
    }
}

void OpenLog(const std::wstring& dataDir) {
    std::wstring path = JoinPath(dataDir, L"host.log");
    WIN32_FILE_ATTRIBUTE_DATA fa{};
    bool restart = GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fa) &&
                   (((uint64_t)fa.nFileSizeHigh << 32) | fa.nFileSizeLow) > 1024 * 1024;
    g_log.Reset(CreateFileW(path.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            nullptr, restart ? CREATE_ALWAYS : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
}

void Log(const wchar_t* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    va_list ap2;
    va_copy(ap2, ap);
    int n = _vscwprintf(fmt, ap);
    va_end(ap);
    std::wstring msg(n > 0 ? n : 0, L'\0');
    if (n > 0) _vsnwprintf(msg.data(), n + 1, fmt, ap2);
    va_end(ap2);

    Print(msg);
    if (!g_log) return;
    std::string line = Narrow(Format(L"%ls [%lu] %ls\r\n", IsoTime(NowTicks(), true).c_str(),
                                      GetCurrentProcessId(), msg.c_str()));
    DWORD written;
    WriteFile(g_log.get(), line.data(), (DWORD)line.size(), &written, nullptr);
}

}  // namespace host
