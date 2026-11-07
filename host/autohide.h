// SPDX-License-Identifier: GPL-3.0-only
//
// The island needs Windows' own taskbar auto-hide, so the add-on keeps it on while
// the island is on. It is checked each time the runtime is in a taskbar, which also
// covers a new explorer that came up with auto-hide off.

#pragma once

#include <string>

#include "json.h"

namespace host {

// True when the mod runs the island: island.enabled and autohide.enabled both on.
// values.json leaves out a value at its default, so a missing key takes the
// default from the settings schema (groups[].items[].default).
bool IslandWanted(const json::Value& values, const json::Value& schema);

// Turns Windows' auto-hide on when the island is on (values.json in dataDir, then
// settings.schema.json beside the host) and auto-hide is off. Logs what it did;
// never turns auto-hide off.
void KeepAutoHideForIsland(const std::wstring& dataDir);

}  // namespace host
