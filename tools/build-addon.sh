#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# Builds a Bloom add-on with the compiler Windhawk ships and Windhawk's mod flags. The
# add-on's names come from its addon.conf (ADDON_CONF, default addon/addon.conf, the
# PillTaskbar one); <name> below is its ADDON_NAME:
#   <name>Runtime.dll  the Windhawk API subset, MinHook, Zydis
#   <name>.dll         the mod source unchanged, plus addon/addon_entry.cpp
#   <name>Host.exe     symbols, injection, explorer watch
#   symbols.txt, addon.json, THIRD_PARTY.md (and settings.schema.json with ADDON_SCHEMA=1)
# Usage: tools/build-addon.sh [all|runtime|mod|host|bundle]...  (default all)
# OUT_DIR (default build/addon), MOD_SRC (default the conf's), DEBUG=1 keeps symbols in the binaries.
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
cxx="${CXX:-/c/Program Files/Windhawk/Compiler/bin/clang++.exe}"
out="${OUT_DIR:-$root/build/addon}"
obj="$out/obj"
gen="$out/gen"
mkdir -p "$out" "$obj" "$gen"

conf="${ADDON_CONF:-$root/addon/addon.conf}"
conf_dir="$(cd "$(dirname "$conf")" && pwd)"
python -I "$root/tools/make-identity.py" "$conf" "$gen"
# shellcheck disable=SC1090
source "$conf"
name="$ADDON_NAME"
mod_src="$conf_dir/$ADDON_MOD_SRC"

strip=(-s)
[[ "${DEBUG:-0}" == 1 ]] && strip=(-g)

defs=(-DUNICODE -D_UNICODE -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -D_WIN32_IE=0x0A00
      -DNTDDI_VERSION=0x0A000008 -D__USE_MINGW_ANSI_STDIO=0)
flags=(-target x86_64-w64-mingw32 -O2 -ffp-exception-behavior=maytrap -mguard=cf-nochecks "${defs[@]}")
cxxflags=("${flags[@]}" -std=c++23)
# The C++ runtime goes into each binary, so nothing like libc++.whl ships next to them.
static=(-static)

# Compiles one C file to obj/ unless the object is newer.
c_obj() {
    local src="$1" o="$obj/$2.o"
    shift 2
    if [[ ! -f "$o" || "$src" -nt "$o" ]]; then
        "$cxx" "${flags[@]}" -x c -c "$src" -o "$o" "$@"
    fi
    echo "$o"
}

build_runtime() {
    local mh="$root/third_party/minhook" zy="$root/third_party/zydis" objs=()
    for f in buffer hook trampoline; do
        objs+=("$(c_obj "$mh/src/$f.c" "mh_$f" -I"$mh/include")")
    done
    objs+=("$(c_obj "$mh/src/hde/hde64.c" mh_hde64 -I"$mh/include")")
    objs+=("$(c_obj "$zy/Zydis.c" zydis -I"$zy" -DZYDIS_STATIC_BUILD -DZYCORE_STATIC_BUILD)")
    "$cxx" "${cxxflags[@]}" -shared "${static[@]}" "${strip[@]}" \
        -I"$root/runtime/include" -I"$root/common" -I"$gen" -I"$mh/include" -I"$zy" \
        -DZYDIS_STATIC_BUILD -DZYCORE_STATIC_BUILD \
        "$root"/runtime/*.cpp "${objs[@]}" \
        -Wl,--out-implib,"$obj/lib${name}Runtime.dll.a" \
        -o "$out/${name}Runtime.dll" -luser32 -ladvapi32 -lshlwapi
    echo "built $out/${name}Runtime.dll"
}

build_mod() {
    local src="${MOD_SRC:-$mod_src}" values=()
    [[ -n "${ADDON_VALUES_DEFAULTS:-}" ]] && values=(--values "$conf_dir/$ADDON_VALUES_DEFAULTS")
    python -I "$root/tools/make-schema.py" --defaults "$gen/settings_defaults.h" "${values[@]}" "$src"
    # The libraries Windhawk links, from the mod's @compilerOptions line.
    local libs
    libs=$(sed -n 's#^// @compilerOptions *##p' "$src")
    # shellcheck disable=SC2086
    "$cxx" "${cxxflags[@]}" -shared "${static[@]}" "${strip[@]}" -DWH_MOD \
        -DWH_MOD_ID="L\"$ADDON_ID\"" -DWH_MOD_VERSION="L\"${ADDON_MOD_VERSION:-0.1}\"" \
        -I"$root/runtime/include" -I"$gen" -include "$root/runtime/include/windhawk_api.h" \
        -x c++ "$src" "$root/addon/addon_entry.cpp" \
        -x none "$obj/lib${name}Runtime.dll.a" -o "$out/${name}.dll" $libs
    echo "built $out/${name}.dll"
}

build_host() {
    # dbghelp is not linked: the host loads the System32 copy at run time.
    "$cxx" "${cxxflags[@]}" -Wall -Wextra -municode -mwindows "${static[@]}" "${strip[@]}" \
        -I"$root/common" -I"$gen" "$root"/host/*.cpp -o "$out/${name}Host.exe" \
        -lwinhttp -lwevtapi -lpsapi -ladvapi32 -lshell32 -luser32
    echo "built $out/${name}Host.exe"
}

build_bundle() {
    python -I "$root/tools/symbol-list.py" "$mod_src" "$out/symbols.txt"
    cp "$conf_dir/$ADDON_THIRD_PARTY" "$out/THIRD_PARTY.md"
    local tp="$root/third_party"
    {
        for f in "$tp/minhook/LICENSE.txt" "$tp/zydis/LICENSE" "$tp/zydis/LICENSE.zycore" \
                 "$tp/notices/COPYING.MinGW-w64-runtime.txt"; do
            printf '==== %s ====\n\n' "${f#$tp/}"
            cat "$f"
            printf '\n\n'
        done
    } > "$out/THIRD_PARTY_NOTICES.txt"
    if [[ "${ADDON_SCHEMA:-0}" == 1 ]]; then
        python -I "$root/tools/make-schema.py" --schema "$out/settings.schema.json" "$mod_src"
    fi
    python -I "$root/tools/make-manifest.py" "$out" "$gen/identity.json"
    echo "bundle in $out"
}

# Test-only binaries, kept out of the bundle: the crash mod for the safe-mode test and
# the offline Wh_Disasm check. Needs the runtime import library (target runtime).
build_tests() {
    local t="$out/../test"
    mkdir -p "$t"
    "$cxx" "${cxxflags[@]}" -shared "${static[@]}" -I"$root/runtime/include" \
        "$root/tools/crash-test-mod.cpp" -o "$t/CrashTest.dll"
    "$cxx" "${cxxflags[@]}" -municode "${static[@]}" -I"$root/runtime/include" \
        "$root/tools/disasm-check.cpp" -x none "$obj/lib${name}Runtime.dll.a" -o "$t/disasm-check.exe"
    # The stand-in mod and the harness that plays host and explorer for it.
    mkdir -p "$t/gen"
    cat > "$t/gen/settings_defaults.h" <<'EOF2'
#pragma once
static const PillAddonDefault kSettingDefaults[] = {
    {L"test.number", 1, nullptr},
    {L"test.default", 42, nullptr},
    {L"test.negative", -1, nullptr},
    {L"test.reload", 0, nullptr},
    {L"test.choice", 0, L"glass"},
};
EOF2
    "$cxx" "${cxxflags[@]}" -shared "${static[@]}" -DWH_MOD -DWH_MOD_ID='L"harness"' \
        -I"$root/runtime/include" -I"$t/gen" -I"$root/tools" -include "$root/runtime/include/windhawk_api.h" \
        "$root/tools/harness-mod.cpp" "$root/addon/addon_entry.cpp" \
        -x none "$obj/lib${name}Runtime.dll.a" -o "$t/HarnessMod.dll" -lcomctl32
    "$cxx" "${cxxflags[@]}" -municode "${static[@]}" -g -gcodeview -Wl,--pdb="$t/runtime-harness.pdb" \
        -I"$root/common" -I"$gen" -I"$root/tools" "$root/tools/runtime-harness.cpp" -o "$t/runtime-harness.exe"
    echo "test binaries in $t"
}

targets=("$@")
[[ ${#targets[@]} -eq 0 ]] && targets=(all)
for t in "${targets[@]}"; do
    case "$t" in
        all) build_runtime; build_mod; build_host; build_bundle ;;
        runtime) build_runtime ;;
        mod) build_mod ;;
        host) build_host ;;
        bundle) build_bundle ;;
        tests) build_tests ;;
        *) echo "unknown target $t" >&2; exit 2 ;;
    esac
done
