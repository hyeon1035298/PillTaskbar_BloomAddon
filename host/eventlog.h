// SPDX-License-Identifier: GPL-3.0-only
//
// Event log questions the host asks, best effort: did this explorer leave an
// Application Error (1000) event, and did code integrity block our DLL.

#pragma once

#include <windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace host {

// Looks for Application Error event 1000 for this pid (and creation time when the
// event carries one) written between from and to (FILETIME ticks). eventTime gets the
// event's time.
bool FindAppCrashEvent(DWORD pid, uint64_t created, uint64_t from, uint64_t to, uint64_t& eventTime);

// Looks for code integrity events 3033, 3076 or 3077 in the last minute that name
// one of the files.
bool FindCodeIntegrityBlock(const std::vector<std::wstring>& fileNames, std::wstring& which);

}  // namespace host
