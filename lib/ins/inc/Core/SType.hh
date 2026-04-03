#pragma once
#include <array>

#include "IBaseInstType.hh"

namespace STypeKey {
// STORE (opcode 0x23): lookup key (opcode<<8)|funct3 (same pattern as IType non-OP-IMM rows).
constexpr uint16_t OPC_F3(uint8_t f3)
{
    return (static_cast<uint16_t>(0x23) << 8) | static_cast<uint16_t>(f3 & 7);
}
} // namespace STypeKey

class SType: public IBaseInstType {
public:
    constexpr static std::array<InstInfo, 4> G_INST_TABLE= {
        { { .name_="sb", .XLEN_="RV32I", .funct_=STypeKey::OPC_F3(0), .opcode_=0x23 },
          { .name_="sh", .XLEN_="RV32I", .funct_=STypeKey::OPC_F3(1), .opcode_=0x23 },
          { .name_="sw", .XLEN_="RV32I", .funct_=STypeKey::OPC_F3(2), .opcode_=0x23 },
          { .name_="sd", .XLEN_="RV64I", .funct_=STypeKey::OPC_F3(3), .opcode_=0x23 } }
    };

public:
    SType(uint32_t inst, InstFormat format, bool hasSetABI= false);
    SType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI= false);

public:
    void Parse() override;

    [[nodiscard]] const std::vector<std::string> &Disassembly() override;
    [[nodiscard]] const InstLayout &Assembly() override;

private:
    KeyT calculateFunctKey() override;
    void mnemonicHelper() override;
    [[nodiscard]] pBiTable_u buildTable() override;
};
