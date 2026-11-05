// SPDX-License-Identifier: GPL-3.0-only
//
// The island needs Windows' own taskbar auto-hide, so the add-on keeps it on while
// the island is on. It is checked each time the runtime is in a taskbar, which also
// covers a new explorer that came up with auto-hide off.

#pragma once

#include <string>

namespace host {

// Turns Windows' auto-hide on when values.json in dataDir has the island on and
// auto-hide is off. Logs what it did; never turns auto-hide off.
void KeepAutoHideForIsland(const std::wstring& dataDir);

}  // namespace host
