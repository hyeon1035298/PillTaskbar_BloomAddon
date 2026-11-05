// SPDX-License-Identifier: GPL-3.0-only
//
// Windhawk API calls PillTaskbar does not make: the per-mod value store, symbol
// enumeration and URL download. They report failure so other sources still build.

#include "internal.h"

extern "C" {

int PillRt_GetIntValue(PCWSTR, int defaultValue) {
    return defaultValue;
}

BOOL PillRt_SetIntValue(PCWSTR, int) {
    return FALSE;
}

size_t PillRt_GetStringValue(PCWSTR, PWSTR stringBuffer, size_t bufferChars) {
    if (stringBuffer && bufferChars) {
        stringBuffer[0] = L'\0';
    }
    return 0;
}

BOOL PillRt_SetStringValue(PCWSTR, PCWSTR) {
    return FALSE;
}

size_t PillRt_GetBinaryValue(PCWSTR, void*, size_t) {
    return 0;
}

BOOL PillRt_SetBinaryValue(PCWSTR, const void*, size_t) {
    return FALSE;
}

BOOL PillRt_DeleteValue(PCWSTR) {
    return FALSE;
}

size_t PillRt_GetModStoragePath(PWSTR pathBuffer, size_t bufferChars) {
    if (pathBuffer && bufferChars) {
        pathBuffer[0] = L'\0';
    }
    return 0;
}

HANDLE PillRt_FindFirstSymbol(HMODULE, const WH_FIND_SYMBOL_OPTIONS*, WH_FIND_SYMBOL*) {
    return nullptr;
}

BOOL PillRt_FindNextSymbol(HANDLE, WH_FIND_SYMBOL*) {
    return FALSE;
}

void PillRt_FindCloseSymbol(HANDLE) {}

const WH_URL_CONTENT* PillRt_GetUrlContent(PCWSTR, const WH_GET_URL_CONTENT_OPTIONS*) {
    return nullptr;
}

void PillRt_FreeUrlContent(const WH_URL_CONTENT*) {}

}  // extern "C"
