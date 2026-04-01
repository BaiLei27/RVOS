#pragma once
#include <array>

#include "IBaseInstType.hh"

class JType: public IBaseInstType {
public:
    constexpr static std::array<InstInfo, 1> G_INST_TABLE= {
        { { .name_="jal", .XLEN_="RV32I", .funct_=0x0000, .opcode_=0x6F } },
    };

public:
    JType(uint32_t inst, InstFormat format, bool hasSetABI= false);
    JType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI= false);

public:
    void Parse() override;

    [[nodiscard]] const std::vector<std::string> &Disassembly() override;
    [[nodiscard]] const InstLayout &Assembly() override;

private:
    KeyT calculateFunctKey() override;
    void mnemonicHelper() override;
    [[nodiscard]] pBiTable_u buildTable() override;
};
