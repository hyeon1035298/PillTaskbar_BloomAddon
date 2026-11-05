// SPDX-License-Identifier: GPL-3.0-only
//
// Built into the add-on's mod DLL next to the unchanged mod source. Hands the runtime
// the mod's five Windhawk callbacks and its setting defaults through one C export, so
// the runtime never looks up compiler-specific (decorated) names.

#include <windows.h>

#include "pill_runtime_api.h"
#include "settings_defaults.h"

BOOL Wh_ModInit();

// A mod may leave out every callback but Wh_ModInit, and may write the settings
// callback without arguments. As in Windhawk, a missing one does nothing and the old
// form means "no reload". The mod's own definitions replace these weak ones.
__attribute__((weak)) void Wh_ModAfterInit() {}
__attribute__((weak)) void Wh_ModBeforeUninit() {}
__attribute__((weak)) void Wh_ModUninit() {}
__attribute__((weak)) void Wh_ModSettingsChanged() {}
__attribute__((weak)) BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_ModSettingsChanged();
    *bReload = FALSE;
    return TRUE;
}

namespace {

BOOL Init() {
    return Wh_ModInit();
}
void AfterInit() {
    Wh_ModAfterInit();
}
void BeforeUninit() {
    Wh_ModBeforeUninit();
}
void Uninit() {
    Wh_ModUninit();
}
BOOL SettingsChanged(BOOL* reload) {
    return Wh_ModSettingsChanged(reload);
}

const PillAddonCallbacks kCallbacks = {
    sizeof(PillAddonCallbacks),
    PILL_ADDON_API_VERSION,
    WH_MOD_ID,
    WH_MOD_VERSION,
    Init,
    AfterInit,
    BeforeUninit,
    Uninit,
    SettingsChanged,
    kSettingDefaults,
    sizeof(kSettingDefaults) / sizeof(kSettingDefaults[0]),
};

}  // namespace

extern "C" __declspec(dllexport) const PillAddonCallbacks* PillAddonGetCallbacks() {
    return &kCallbacks;
}
