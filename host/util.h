// SPDX-License-Identifier: GPL-3.0-only
//
// Small helpers every part of the host uses: strings, paths, files, time, output.

#pragma once

#include <windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace host {

// --- strings ---
std::wstring Widen(const std::string& s);
std::string Narrow(const std::wstring& w);
std::wstring Lower(std::wstring s);
bool EqualsNoCase(const std::wstring& a, const std::wstring& b);
bool StartsWithNoCase(const std::wstring& s, const std::wstring& prefix);
std::wstring Format(const wchar_t* fmt, ...);
void ReplaceAll(std::wstring& s, const std::wstring& from, const std::wstring& to);

// --- paths and files ---
std::wstring JoinPath(const std::wstring& a, const std::wstring& b);
std::wstring DirName(const std::wstring& path);
std::wstring FileName(const std::wstring& path);
std::wstring ExeDir();
bool FileExists(const std::wstring& path);
bool CreateDirs(const std::wstring& path);
bool ReadFileBytes(const std::wstring& path, std::string& out);
// Writes to "<path>.tmp" and renames it over path, so readers never see half a file.
bool WriteFileAtomic(const std::wstring& path, const std::string& bytes);
// Removes empty folders from dir up to (not including) stop.
void RemoveEmptyDirsUpTo(std::wstring dir, const std::wstring& stop);

// --- time (FILETIME ticks, UTC) ---
constexpr uint64_t kTicksPerMs = 10000;
constexpr uint64_t kTicksPerSecond = 1000 * kTicksPerMs;
constexpr uint64_t kTicksPerMinute = 60 * kTicksPerSecond;
uint64_t NowTicks();
uint64_t ToTicks(const FILETIME& ft);
std::wstring IsoTime(uint64_t ticks, bool millis = false);  // 2026-01-01T00:00:00Z
bool ParseIsoTime(const std::wstring& s, uint64_t& ticks);

// --- output ---
// Console output for the check modes. Uses the inherited stdout (a pipe or file)
// when there is one, else the parent's console; never used by --serve, whose
// stdout Bloom may have redirected without reading it.
void Print(const std::wstring& text);
void EnablePrint();

// host.log in the data folder. Lines carry a UTC timestamp and no personal paths.
void OpenLog(const std::wstring& dataDir);
void Log(const wchar_t* fmt, ...);

// RAII for kernel handles.
struct Handle {
    HANDLE h = nullptr;
    Handle() = default;
    explicit Handle(HANDLE v) : h(v == INVALID_HANDLE_VALUE ? nullptr : v) {}
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    Handle(Handle&& o) noexcept : h(o.h) { o.h = nullptr; }
    Handle& operator=(Handle&& o) noexcept {
        if (this != &o) {
            Reset();
            h = o.h;
            o.h = nullptr;
        }
        return *this;
    }
    ~Handle() { Reset(); }
    void Reset(HANDLE v = nullptr) {
        if (h) CloseHandle(h);
        h = v == INVALID_HANDLE_VALUE ? nullptr : v;
    }
    explicit operator bool() const { return h != nullptr; }
    HANDLE get() const { return h; }
};

}  // namespace host
