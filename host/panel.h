// SPDX-License-Identifier: GPL-3.0-only
//
// ShellHost.exe places Quick Settings itself, so the mod's panels part has to run in
// it too. The runtime goes in the same way as into explorer (inject.h: Config, the
// three events and the WH_GETMESSAGE hook, named by ShellHost's pid) with
// addon::kConfigPanel set; docs/addon-protocol.md section 4, "ShellHost".

#pragma once

#include <windows.h>

#include "taskbar.h"

namespace host {

// A top-level window of System32\ShellHost.exe on this desktop. Its thread takes
// messages, so the hook reaches the process through it. In out, tray holds that
// window (Explorer and HookUntilLoaded post WM_NULL to it).
bool FindShellHost(Taskbar& out);

}  // namespace host
