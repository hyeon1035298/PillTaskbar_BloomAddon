# PillTaskbar

PillTaskbar turns the Windows 11 taskbar into a floating, pill-shaped bar in the middle of the screen. It also adds taskbar folders, a usage meter, and options for icon size, auto hide, and the system tray.

It comes in two forms from one source file.

- A Windhawk mod. Copy `pill-taskbar.wh.cpp` into Windhawk and compile it there.
- A Bloom add-on. The same mod runs without Windhawk. A small host program injects the mod into the taskbar process and keeps it running. This repository is the source of that add-on.

## How the add-on finds symbols

The mod hooks internal functions of the taskbar DLLs, so it needs their symbol files. On first run the host downloads the matching Microsoft symbol files for those DLLs and keeps the resulting offsets in a local cache. Nothing from Microsoft is bundled in this repository or in the release zip. Without an internet connection the first start cannot finish.

## Build from source

You need a clang that targets `x86_64-w64-mingw32`. Either llvm-mingw or the compiler that ships with Windhawk works. You also need Python 3 and a bash shell such as Git Bash.

```
export CXX=/path/to/llvm-mingw/bin/clang++.exe   # optional, defaults to Windhawk's compiler
bash tools/build-addon.sh
```

The result lands in `build/addon`. The C++ runtime is linked in statically, so no extra DLLs ship next to the binaries.

Before you commit, run `python tools/check-public.py`. It looks for personal paths, e-mail addresses, debug symbols, and other files that must not be published.

## License

PillTaskbar is free software under the GNU General Public License, version 3 only. See `LICENSE`.

Parts of the mod come from other Windhawk mods, and the add-on bundles MinHook (BSD-2-Clause) and Zydis (MIT). Their credits and licenses are in `THIRD_PARTY.md` and `THIRD_PARTY_NOTICES.txt`.

This program comes with no warranty. It patches code inside explorer.exe, so use it at your own risk. Windows and Windhawk are trademarks of their owners. This project is not affiliated with Microsoft or with the Windhawk authors.
