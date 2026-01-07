#pragma once
#include <array>

#include "IBaseInstType.hh"

class RType: public IBaseInstType {
public:
    constexpr static std::array<infoTup_u, 32> G_INST_TABLE= {
        {
         { 0x0000, 0x33, "add", "RV32I" },
         { 0x0100, 0x33, "sub", "RV32I" },
         }
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
    [[nodiscard]] pTable_u buildTable() override;
};

// Date:25/12/22/23:52
