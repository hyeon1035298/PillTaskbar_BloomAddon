// SPDX-License-Identifier: GPL-3.0-only
//
// Wh_GetIntSetting and Wh_GetStringSetting over values.json. A missing key answers
// the mod's YAML default rather than Windhawk's 0, so Bloom may write only what the
// user changed.

#include "internal.h"

#include <stdio.h>
#include <wchar.h>

#include <cstring>
#include <cwchar>
#include <optional>

namespace rt::settings {

namespace {

struct Default {
    int number = 0;
    bool isString = false;
    std::wstring text;
};

SRWLOCK g_lock = SRWLOCK_INIT;
JsonObject g_values;
std::unordered_map<std::wstring, Default> g_defaults;
std::optional<std::string> g_lastContent;

const wchar_t kEmpty[] = L"";

std::wstring FormatName(PCWSTR valueName, va_list args) {
    wchar_t buffer[256];
    int n = _vsnwprintf(buffer, 255, valueName, args);
    buffer[(n < 0 || n > 255) ? 255 : n] = L'\0';
    return buffer;
}

// A whole number in text, as a REG_SZ read as a number would need.
std::optional<int> ParseInt(const std::wstring& text) {
    if (text.empty()) {
        return std::nullopt;
    }
    wchar_t* end = nullptr;
    long value = wcstol(text.c_str(), &end, 10);
    if (*end) {
        return std::nullopt;
    }
    return (int)value;
}

PCWSTR Copy(const std::wstring& text) {
    auto* copy = new wchar_t[text.size() + 1];
    std::memcpy(copy, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
    return copy;
}

void SetModLog(bool on) {
    if (log::g_modLog != on) {
        log::g_modLog = on;
        log::Line(L"mod log %s", on ? L"on" : L"off");
    }
}

}  // namespace

Reread Reload() {
    bool configLog = (g_config.flags & addon::kConfigLog) != 0;
    bool first = !g_lastContent;
    std::string content;
    bool missing = false;
    if (!ReadFileBytes(DataPath(addon::kValuesFile), content, &missing)) {
        if (!missing) {
            // There but unreadable (held open, too big, cut short): keep what was read
            // before. A first read compares as an empty file afterwards.
            log::Line(L"cannot read %s (%u); keeping the previous values", addon::kValuesFile, GetLastError());
            if (first) {
                g_lastContent.emplace();
                SetModLog(configLog);
            }
            return Reread::kFailed;
        }
        content.clear();  // no file yet: every key answers its default
    }
    if (g_lastContent && *g_lastContent == content) {
        return Reread::kSame;
    }
    JsonObject values;
    if (!content.empty() && !ParseJsonObject(content, values)) {
        log::Line(L"%s is not a JSON object; keeping the previous values", addon::kValuesFile);
        g_lastContent = content;
        if (first) {
            SetModLog(configLog);
        }
        return Reread::kSame;
    }
    bool modLog = configLog;
    if (auto it = values.find(L"$log"); it != values.end()) {
        modLog = modLog || it->second.number != 0;
    }
    AcquireSRWLockExclusive(&g_lock);
    g_values = std::move(values);
    ReleaseSRWLockExclusive(&g_lock);
    g_lastContent = std::move(content);
    SetModLog(modLog);
    return first ? Reread::kSame : Reread::kChanged;
}

void Reset() {
    AcquireSRWLockExclusive(&g_lock);
    g_values.clear();
    g_defaults.clear();
    ReleaseSRWLockExclusive(&g_lock);
    g_lastContent.reset();
    log::g_modLog = false;
}

int RuntimeInt(PCWSTR name, int fallback) {
    int result = fallback;
    AcquireSRWLockShared(&g_lock);
    if (auto it = g_values.find(name); it != g_values.end() && it->second.kind == JsonValue::kNumber) {
        result = (int)it->second.number;
    }
    ReleaseSRWLockShared(&g_lock);
    return result;
}

void SetDefaults(const PillAddonDefault* defaults, size_t count) {
    std::unordered_map<std::wstring, Default> copy;
    for (size_t i = 0; i < count; i++) {
        Default d;
        d.number = defaults[i].intValue;
        if (defaults[i].stringValue) {
            d.isString = true;
            d.text = defaults[i].stringValue;
        }
        copy[defaults[i].name] = std::move(d);
    }
    AcquireSRWLockExclusive(&g_lock);
    g_defaults = std::move(copy);
    ReleaseSRWLockExclusive(&g_lock);
}

int GetInt(const std::wstring& name) {
    int result = 0;
    AcquireSRWLockShared(&g_lock);
    if (auto it = g_values.find(name); it != g_values.end()) {
        const JsonValue& v = it->second;
        if (v.kind == JsonValue::kNumber || v.kind == JsonValue::kBool) {
            result = (int)v.number;
            ReleaseSRWLockShared(&g_lock);
            return result;
        }
        if (v.kind == JsonValue::kString) {
            if (auto number = ParseInt(v.text)) {
                ReleaseSRWLockShared(&g_lock);
                return *number;
            }
        }
    }
    if (auto it = g_defaults.find(name); it != g_defaults.end()) {
        result = it->second.number;
    }
    ReleaseSRWLockShared(&g_lock);
    return result;
}

PCWSTR GetString(const std::wstring& name) {
    PCWSTR result = kEmpty;
    AcquireSRWLockShared(&g_lock);
    if (auto it = g_values.find(name); it != g_values.end() && it->second.kind == JsonValue::kString) {
        result = Copy(it->second.text);
    } else if (auto d = g_defaults.find(name); d != g_defaults.end() && d->second.isString) {
        result = Copy(d->second.text);
    }
    ReleaseSRWLockShared(&g_lock);
    return result;
}

}  // namespace rt::settings

using namespace rt;

extern "C" {

BOOL PillRt_IsLogEnabled(void) {
    return log::g_modLog ? TRUE : FALSE;
}

void PillRt_Log(PCWSTR format, va_list args) {
    log::Mod(format, args);
}

int PillRt_GetIntSetting(PCWSTR valueName, va_list args) {
    return settings::GetInt(settings::FormatName(valueName, args));
}

PCWSTR PillRt_GetStringSetting(PCWSTR valueName, va_list args) {
    return settings::GetString(settings::FormatName(valueName, args));
}

void PillRt_FreeStringSetting(PCWSTR string) {
    if (string && string != settings::kEmpty) {
        delete[] string;
    }
}

}  // extern "C"
