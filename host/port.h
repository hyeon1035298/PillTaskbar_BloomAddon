// SPDX-License-Identifier: GPL-3.0-only
//
// Clean-room symbol porting between two builds of one module. When Microsoft has not
// published the PDB of a NEW build yet, port::Run finds the wanted names of an OLD build,
// whose PDB is public, in the NEW build from the two binaries alone (port_engine.cpp).
// port::ResolveAll lists the OLD PDB's publics with the names the host gives them
// (port_pdb.cpp). The engine answers a name only when independent evidence agrees; any
// doubt leaves it unported, since a wrong address hooks the wrong code in explorer.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace port {

// Stored with every .ported sidecar and every recorded port failure. Raise it whenever a
// change to the engine can change what it answers.
inline constexpr int kEngineVersion = 1;

// One public of the OLD PDB, code or data, named as the host undecorates it.
struct OldSymbol {
    std::wstring name;
    uint32_t rva = 0;
};

// A name to port, in symbols.txt order; required comes from symbols-meta.txt.
struct Want {
    std::wstring name;
    bool required = false;
};

// Absent: the OLD PDB does not have the name at all.
enum class Outcome { Ported, Absent, Ambiguous, NotFound };

struct Result {
    std::wstring name;
    Outcome outcome = Outcome::NotFound;
    uint32_t rva = 0;
    std::wstring evidence;
};

struct Input {
    std::wstring oldDll, newDll;
    std::vector<OldSymbol> oldSymbols;
    std::vector<Want> wanted;
};

struct Stats {
    uint32_t functionsOld = 0, functionsNew = 0;
    uint64_t ms = 0;
    uint64_t analyzeMs = 0, engineMs = 0;
};

// One Result per wanted name, in order. False only on an input error (an unreadable PE),
// with error set.
bool Run(const Input& in, std::vector<Result>& out, Stats& stats, std::wstring& error);

// Every public of a PDB through System32's dbghelp, with the host's options and
// undecoration (symbols.cpp Resolve), in enumeration order. A name the PDB has at several
// addresses comes once per distinct address, the host's address (the first) first.
bool ResolveAll(const std::wstring& pdbPath, const std::wstring& dllPath,
                std::vector<OldSymbol>& out, std::wstring& error);

}  // namespace port
