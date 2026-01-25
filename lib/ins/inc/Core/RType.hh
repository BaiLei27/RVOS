#pragma once
#include <array>

#include "IBaseInstType.hh"

class RType: public IBaseInstType {
public:
    constexpr static std::array<infoTup_u, 32> G_INST_TABLE= {
        {
         { 0x0100, 0x33, "sub", "RV32I" },
         { 0x0105, 0x33, "sra", "RV32I" },
         { 0x0000, 0x33, "add", "RV32I" },
         { 0x0001, 0x33, "sll", "RV32I" },
         { 0x0002, 0x33, "slt", "RV32I" },
         { 0x0003, 0x33, "sltu", "RV32I" },
         { 0x0004, 0x33, "xor", "RV32I" },
         { 0x0005, 0x33, "sra", "RV32I" },
         { 0x0006, 0x33, "or", "RV32I" },
         { 0x0007, 0x33, "and", "RV32I" },
         { 0x0008, 0x33, "mul", "RV32M" },
         { 0x0009, 0x33, "mulh", "RV32M" },
         { 0x000a, 0x33, "mulhsu", "RV32M" },
         { 0x000b, 0x33, "mulhu", "RV32M" },
         { 0x000c, 0x33, "div", "RV32M" },
         { 0x000d, 0x33, "divu", "RV32M" },
         { 0x000e, 0x33, "rem", "RV32M" },
         { 0x000f, 0x33, "remu", "RV32M" },
         { 0x0008, 0x3b, "mulw", "RV64M" },
         { 0x000c, 0x3b, "divw", "RV64M" },
         { 0x000d, 0x3b, "divuw", "RV64M" },
         { 0x000e, 0x3b, "remw", "RV64M" },
         { 0x000f, 0x3b, "remuw", "RV64M" },
         { 0x0008, 0x7b, "muld", "RV128M" },
         { 0x000c, 0x7b, "divd", "RV128M" },
         { 0x000d, 0x7b, "divud", "RV128M" },
         { 0x000e, 0x7b, "remd", "RV128M" },
         { 0x000f, 0x7b, "remud", "RV128M" },
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
    void mnemonicHelper() override;
    [[nodiscard]] pTable_u buildTable() override;
};

// Date:25/12/22/23:52
