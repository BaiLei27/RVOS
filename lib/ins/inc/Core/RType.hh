#pragma once
#include <array>

#include "IBaseInstType.hh"

class RType: public IBaseInstType {
public:
    // RV32I R-type (OP opcode 0x33): functKey = (funct7 << 3) | funct3
    // Ref: https://gitlab.com/AlundorZhu/rvcodecjs/-/blob/main/core/Constants.js
    constexpr static std::array<InstInfo, 10> G_INST_TABLE= {
        { { .name_="add",  .XLEN_="RV32I", .funct_=0x0000, .opcode_=0x33 },  // funct7=0, funct3=0
          { .name_="sub",  .XLEN_="RV32I", .funct_=0x0100, .opcode_=0x33 },  // funct7=0x20, funct3=0
          { .name_="sll",  .XLEN_="RV32I", .funct_=0x0001, .opcode_=0x33 },  // funct7=0, funct3=1
          { .name_="slt",  .XLEN_="RV32I", .funct_=0x0002, .opcode_=0x33 },  // funct7=0, funct3=2
          { .name_="sltu", .XLEN_="RV32I", .funct_=0x0003, .opcode_=0x33 },  // funct7=0, funct3=3
          { .name_="xor",  .XLEN_="RV32I", .funct_=0x0004, .opcode_=0x33 },  // funct7=0, funct3=4
          { .name_="srl",  .XLEN_="RV32I", .funct_=0x0005, .opcode_=0x33 },  // funct7=0, funct3=5
          { .name_="sra",  .XLEN_="RV32I", .funct_=0x0105, .opcode_=0x33 },  // funct7=0x20, funct3=5
          { .name_="or",   .XLEN_="RV32I", .funct_=0x0006, .opcode_=0x33 },  // funct7=0, funct3=6
          { .name_="and",  .XLEN_="RV32I", .funct_=0x0007, .opcode_=0x33 } } // funct7=0, funct3=7
    };

public:
    // using IBaseInstType::IBaseInstType;
    RType(uint32_t inst, InstFormat format, bool hasSetABI= false);
    // RType(uint16_t opcode, InstFormat format, bool hasSetABI= false);
    RType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI= false);

public:
    void Parse() override;

    [[nodiscard]] const std::vector<std::string> &Disassembly() override;
    [[nodiscard]] const InstLayout &Assembly() override;

private:
    KeyT calculateFunctKey() override;
    void mnemonicHelper() override;
    [[nodiscard]] pBiTable_u buildTable() override;
};

// Date:25/12/22/23:52
