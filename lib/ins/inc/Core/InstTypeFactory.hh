#pragma once

#include <string_view>
#include <utility>

#include "Core/IBaseInstType.hh"

class InstTypeFactory {
private:
    const static inline std::unordered_map<uint16_t, InstFormat> G_Opcode2Format= {
        // Opcode -> Instruction format mapping table (RISC-V standard opcode)
        { 0x33, InstFormat::R }, // R-type: add/sub/sll/slt etc.
        { 0x03, InstFormat::I }, // I-type: lb/lh/lw/...
        { 0x13, InstFormat::I }, // I-type: addi/slli/...
        { 0x17, InstFormat::U }, // U-type: auipc
        { 0x37, InstFormat::U }, // U-type: lui
        { 0x67, InstFormat::I }, // I-type: jalr
        { 0x0F, InstFormat::I }, // I-type: fence / fence.i
        { 0x73, InstFormat::I }, // I-type: ecall / ebreak
        { 0x23, InstFormat::S }, // S-type: sw/sb etc.
        { 0x63, InstFormat::B }, // B-type: beq/bne etc.
        { 0x6F, InstFormat::J }  // J-type: jal
    };

    // name -> (format, opcode), O(1) lookup; built from RType::G_INST_TABLE (and future type tables)
    using name2FormatOpcode_u= std::unordered_map<std::string_view, std::pair<InstFormat, uint16_t>>;

public:
    static std::unique_ptr<IBaseInstType> CreateType(uint32_t inst, bool hasSetABI= false);
    static std::unique_ptr<IBaseInstType> CreateType(std::vector<std::string> instAssembly, bool hasSetABI= false);

private:
    template <typename T>
    static std::unique_ptr<IBaseInstType> createHelper(T key, bool hasSetABI= false);
    [[nodiscard]] static std::optional<std::pair<InstFormat, uint16_t>> matchInstName(std::string_view instName);
    static const name2FormatOpcode_u &getName2FormatOpcode();
};
