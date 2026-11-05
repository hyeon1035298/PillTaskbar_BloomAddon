// SPDX-License-Identifier: GPL-3.0-only
//
// The wait before a mod DLL is freed. Switching its hooks off stops new calls, but a
// thread may still be inside a hooked function, called from the mod's detour, and
// would return into a freed image. Each thread is suspended only long enough to read
// its registers and copy its stack; nothing that allocates or takes a lock runs
// while it is stopped, since the thread might hold the heap or loader lock. The copy
// is then searched for return addresses into the mod: values that point into its
// code right after a call instruction. A stale value left in an uninitialized stack
// slot can look the same; that only makes the wait longer, or keeps the mod mapped.

#include "internal.h"

#include <tlhelp32.h>

#include <algorithm>

namespace rt::threads {

namespace {

constexpr SIZE_T kStackWindow = 1 << 20;  // bytes copied from RSP upward
constexpr DWORD kRetryMs = 400;

struct ThreadBasicInformation {
    LONG exitStatus;
    PVOID tebBaseAddress;
    HANDLE clientId[2];  // CLIENT_ID: process and thread id
    ULONG_PTR affinityMask;
    LONG priority;
    LONG basePriority;
};
using NtQueryInformationThread_t = LONG(NTAPI*)(HANDLE, int, PVOID, ULONG, PULONG);

const CodeRange* Find(const std::vector<CodeRange>& ranges, uintptr_t address) {
    for (const CodeRange& r : ranges) {
        if (address >= r.start && address < r.end) {
            return &r;
        }
    }
    return nullptr;
}

// Whether the bytes before address end with a call: E8 rel32 or one of the FF /2
// forms (register, memory, RIP-relative, with SIB and displacements).
bool AfterCall(const CodeRange& range, uintptr_t address) {
    auto at = [&](int back) { return *reinterpret_cast<const BYTE*>(address - back); };
    auto has = [&](int back) { return address - range.start >= (uintptr_t)back; };
    if (has(5) && at(5) == 0xE8) {
        return true;
    }
    if (has(2) && at(2) == 0xFF) {
        BYTE m = at(1);
        if ((m >= 0xD0 && m <= 0xD7) || (m >= 0x10 && m <= 0x17 && m != 0x14 && m != 0x15)) {
            return true;
        }
    }
    if (has(3) && at(3) == 0xFF && (at(2) == 0x14 || (at(2) >= 0x50 && at(2) <= 0x57 && at(2) != 0x54))) {
        return true;
    }
    if (has(4) && at(4) == 0xFF && at(3) == 0x54) {
        return true;
    }
    if (has(6) && at(6) == 0xFF && (at(5) == 0x15 || (at(5) >= 0x90 && at(5) <= 0x97 && at(5) != 0x94))) {
        return true;
    }
    if (has(7) && at(7) == 0xFF && (at(6) == 0x94 || at(6) == 0x14)) {
        return true;
    }
    return false;
}

class Scanner {
   public:
    ~Scanner() {
        if (m_copy) {
            VirtualFree(m_copy, 0, MEM_RELEASE);
        }
    }

    bool Ready() {
        if (!m_query) {
            m_query = reinterpret_cast<NtQueryInformationThread_t>(
                reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationThread")));
        }
        if (!m_copy) {
            m_copy = static_cast<BYTE*>(VirtualAlloc(nullptr, kStackWindow, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        }
        return m_query && m_copy;
    }

    // The id of a thread still in the ranges, 0 when there is none. *failed is set
    // when the threads could not be listed.
    DWORD FindBusy(const std::vector<CodeRange>& ranges, bool* failed) {
        *failed = false;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            *failed = true;
            return 0;
        }
        DWORD busy = 0;
        THREADENTRY32 entry{};
        entry.dwSize = sizeof(entry);
        DWORD pid = GetCurrentProcessId(), self = GetCurrentThreadId();
        for (BOOL more = Thread32First(snapshot, &entry); more && !busy; more = Thread32Next(snapshot, &entry)) {
            if (entry.th32OwnerProcessID == pid && entry.th32ThreadID != self && InRanges(entry.th32ThreadID, ranges)) {
                busy = entry.th32ThreadID;
            }
        }
        CloseHandle(snapshot);
        return busy;
    }

   private:
    NtQueryInformationThread_t m_query = nullptr;
    BYTE* m_copy = nullptr;

    bool InRanges(DWORD tid, const std::vector<CodeRange>& ranges) {
        HANDLE thread =
            OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, tid);
        if (!thread) {
            return false;  // already gone
        }
        // The stack bounds from the TEB, read before the thread stops. A TEB can go
        // away with its thread, so it is read through ReadProcessMemory, which fails
        // instead of faulting.
        uintptr_t stackBase = 0, stackLimit = 0;
        ThreadBasicInformation info{};
        if (m_query(thread, 0 /* ThreadBasicInformation */, &info, sizeof(info), nullptr) >= 0 &&
            info.tebBaseAddress) {
            NT_TIB tib{};
            if (ReadProcessMemory(GetCurrentProcess(), info.tebBaseAddress, &tib, sizeof(tib), nullptr)) {
                stackBase = reinterpret_cast<uintptr_t>(tib.StackBase);
                stackLimit = reinterpret_cast<uintptr_t>(tib.StackLimit);
            }
        }

        if (SuspendThread(thread) == (DWORD)-1) {
            CloseHandle(thread);
            return false;  // exiting
        }
        // From here to ResumeThread: system calls only.
        alignas(16) CONTEXT context{};
        context.ContextFlags = CONTEXT_CONTROL;
        bool gotContext = GetThreadContext(thread, &context);
        SIZE_T copied = 0;
        if (gotContext) {
            uintptr_t sp = context.Rsp;
            // Outside its own stack (a fiber, say) the window is read as far as it is
            // readable.
            uintptr_t top = sp >= stackLimit && sp < stackBase ? stackBase : sp + kStackWindow;
            SIZE_T size = (SIZE_T)std::min<uintptr_t>(top - sp, kStackWindow);
            if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(sp), m_copy, size, &copied) &&
                GetLastError() != ERROR_PARTIAL_COPY) {
                copied = 0;
            }
        }
        ResumeThread(thread);
        CloseHandle(thread);

        if (!gotContext) {
            return true;  // cannot tell; ask again next round
        }
        if (Find(ranges, context.Rip)) {
            return true;
        }
        auto* slots = reinterpret_cast<const uintptr_t*>(m_copy);
        for (SIZE_T i = 0; i < copied / sizeof(uintptr_t); i++) {
            uintptr_t value = slots[i];
            const CodeRange* range = Find(ranges, value);
            if (range && AfterCall(*range, value)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace

void AddImageCode(HMODULE module, std::vector<CodeRange>& out) {
    auto* base = reinterpret_cast<const BYTE*>(module);
    auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + reinterpret_cast<const IMAGE_DOS_HEADER*>(base)->e_lfanew);
    const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
        if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
            uintptr_t start = reinterpret_cast<uintptr_t>(base) + section->VirtualAddress;
            DWORD size = section->Misc.VirtualSize ? section->Misc.VirtualSize : section->SizeOfRawData;
            out.push_back({start, start + size});
        }
    }
}

bool WaitUntilOut(const std::vector<CodeRange>& ranges, ULONGLONG deadline) {
    Scanner scanner;
    if (!scanner.Ready()) {
        log::Line(L"cannot check the threads (%u)", GetLastError());
        return false;
    }
    DWORD reported = 0;
    for (;;) {
        bool failed = false;
        DWORD busy = scanner.FindBusy(ranges, &failed);
        if (failed) {
            log::Line(L"cannot list the threads (%u)", GetLastError());
            return false;
        }
        if (!busy) {
            if (reported) {
                log::Line(L"the threads have left the mod's code");
            }
            return true;
        }
        ULONGLONG now = GetTickCount64();
        if (now >= deadline) {
            log::Line(L"thread %u is still in the mod's code", busy);
            return false;
        }
        if (busy != reported) {
            log::Line(L"waiting for thread %u to leave the mod's code", busy);
            reported = busy;
        }
        Sleep((DWORD)std::min<ULONGLONG>(kRetryMs, deadline - now));
    }
}

}  // namespace rt::threads
