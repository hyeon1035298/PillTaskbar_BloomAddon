// host/port.h — clean-room symbol porting between two builds of one module.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace port {
inline constexpr int kEngineVersion = 1;
struct OldSymbol { std::wstring name; uint32_t rva = 0; };   // every public of the OLD PDB (code and data), host undecoration
struct Want { std::wstring name; bool required = false; };   // symbols.txt order; required from symbols-meta.txt
enum class Outcome { Ported, Absent, Ambiguous, NotFound };  // Absent: not in the OLD PDB at all
struct Result { std::wstring name; Outcome outcome = Outcome::NotFound; uint32_t rva = 0; std::wstring evidence; };
struct Input { std::wstring oldDll, newDll; std::vector<OldSymbol> oldSymbols; std::vector<Want> wanted; };
struct Stats { uint32_t functionsOld = 0, functionsNew = 0; uint64_t ms = 0; uint64_t analyzeMs = 0, engineMs = 0; };
// One Result per wanted name, in order. False only on an input error (unreadable PE...), with error set.
bool Run(const Input& in, std::vector<Result>& out, Stats& stats, std::wstring& error);
// Every public of a PDB via System32 dbghelp, with exactly the host's options and undecoration (host/symbols.cpp Resolve).
bool ResolveAll(const std::wstring& pdbPath, const std::wstring& dllPath, std::vector<OldSymbol>& out, std::wstring& error);
}
