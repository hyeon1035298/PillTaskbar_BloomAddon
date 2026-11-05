// SPDX-License-Identifier: GPL-3.0-only
//
// Wh_Disasm with Zydis' Intel formatter, the one Windhawk uses: mods match the text
// with regular expressions, so another disassembler's spelling would silently miss.

#include "internal.h"

#include <Zydis.h>

#include <cstring>

extern "C" BOOL PillRt_Disasm(void* address, WH_DISASM_RESULT* result) {
    ZydisDisassembledInstruction instruction;
    ZyanStatus status = ZydisDisassembleIntel(ZYDIS_MACHINE_MODE_LONG_64, reinterpret_cast<ZyanU64>(address), address,
                                              ZYDIS_MAX_INSTRUCTION_LENGTH, &instruction);
    if (!ZYAN_SUCCESS(status)) {
        rt::log::Line(L"Zydis could not decode %p (%08X)", address, status);
        return FALSE;
    }
    result->length = instruction.info.length;
    static_assert(sizeof(result->text) <= sizeof(instruction.text));
    std::strncpy(result->text, instruction.text, sizeof(result->text) - 1);
    result->text[sizeof(result->text) - 1] = '\0';
    return TRUE;
}
