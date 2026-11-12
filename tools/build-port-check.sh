#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# Builds tools/port-check.exe, the offline harness for the symbol porter, with the host's
# compiler and flags plus Zydis. Dev only; it is not part of the bundle.
# OUT_DIR (default build/port-check).
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
cxx="${CXX:-/c/Program Files/Windhawk/Compiler/bin/clang++.exe}"
out="${OUT_DIR:-$root/build/port-check}"
gen="$out/gen"
zy="$root/third_party/zydis"
mkdir -p "$out" "$gen"
python -I "$root/tools/make-identity.py" "$root/addon/addon.conf" "$gen"

defs=(-DUNICODE -D_UNICODE -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -D_WIN32_IE=0x0A00
      -DNTDDI_VERSION=0x0A000008 -D__USE_MINGW_ANSI_STDIO=0)
flags=(-target x86_64-w64-mingw32 -O2 -ffp-exception-behavior=maytrap -mguard=cf-nochecks "${defs[@]}")

zobj="$out/zydis.o"
if [[ ! -f "$zobj" || "$zy/Zydis.c" -nt "$zobj" ]]; then
    "$cxx" "${flags[@]}" -x c -c "$zy/Zydis.c" -o "$zobj" -I"$zy" -DZYDIS_STATIC_BUILD -DZYCORE_STATIC_BUILD
fi
"$cxx" "${flags[@]}" -std=c++23 -Wall -Wextra -municode -static -s \
    -I"$root/host" -I"$root/common" -I"$gen" -I"$zy" -DZYDIS_STATIC_BUILD -DZYCORE_STATIC_BUILD \
    "$root/tools/port-check.cpp" "$root/host/pe.cpp" "$root/host/port_pdb.cpp" "$root/host/port_engine.cpp" \
    "$root/host/symbols.cpp" "$root/host/util.cpp" -x none "$zobj" \
    -o "$out/port-check.exe" -lwinhttp
echo "built $out/port-check.exe"
