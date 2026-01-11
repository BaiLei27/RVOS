#pragma once
#include <array>

#include "IBaseInstType.hh"

class IType: public IBaseInstType {
public:
    constexpr static std::array<infoTup_u, 32> G_INST_TABLE= {
        {
            { 0x0100, 0x33, "sub", "RV32I" },
            { 0x0105, 0x33, "sra", "RV32I" },
            { 0x0000, 0x33, "add", "RV32I" },
            { 0x0001, 0x33, "sll", "RV32I" },
            { 0x0002, 0x33, "slt", "RV32I" },
        }
    };

public:
    // using IBaseInstType::IBaseInstType;
    IType(uint32_t inst, InstFormat format, bool hasSetABI= false);
    // IType(uint16_t opcode, InstFormat format, bool hasSetABI= false);
    IType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI= false);

public:
    void Parse() override;

    [[nodiscard]] const std::vector<std::string> &Disassembly() override;
    [[nodiscard]] const InstLayout &Assembly() override;

private:
    KeyT calculateFunctKey() override;
    void mnemonicHelper() override;
    [[nodiscard]] pTable_u buildTable() override;
};

// Date:25/12/22/23:52
