// SPDX-License-Identifier: GPL-3.0-only
//
// The Windhawk mod API as PillTaskbarRuntime.dll provides it. A mod source built
// with "-include windhawk_api.h" against this folder compiles unchanged; every call
// goes to the runtime instead of windhawk.dll. Only the signatures follow Windhawk.

#pragma once

#include <windows.h>
#include <stdarg.h>

#include "pill_runtime_api.h"

#ifdef WH_MOD

#ifndef WH_MOD_ID
#error "WH_MOD_ID must be defined, e.g. -DWH_MOD_ID=L\"pill-taskbar\""
#endif
#ifndef WH_MOD_VERSION
#define WH_MOD_VERSION L"0"
#endif

// Formats only when logging is on, so arguments are not evaluated otherwise.
// %s takes a wide string and %S a narrow one (msvcrt rules).
inline void PillRt_LogLine(PCWSTR format, ...) {
    va_list args;
    va_start(args, format);
    PillRt_Log(format, args);
    va_end(args);
}

#define Wh_Log(message, ...)                                                        \
    do {                                                                            \
        if (PillRt_IsLogEnabled()) {                                                \
            PillRt_LogLine(L"[%d:%S]: " message, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        }                                                                           \
    } while (0)

inline int Wh_GetIntValue(PCWSTR valueName, int defaultValue) {
    return PillRt_GetIntValue(valueName, defaultValue);
}

inline BOOL Wh_SetIntValue(PCWSTR valueName, int value) {
    return PillRt_SetIntValue(valueName, value);
}

inline size_t Wh_GetStringValue(PCWSTR valueName, PWSTR stringBuffer, size_t bufferChars) {
    return PillRt_GetStringValue(valueName, stringBuffer, bufferChars);
}

inline BOOL Wh_SetStringValue(PCWSTR valueName, PCWSTR value) {
    return PillRt_SetStringValue(valueName, value);
}

inline size_t Wh_GetBinaryValue(PCWSTR valueName, void* buffer, size_t bufferSize) {
    return PillRt_GetBinaryValue(valueName, buffer, bufferSize);
}

inline BOOL Wh_SetBinaryValue(PCWSTR valueName, const void* buffer, size_t bufferSize) {
    return PillRt_SetBinaryValue(valueName, buffer, bufferSize);
}

inline BOOL Wh_DeleteValue(PCWSTR valueName) {
    return PillRt_DeleteValue(valueName);
}

inline size_t Wh_GetModStoragePath(PWSTR pathBuffer, size_t bufferChars) {
    return PillRt_GetModStoragePath(pathBuffer, bufferChars);
}

// The name may carry printf arguments, as in Windhawk.
inline int Wh_GetIntSetting(PCWSTR valueName, ...) {
    va_list args;
    va_start(args, valueName);
    int result = PillRt_GetIntSetting(valueName, args);
    va_end(args);
    return result;
}

// Free the result with Wh_FreeStringSetting.
inline PCWSTR Wh_GetStringSetting(PCWSTR valueName, ...) {
    va_list args;
    va_start(args, valueName);
    PCWSTR result = PillRt_GetStringSetting(valueName, args);
    va_end(args);
    return result;
}

inline void Wh_FreeStringSetting(PCWSTR string) {
    PillRt_FreeStringSetting(string);
}

// Creates the hook and fills *originalFunction at once; the hook goes live with the
// next Wh_ApplyHookOperations, or after Wh_ModInit returns.
inline BOOL Wh_SetFunctionHook(void* targetFunction, void* hookFunction, void** originalFunction) {
    return PillRt_SetFunctionHook(targetFunction, hookFunction, originalFunction);
}

inline BOOL Wh_RemoveFunctionHook(void* targetFunction) {
    return PillRt_RemoveFunctionHook(targetFunction);
}

// Fails before Wh_ModInit returns and while unloading. Stops the other threads.
inline BOOL Wh_ApplyHookOperations() {
    return PillRt_ApplyHookOperations();
}

inline HANDLE Wh_FindFirstSymbol(HMODULE hModule, const WH_FIND_SYMBOL_OPTIONS* options,
                                 WH_FIND_SYMBOL* findData) {
    return PillRt_FindFirstSymbol(hModule, options, findData);
}

inline BOOL Wh_FindNextSymbol(HANDLE symSearch, WH_FIND_SYMBOL* findData) {
    return PillRt_FindNextSymbol(symSearch, findData);
}

inline void Wh_FindCloseSymbol(HANDLE symSearch) {
    PillRt_FindCloseSymbol(symSearch);
}

// One x64 instruction in Zydis' Intel syntax.
inline BOOL Wh_Disasm(void* address, WH_DISASM_RESULT* result) {
    return PillRt_Disasm(address, result);
}

inline const WH_URL_CONTENT* Wh_GetUrlContent(PCWSTR url, const WH_GET_URL_CONTENT_OPTIONS* options) {
    return PillRt_GetUrlContent(url, options);
}

inline void Wh_FreeUrlContent(const WH_URL_CONTENT* content) {
    PillRt_FreeUrlContent(content);
}

#endif  // WH_MOD
