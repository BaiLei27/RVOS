#pragma once
#include <array>

#include "IBaseInstType.hh"

class UType: public IBaseInstType {
public:
    // funct_ is opcode (0x17 / 0x37): U-type has no funct3/funct7; keys must be unique in the table.
    constexpr static std::array<InstInfo, 2> G_INST_TABLE= {
        { { .name_="auipc", .XLEN_="RV32I", .funct_=0x17, .opcode_=0x17 },
          { .name_="lui",   .XLEN_="RV32I", .funct_=0x37, .opcode_=0x37 } }
    };

public:
    UType(uint32_t inst, InstFormat format, bool hasSetABI= false);
    UType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI= false);

public:
    void Parse() override;

    [[nodiscard]] const std::vector<std::string> &Disassembly() override;
    [[nodiscard]] const InstLayout &Assembly() override;

private:
    KeyT calculateFunctKey() override;
    void mnemonicHelper() override;
    [[nodiscard]] pBiTable_u buildTable() override;
};
