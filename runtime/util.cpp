// SPDX-License-Identifier: GPL-3.0-only
//
// Strings, files, JSON and the log.

#include "internal.h"

#include <stdio.h>
#include <wchar.h>

#include <cstdlib>
#include <string>

namespace rt {

std::wstring DataPath(std::wstring_view name) {
    std::wstring path = g_config.dataDir;
    if (!path.empty() && path.back() != L'\\') {
        path += L'\\';
    }
    path += name;
    return path;
}

std::string Utf8(std::wstring_view text) {
    if (text.empty()) {
        return {};
    }
    int n = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string out(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), out.data(), n, nullptr, nullptr);
    return out;
}

std::wstring Wide(std::string_view text) {
    if (text.empty()) {
        return {};
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);
    std::wstring out(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), out.data(), n);
    return out;
}

std::string IsoNow() {
    SYSTEMTIME t;
    GetSystemTime(&t);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%04u-%02u-%02uT%02u:%02u:%02uZ", t.wYear, t.wMonth, t.wDay, t.wHour,
             t.wMinute, t.wSecond);
    return buffer;
}

bool ReadFileBytes(const std::wstring& path, std::string& out, bool* missing) {
    if (missing) {
        *missing = false;
    }
    for (int attempt = 0; attempt < 10; attempt++) {
        HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                  nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            if (error == ERROR_SHARING_VIOLATION || error == ERROR_ACCESS_DENIED) {
                Sleep(20);
                continue;
            }
            if (missing) {
                *missing = error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
            }
            return false;
        }
        LARGE_INTEGER size{};
        bool ok = GetFileSizeEx(file, &size) && size.QuadPart < (16 << 20);
        if (ok) {
            out.assign((size_t)size.QuadPart, '\0');
            DWORD read = 0;
            ok = out.empty() || (ReadFile(file, out.data(), (DWORD)out.size(), &read, nullptr) && read == out.size());
        }
        CloseHandle(file);
        return ok;
    }
    return false;
}

bool ReplaceFile(const std::wstring& path, std::string_view bytes) {
    std::wstring temp = path + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(file, bytes.data(), (DWORD)bytes.size(), &written, nullptr) && written == bytes.size();
    CloseHandle(file);
    for (int attempt = 0; ok && attempt < 10; attempt++) {
        if (MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            return true;
        }
        Sleep(20);  // a reader may hold the old file for a moment
    }
    DeleteFileW(temp.c_str());
    return false;
}

void WriteState(PCWSTR state, PCWSTR reason) {
    std::string json = "{\"v\":1,\"version\":" + JsonString(g_config.version) +
                       ",\"pid\":" + std::to_string(GetCurrentProcessId()) + ",\"state\":" + JsonString(state) +
                       ",\"reason\":" + JsonString(reason) + ",\"at\":\"" + IsoNow() + "\"}\n";
    if (!ReplaceFile(DataPath(addon::kRuntimeStateFile), json)) {
        log::Line(L"could not write %s", addon::kRuntimeStateFile);
    }
    log::Line(L"state %s%s%s", state, *reason ? L" " : L"", reason);
}

// ---- log ------------------------------------------------------------------------

namespace log {

std::atomic<bool> g_modLog;

namespace {

SRWLOCK g_lock = SRWLOCK_INIT;
HANDLE g_file = INVALID_HANDLE_VALUE;
constexpr LONGLONG kMaxSize = 1 << 20;
constexpr int kMaxChars = 1024;  // Windhawk cuts log lines here too

void Write(PCWSTR text) {
    SYSTEMTIME t;
    GetLocalTime(&t);
    wchar_t prefix[64];
    swprintf(prefix, 64, L"%04u-%02u-%02u %02u:%02u:%02u.%03u [%u] ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute,
             t.wSecond, t.wMilliseconds, GetCurrentThreadId());
    std::wstring line = prefix;
    line += text;
    line += L"\r\n";
    OutputDebugStringW((ADDON_NAME L" " + line).c_str());
    std::string bytes = Utf8(line);
    AcquireSRWLockExclusive(&g_lock);
    if (g_file != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER size{};
        if (GetFileSizeEx(g_file, &size) && size.QuadPart > kMaxSize) {
            SetFilePointer(g_file, 0, nullptr, FILE_BEGIN);
            SetEndOfFile(g_file);
        }
        SetFilePointer(g_file, 0, nullptr, FILE_END);
        DWORD written;
        WriteFile(g_file, bytes.data(), (DWORD)bytes.size(), &written, nullptr);
    }
    ReleaseSRWLockExclusive(&g_lock);
}

void Format(PCWSTR format, va_list args) {
    wchar_t buffer[kMaxChars + 1];
    int n = _vsnwprintf(buffer, kMaxChars, format, args);
    buffer[(n < 0 || n > kMaxChars) ? kMaxChars : n] = L'\0';
    Write(buffer);
}

}  // namespace

void Open(const std::wstring& path) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    AcquireSRWLockExclusive(&g_lock);
    g_file = file;
    ReleaseSRWLockExclusive(&g_lock);
}

void Close() {
    AcquireSRWLockExclusive(&g_lock);
    if (g_file != INVALID_HANDLE_VALUE) {
        CloseHandle(g_file);
        g_file = INVALID_HANDLE_VALUE;
    }
    ReleaseSRWLockExclusive(&g_lock);
}

void Line(PCWSTR format, ...) {
    va_list args;
    va_start(args, format);
    Format(format, args);
    va_end(args);
}

void Mod(PCWSTR format, va_list args) {
    if (g_modLog) {
        Format(format, args);
    }
}

}  // namespace log

// ---- JSON -----------------------------------------------------------------------

namespace {

class JsonReader {
   public:
    explicit JsonReader(std::string_view text) : s(text) {
        if (s.substr(0, 3) == "\xEF\xBB\xBF") {
            i = 3;
        }
    }

    bool Object(JsonObject& out) {
        Space();
        if (!Take('{')) {
            return false;
        }
        Space();
        if (Take('}')) {
            return End();
        }
        for (;;) {
            Space();
            std::wstring key;
            if (!String(key)) {
                return false;
            }
            Space();
            if (!Take(':')) {
                return false;
            }
            JsonValue value;
            if (!Value(value)) {
                return false;
            }
            out[key] = std::move(value);
            Space();
            if (Take(',')) {
                continue;
            }
            return Take('}') && End();
        }
    }

   private:
    std::string_view s;
    size_t i = 0;

    void Space() {
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) {
            i++;
        }
    }
    bool Take(char c) {
        if (i < s.size() && s[i] == c) {
            i++;
            return true;
        }
        return false;
    }
    bool Word(std::string_view word) {
        if (s.substr(i, word.size()) == word) {
            i += word.size();
            return true;
        }
        return false;
    }
    bool End() {
        Space();
        return i == s.size();
    }

    static void PutUtf8(std::string& out, unsigned cp) {
        if (cp < 0x80) {
            out += (char)cp;
        } else if (cp < 0x800) {
            out += (char)(0xC0 | (cp >> 6));
            out += (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            out += (char)(0xE0 | (cp >> 12));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        } else {
            out += (char)(0xF0 | (cp >> 18));
            out += (char)(0x80 | ((cp >> 12) & 0x3F));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        }
    }

    bool Hex4(unsigned& out) {
        if (i + 4 > s.size()) {
            return false;
        }
        out = 0;
        for (int k = 0; k < 4; k++) {
            char c = s[i++];
            out <<= 4;
            if (c >= '0' && c <= '9') {
                out |= c - '0';
            } else if (c >= 'a' && c <= 'f') {
                out |= c - 'a' + 10;
            } else if (c >= 'A' && c <= 'F') {
                out |= c - 'A' + 10;
            } else {
                return false;
            }
        }
        return true;
    }

    bool String(std::wstring& out) {
        if (!Take('"')) {
            return false;
        }
        std::string raw;
        while (i < s.size()) {
            char c = s[i++];
            if (c == '"') {
                out = Wide(raw);
                return true;
            }
            if ((unsigned char)c < 0x20) {
                return false;
            }
            if (c != '\\') {
                raw += c;
                continue;
            }
            if (i >= s.size()) {
                return false;
            }
            switch (char e = s[i++]) {
                case '"':
                case '\\':
                case '/':
                    raw += e;
                    break;
                case 'b':
                    raw += '\b';
                    break;
                case 'f':
                    raw += '\f';
                    break;
                case 'n':
                    raw += '\n';
                    break;
                case 'r':
                    raw += '\r';
                    break;
                case 't':
                    raw += '\t';
                    break;
                case 'u': {
                    unsigned cp;
                    if (!Hex4(cp)) {
                        return false;
                    }
                    if (cp >= 0xD800 && cp < 0xDC00 && Word("\\u")) {
                        unsigned low;
                        if (!Hex4(low) || low < 0xDC00 || low >= 0xE000) {
                            return false;
                        }
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                    }
                    PutUtf8(raw, cp);
                    break;
                }
                default:
                    return false;
            }
        }
        return false;
    }

    // Skips a nested object or array, minding strings.
    bool SkipNested() {
        int depth = 0;
        while (i < s.size()) {
            char c = s[i];
            if (c == '"') {
                std::wstring ignored;
                if (!String(ignored)) {
                    return false;
                }
                continue;
            }
            i++;
            if (c == '{' || c == '[') {
                depth++;
            } else if (c == '}' || c == ']') {
                if (--depth == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Number(JsonValue& out) {
        size_t start = i;
        Take('-');
        bool digits = false, fraction = false;
        while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
            i++;
            digits = true;
        }
        if (Take('.')) {
            fraction = true;
            while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
                i++;
            }
        }
        if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
            fraction = true;
            i++;
            if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
                i++;
            }
            while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
                i++;
            }
        }
        if (!digits) {
            return false;
        }
        std::string text(s.substr(start, i - start));
        out.kind = JsonValue::kNumber;
        out.number = fraction ? (long long)strtod(text.c_str(), nullptr) : strtoll(text.c_str(), nullptr, 10);
        return true;
    }

    bool Value(JsonValue& out) {
        Space();
        if (i >= s.size()) {
            return false;
        }
        char c = s[i];
        if (c == '"') {
            out.kind = JsonValue::kString;
            return String(out.text);
        }
        if (Word("true")) {
            out.kind = JsonValue::kBool;
            out.number = 1;
            return true;
        }
        if (Word("false")) {
            out.kind = JsonValue::kBool;
            out.number = 0;
            return true;
        }
        if (Word("null")) {
            out.kind = JsonValue::kNull;
            return true;
        }
        if (c == '{' || c == '[') {
            out.kind = JsonValue::kOther;
            return SkipNested();
        }
        return Number(out);
    }
};

}  // namespace

bool ParseJsonObject(std::string_view utf8, JsonObject& out) {
    JsonObject parsed;
    if (!JsonReader(utf8).Object(parsed)) {
        return false;
    }
    out = std::move(parsed);
    return true;
}

std::string JsonString(std::wstring_view text) {
    std::string out = "\"";
    for (char c : Utf8(text)) {
        switch (c) {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if ((unsigned char)c < 0x20) {
                    char escaped[8];
                    snprintf(escaped, sizeof(escaped), "\\u%04x", c);
                    out += escaped;
                } else {
                    out += c;
                }
        }
    }
    return out + "\"";
}

}  // namespace rt
