// SPDX-License-Identifier: GPL-3.0-only
//
// WindhawkUtils as PillTaskbarRuntime.dll provides it: typed hook helpers, symbol
// hooks and subclassing from any thread. Only the signatures follow Windhawk.

#pragma once

#include <windhawk_api.h>

#include <commctrl.h>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <type_traits>

namespace WindhawkUtils {

// One HookSymbols entry: the names to look for (the first one found is used), where
// to put the original function or the address, the hook (nullptr: address only),
// and whether a missing symbol is an error.
struct SYMBOL_HOOK {
    template <typename Prototype>
    SYMBOL_HOOK(std::initializer_list<std::wstring_view> names,
                Prototype** originalFunction,
                std::type_identity_t<Prototype*> hookFunction = nullptr,
                bool optional = false)
        : symbols(Copy(names)),
          symbolsCount(names.size()),
          originalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}

    template <typename Prototype>
    SYMBOL_HOOK(std::initializer_list<std::wstring_view> names,
                std::nullptr_t,
                Prototype* hookFunction = nullptr,
                bool optional = false)
        : symbols(Copy(names)),
          symbolsCount(names.size()),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}

    SYMBOL_HOOK() = default;

   private:
    // Same shape as WH_SYMBOL_NAME: the runtime reads these through that type.
    struct Name {
        std::unique_ptr<WCHAR[]> string;
        size_t length = 0;
    };

    static std::unique_ptr<Name[]> Copy(std::initializer_list<std::wstring_view> names) {
        auto copy = std::make_unique<Name[]>(names.size());
        Name* p = copy.get();
        for (std::wstring_view name : names) {
            p->string = std::make_unique<WCHAR[]>(name.size());
            std::copy(name.begin(), name.end(), p->string.get());
            p->length = name.size();
            p++;
        }
        return copy;
    }

    std::unique_ptr<Name[]> symbols;
    size_t symbolsCount = 0;
    void** originalFunction = nullptr;
    void* hookFunction = nullptr;
    bool optional = false;

    static void CheckLayout() {
        static_assert(sizeof(Name) == sizeof(WH_SYMBOL_NAME));
        static_assert(offsetof(Name, length) == offsetof(WH_SYMBOL_NAME, length));
        static_assert(sizeof(symbols) == sizeof(void*));
        static_assert(offsetof(SYMBOL_HOOK, symbolsCount) == offsetof(WH_SYMBOL_HOOK, symbolsCount));
        static_assert(offsetof(SYMBOL_HOOK, originalFunction) == offsetof(WH_SYMBOL_HOOK, pOriginalFunction));
        static_assert(offsetof(SYMBOL_HOOK, hookFunction) == offsetof(WH_SYMBOL_HOOK, hookFunction));
        static_assert(offsetof(SYMBOL_HOOK, optional) == offsetof(WH_SYMBOL_HOOK, optional));
    }
};

static_assert(sizeof(SYMBOL_HOOK) == sizeof(WH_SYMBOL_HOOK));

// Looks the names up in the offset table the host made for this module, then hooks
// or stores addresses. Fails, hooking nothing, when a required symbol is missing.
inline bool HookSymbols(HMODULE module,
                        const SYMBOL_HOOK* symbolHooks,
                        size_t symbolHooksCount,
                        const WH_HOOK_SYMBOLS_OPTIONS* options = nullptr) {
    return PillRt_HookSymbols(module, reinterpret_cast<const WH_SYMBOL_HOOK*>(symbolHooks),
                              symbolHooksCount, options);
}

template <typename Prototype>
BOOL SetFunctionHook(Prototype* targetFunction, Prototype* hookFunction, Prototype** originalFunction) {
    return Wh_SetFunctionHook(reinterpret_cast<void*>(targetFunction), reinterpret_cast<void*>(hookFunction),
                              reinterpret_cast<void**>(originalFunction));
}

template <typename Prototype>
BOOL SetFunctionHook(Prototype* targetFunction, Prototype* hookFunction, std::nullptr_t) {
    return Wh_SetFunctionHook(reinterpret_cast<void*>(targetFunction), reinterpret_cast<void*>(hookFunction),
                              nullptr);
}

// A string setting freed when it goes out of scope.
class StringSetting {
   public:
    StringSetting() = default;
    explicit StringSetting(PCWSTR value) : m_value(value) {}

    operator PCWSTR() const { return m_value.get(); }
    PCWSTR get() const { return m_value.get(); }

    template <typename... Args>
    static StringSetting make(PCWSTR valueName, Args... args) {
        return StringSetting(Wh_GetStringSetting(valueName, args...));
    }

   private:
    struct Free {
        void operator()(const WCHAR* value) const { Wh_FreeStringSetting(value); }
    };
    std::unique_ptr<const WCHAR[], Free> m_value;
};

using WH_SUBCLASSPROC = LRESULT(CALLBACK*)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                           DWORD_PTR dwRefData);

namespace detail {

// wParam TRUE: lParam is a SubclassRequest to install. wParam FALSE: lParam is the
// procedure to remove.
inline UINT SubclassMessage() {
    static UINT message = RegisterWindowMessageW(L"PillRt_SubclassFromAnyThread_" WH_MOD_ID);
    return message;
}

// The mod's procedure is the subclass id, so one function serves every procedure and
// a removal request can name the procedure.
inline LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                     UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY ||
        (uMsg == SubclassMessage() && !wParam && static_cast<UINT_PTR>(lParam) == uIdSubclass)) {
        RemoveWindowSubclass(hWnd, SubclassProc, uIdSubclass);
    }
    return reinterpret_cast<WH_SUBCLASSPROC>(uIdSubclass)(hWnd, uMsg, wParam, lParam, dwRefData);
}

struct SubclassRequest {
    UINT_PTR id;
    DWORD_PTR refData;
    BOOL result;
};

// Runs in the window's own thread while SendMessage waits, where SetWindowSubclass
// is allowed.
inline LRESULT CALLBACK InstallInWindowThread(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const auto* message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (message->message == SubclassMessage() && message->wParam) {
            auto* request = reinterpret_cast<SubclassRequest*>(message->lParam);
            request->result = SetWindowSubclass(message->hwnd, SubclassProc, request->id, request->refData);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

}  // namespace detail

// SetWindowSubclass works only in the window's thread; this hops there for a window
// of another thread of the same process.
inline BOOL SetWindowSubclassFromAnyThread(HWND hWnd, WH_SUBCLASSPROC pfnSubclass, DWORD_PTR dwRefData) {
    DWORD thread = GetWindowThreadProcessId(hWnd, nullptr);
    if (!thread) {
        return FALSE;
    }
    auto id = reinterpret_cast<UINT_PTR>(pfnSubclass);
    if (thread == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, detail::SubclassProc, id, dwRefData);
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, detail::InstallInWindowThread, nullptr, thread);
    if (!hook) {
        return FALSE;
    }
    detail::SubclassRequest request{id, dwRefData, FALSE};
    SendMessageW(hWnd, detail::SubclassMessage(), TRUE, reinterpret_cast<LPARAM>(&request));
    UnhookWindowsHookEx(hook);
    return request.result;
}

inline void RemoveWindowSubclassFromAnyThread(HWND hWnd, WH_SUBCLASSPROC pfnSubclass) {
    SendMessageW(hWnd, detail::SubclassMessage(), FALSE, reinterpret_cast<LPARAM>(pfnSubclass));
}

}  // namespace WindhawkUtils
