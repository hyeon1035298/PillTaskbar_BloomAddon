// SPDX-License-Identifier: GPL-3.0-only

#include "autohide.h"

#include <windows.h>
#include <shellapi.h>

#include "addon_protocol.h"
#include "json.h"
#include "util.h"

namespace host {

void KeepAutoHideForIsland(const std::wstring& dataDir) {
    std::string bytes;
    json::Value values;
    if (!ReadFileBytes(JoinPath(dataDir, addon::kValuesFile), bytes) || !json::Parse(bytes, values) ||
        !values.Get(L"island.enabled").AsBool()) {
        return;
    }
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &data);
    if (state & ABS_AUTOHIDE) return;
    data.lParam = state | ABS_AUTOHIDE;
    SHAppBarMessage(ABM_SETSTATE, &data);
    bool on = SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE;
    Log(on ? L"taskbar auto-hide was off; turned it on for the island"
           : L"taskbar auto-hide is off and did not turn on");
}

}  // namespace host
