// SPDX-License-Identifier: GPL-3.0-only
//
// The functions PillTaskbarRuntime.dll exports for a mod DLL, and the table a mod
// DLL exports back (addon/addon_entry.cpp). A mod reaches these through the
// Windhawk-compatible inline wrappers in windhawk_api.h, never directly.

#pragma once

#include <windows.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef PILL_RUNTIME_BUILD
#define PILLRT_API __declspec(dllexport)
#else
#define PILLRT_API __declspec(dllimport)
#endif

typedef struct tagWH_FIND_SYMBOL_OPTIONS {
    size_t optionsSize;
    PCWSTR symbolServer;
    BOOL noUndecoratedSymbols;
} WH_FIND_SYMBOL_OPTIONS;

typedef struct tagWH_FIND_SYMBOL {
    void* address;
    PCWSTR symbol;
    PCWSTR symbolDecorated;
} WH_FIND_SYMBOL;

typedef struct tagWH_HOOK_SYMBOLS_OPTIONS {
    size_t optionsSize;
    PCWSTR symbolServer;
    BOOL noUndecoratedSymbols;
    PCWSTR onlineCacheUrl;
} WH_HOOK_SYMBOLS_OPTIONS;

typedef struct tagWH_DISASM_RESULT {
    size_t length;
    char text[96];
} WH_DISASM_RESULT;

typedef struct tagWH_GET_URL_CONTENT_OPTIONS {
    size_t optionsSize;
    PCWSTR targetFilePath;
} WH_GET_URL_CONTENT_OPTIONS;

typedef struct tagWH_URL_CONTENT {
    const char* data;
    size_t length;
    int statusCode;
} WH_URL_CONTENT;

// One HookSymbols entry as it crosses into the runtime. WindhawkUtils::SYMBOL_HOOK
// has the same layout (checked with static_assert in windhawk_utils.h). The names
// are counted, not NUL-terminated.
typedef struct tagWH_SYMBOL_NAME {
    PCWSTR string;
    size_t length;
} WH_SYMBOL_NAME;

typedef struct tagWH_SYMBOL_HOOK {
    const WH_SYMBOL_NAME* symbols;
    size_t symbolsCount;
    void** pOriginalFunction;
    void* hookFunction;
    bool optional;
} WH_SYMBOL_HOOK;

// A YAML default the runtime returns when values.json has no such key.
// stringValue is set for string and choice settings, null for numbers and switches.
typedef struct tagPILL_ADDON_DEFAULT {
    PCWSTR name;
    int intValue;
    PCWSTR stringValue;
} PillAddonDefault;

// What a mod DLL hands the runtime. The runtime calls these in Windhawk's order:
// init, apply queued hooks, afterInit ... beforeUninit, disable hooks, uninit.
#define PILL_ADDON_API_VERSION 1
typedef struct tagPILL_ADDON_CALLBACKS {
    DWORD size;
    DWORD apiVersion;
    PCWSTR modId;
    PCWSTR modVersion;
    BOOL (*init)(void);
    void (*afterInit)(void);
    void (*beforeUninit)(void);
    void (*uninit)(void);
    BOOL (*settingsChanged)(BOOL* reload);
    const PillAddonDefault* defaults;
    size_t defaultsCount;
} PillAddonCallbacks;

#define PILL_ADDON_ENTRY_NAME "PillAddonGetCallbacks"
typedef const PillAddonCallbacks* (*PillAddonGetCallbacks_t)(void);

#ifdef __cplusplus
extern "C" {
#endif

PILLRT_API BOOL PillRt_IsLogEnabled(void);
PILLRT_API void PillRt_Log(PCWSTR format, va_list args);

PILLRT_API int PillRt_GetIntSetting(PCWSTR valueName, va_list args);
PILLRT_API PCWSTR PillRt_GetStringSetting(PCWSTR valueName, va_list args);
PILLRT_API void PillRt_FreeStringSetting(PCWSTR string);

PILLRT_API BOOL PillRt_SetFunctionHook(void* targetFunction, void* hookFunction, void** originalFunction);
PILLRT_API BOOL PillRt_RemoveFunctionHook(void* targetFunction);
PILLRT_API BOOL PillRt_ApplyHookOperations(void);
PILLRT_API BOOL PillRt_HookSymbols(HMODULE module, const WH_SYMBOL_HOOK* symbolHooks,
                                   size_t symbolHooksCount, const WH_HOOK_SYMBOLS_OPTIONS* options);
PILLRT_API BOOL PillRt_Disasm(void* address, WH_DISASM_RESULT* result);

// Not used by PillTaskbar. Present so other Windhawk sources compile; they report failure.
PILLRT_API int PillRt_GetIntValue(PCWSTR valueName, int defaultValue);
PILLRT_API BOOL PillRt_SetIntValue(PCWSTR valueName, int value);
PILLRT_API size_t PillRt_GetStringValue(PCWSTR valueName, PWSTR stringBuffer, size_t bufferChars);
PILLRT_API BOOL PillRt_SetStringValue(PCWSTR valueName, PCWSTR value);
PILLRT_API size_t PillRt_GetBinaryValue(PCWSTR valueName, void* buffer, size_t bufferSize);
PILLRT_API BOOL PillRt_SetBinaryValue(PCWSTR valueName, const void* buffer, size_t bufferSize);
PILLRT_API BOOL PillRt_DeleteValue(PCWSTR valueName);
PILLRT_API size_t PillRt_GetModStoragePath(PWSTR pathBuffer, size_t bufferChars);
PILLRT_API HANDLE PillRt_FindFirstSymbol(HMODULE hModule, const WH_FIND_SYMBOL_OPTIONS* options,
                                         WH_FIND_SYMBOL* findData);
PILLRT_API BOOL PillRt_FindNextSymbol(HANDLE symSearch, WH_FIND_SYMBOL* findData);
PILLRT_API void PillRt_FindCloseSymbol(HANDLE symSearch);
PILLRT_API const WH_URL_CONTENT* PillRt_GetUrlContent(PCWSTR url, const WH_GET_URL_CONTENT_OPTIONS* options);
PILLRT_API void PillRt_FreeUrlContent(const WH_URL_CONTENT* content);

#ifdef __cplusplus
}
#endif
