#pragma once
#include <array>

#include "IBaseInstType.hh"

class RType: public IBaseInstType {
public:
    constexpr static std::array<InstInfo, 32> G_INST_TABLE= {
        { { .name_= "add", .XLEN_= "RV32I", .funct_= 0x0000, .opcode_= 0x33 },
         { .name_= "sub", .XLEN_= "RV32I", .funct_= 0x0100, .opcode_= 0x33 } }
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
