#pragma once
#include <array>

#include "IBaseInstType.hh"

class IType: public IBaseInstType {
public:
    // RV32I I-type OP-IMM (opcode 0x13): functKey = (imm[11:5] << 3) | funct3
    // imm[11:5] acts as funct7 for slli/srli/srai; 0 for others
    constexpr static std::array<InstInfo, 9> G_INST_TABLE= {
        { { .name_="addi",  .XLEN_="RV32I", .funct_=0x0000, .opcode_=0x13 },  // funct3=0
          { .name_="slli",  .XLEN_="RV32I", .funct_=0x0001, .opcode_=0x13 },  // funct3=1, imm[11:5]=0
          { .name_="slti",  .XLEN_="RV32I", .funct_=0x0002, .opcode_=0x13 },  // funct3=2
          { .name_="sltiu", .XLEN_="RV32I", .funct_=0x0003, .opcode_=0x13 },  // funct3=3
          { .name_="xori",  .XLEN_="RV32I", .funct_=0x0004, .opcode_=0x13 },  // funct3=4
          { .name_="srli",  .XLEN_="RV32I", .funct_=0x0005, .opcode_=0x13 },  // funct3=5, imm[11:5]=0
          { .name_="srai",  .XLEN_="RV32I", .funct_=0x0105, .opcode_=0x13 },  // funct3=5, imm[11:5]=0x20
          { .name_="ori",   .XLEN_="RV32I", .funct_=0x0006, .opcode_=0x13 },  // funct3=6
          { .name_="andi",  .XLEN_="RV32I", .funct_=0x0007, .opcode_=0x13 } } // funct3=7
    };

public:
    IType(uint32_t inst, InstFormat format, bool hasSetABI= false);
    IType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI= false);

public:
    void Parse() override;

    [[nodiscard]] const std::vector<std::string> &Disassembly() override;
    [[nodiscard]] const InstLayout &Assembly() override;

private:
    KeyT calculateFunctKey() override;
    void mnemonicHelper() override;
    [[nodiscard]] pBiTable_u buildTable() override;
};
