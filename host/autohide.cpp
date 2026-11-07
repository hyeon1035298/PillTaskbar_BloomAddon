// SPDX-License-Identifier: GPL-3.0-only

#include "autohide.h"

#include <windows.h>
#include <shellapi.h>

#include "addon_protocol.h"
#include "util.h"

namespace host {

namespace {

// Written by tools/make-schema.py next to the host.
constexpr wchar_t kSchemaFile[] = L"settings.schema.json";

const json::Value& SchemaDefault(const json::Value& schema, const std::wstring& key) {
    static const json::Value kNull;
    for (auto& group : schema.Get(L"groups").items)
        for (auto& item : group.Get(L"items").items)
            if (item.Get(L"key").AsString() == key) return item.Get(L"default");
    return kNull;
}

}  // namespace

bool IslandWanted(const json::Value& values, const json::Value& schema) {
    auto on = [&](const wchar_t* key) {
        const json::Value& v = values.Get(key);
        return (v.IsNull() ? SchemaDefault(schema, key) : v).AsBool();
    };
    return on(L"island.enabled") && on(L"autohide.enabled");
}

void KeepAutoHideForIsland(const std::wstring& dataDir) {
    std::string bytes;
    json::Value values, schema;
    // No values.json: every setting at its default. One that can't be read: nothing is known.
    std::wstring valuesFile = JoinPath(dataDir, addon::kValuesFile);
    if (FileExists(valuesFile) && (!ReadFileBytes(valuesFile, bytes) || !json::Parse(bytes, values))) return;
    if (ReadFileBytes(JoinPath(ExeDir(), kSchemaFile), bytes) && !json::Parse(bytes, schema)) schema = {};
    if (!IslandWanted(values, schema)) return;
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
