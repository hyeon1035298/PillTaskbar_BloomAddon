// SPDX-License-Identifier: GPL-3.0-only
//
// Keeps the add-on from running next to the Windhawk build of the same mod, which
// would hook every function twice (plan, design 5). Reads only; never writes
// Windhawk's registry or its service.
//
// The registry alone does not load anything: Windhawk's engine does, and only while
// windhawk.exe runs. So a mod that is on in Windhawk is a conflict while windhawk.exe
// runs or its service starts by itself at boot; with the service on manual (or gone)
// and no windhawk.exe, the add-on may run, and the host stops it if windhawk.exe
// appears later.

#pragma once

#include <string>
#include <vector>

namespace host {

struct WindhawkConflict {
    bool registryOn = false;  // the mod is on in Windhawk and Windhawk can load it
    bool loaded = false;      // explorer already has the Windhawk mod DLL
    std::wstring detail;
    bool any() const { return registryOn || loaded; }
};

// Whether Windhawk's engine runs now or will at the next boot.
struct WindhawkEngine {
    bool running = false;    // a windhawk.exe process
    bool autoStart = false;  // the "windhawk" service starts at boot
};
WindhawkEngine FindWindhawkEngine();

WindhawkConflict CheckWindhawkRegistry();
// Adds the module check to a registry result.
void CheckWindhawkModules(const std::vector<std::wstring>& explorerModules, WindhawkConflict& c);
// The registry rule for given engine facts (the self-test feeds it made-up ones).
WindhawkConflict WindhawkRule(bool modOn, bool modInstalled, const WindhawkEngine& engine);

}  // namespace host
