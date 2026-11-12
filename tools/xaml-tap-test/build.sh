#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# Builds the XAML Diagnostics test for runtime/include/taskbar_xaml.h into OUT_DIR
# (default build/xaml-tap-test): app.exe hosts system XAML islands in its own windows;
# tapA/B/C.dll stand in for mod DLLs that include the header; tapN.dll connects but
# never advises. Run from OUT_DIR: app.exe [cycles|refs|concurrent|cost|explore|
# explore2|explore3|teardown]. It never touches explorer.
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$here/../.." && pwd)"
cxx="${CXX:-/c/Program Files/Windhawk/Compiler/bin/clang++.exe}"
out="${OUT_DIR:-$root/build/xaml-tap-test}"
mkdir -p "$out/noadvise"
flags=(-target x86_64-w64-mingw32 -O2 -std=c++23 -static -DUNICODE -D_UNICODE -DWINVER=0x0A00
       -D_WIN32_WINNT=0x0A00 -DNTDDI_VERSION=0x0A000008)
libs=(-lole32 -loleaut32 -lruntimeobject -luuid)
"$cxx" "${flags[@]}" -shared -I"$root/runtime/include" "$here/tap_test.cpp" -o "$out/tapA.dll" "${libs[@]}"
cp "$out/tapA.dll" "$out/tapB.dll"
cp "$out/tapA.dll" "$out/tapC.dll"
# The same header with the advise taken out.
python -I - "$root/runtime/include/taskbar_xaml.h" "$out/noadvise/taskbar_xaml.h" <<'PY'
import sys
s = open(sys.argv[1], encoding="utf-8").read()
old = "hr = service->AdviseVisualTreeChange(g_callback);"
assert old in s
open(sys.argv[2], "w", encoding="utf-8", newline="\n").write(s.replace(old, "hr = S_OK;"))
PY
"$cxx" "${flags[@]}" -shared -I"$out/noadvise" "$here/tap_test.cpp" -o "$out/tapN.dll" "${libs[@]}"
"$cxx" "${flags[@]}" -municode "$here/app.cpp" -o "$out/app.exe" "${libs[@]}" -lpsapi -luser32
# Islands need maxversiontested; app.exe has no embedded manifest, so this one is used.
cp "$here/app.exe.manifest" "$out/app.exe.manifest"
echo "built $out"
