#pragma once

#include <memory>
#include "IBaseInstType.hh"
#include "ISA/InstFormat.hh"

class InstTypeFactory {
private:
    const static inline std::unordered_map<uint16_t, InstFormat> G_Opcode2Format= {
        // Opcode -> Instruction format mapping table (RISC-V standard opcode)
        { 0x33, InstFormat::R }, // R-type: add/sub/sll/slt etc.
        { 0x13, InstFormat::I }, // I-type: lw/addi etc.
        { 0x23, InstFormat::S }, // S-type: sw/sb etc.
        { 0x63, InstFormat::B }, // B-type: beq/bne etc.
        { 0x37, InstFormat::U }, // U-type: lui
        { 0x17, InstFormat::U }, // U-type: auipc
        { 0x6F, InstFormat::J }  // J-type: jal
    };

public:
    static std::unique_ptr<IBaseInstType> CreateType(uint32_t inst, bool hasSetABI= false);

private:
};
