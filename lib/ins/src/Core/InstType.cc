
#include <iostream>

#include "Core/InstType.hh"
#include "ISA/Regs.hpp"

// #include "Log/Logger.hpp"

RType::RType(uint32_t inst, InstFormat format, bool hasSetABI)
    : IBaseInstType(inst, format, hasSetABI)
{
    init();
}

RType::RType(KeyT opcode, InstFormat format, bool hasSetABI)
    : IBaseInstType(opcode, format, hasSetABI)
{
    // FunctKey_     = Layout_.R.fct7 << 3 | Layout_.R.fct3;
    // instTable_= BuildTable();
    init();
}

void RType::Parse()
{
    std::cout << "opcode: 0x" << std::hex << GetInstOpcode() << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "funct3: " << Layout_.R.fct3 << '\n'
              << "funct7: " << Layout_.R.fct7 << '\n'
              << "rs1: " << Layout_.R.rs1 << '\n'
              << "rs2: " << Layout_.R.rs2 << '\n'
              << "rd: " << Layout_.R.rd << '\n';
}

const std::vector<std::string_view> &RType::Disassembly()
{
    if(!InstTable_) {
        InstTable_= buildTable();
    }

    if(InstAssembly_.empty()) {
        const auto &[instName, _]= LookupNameAndXLEN();
        InstAssembly_.emplace_back(instName);

        if(HasSetABI_) {
            InstAssembly_.emplace_back(G_INDEX_REGS_ABI[Layout_.R.rd].second);
            InstAssembly_.emplace_back(G_INDEX_REGS_ABI[Layout_.R.rs1].second);
            InstAssembly_.emplace_back(G_INDEX_REGS_ABI[Layout_.R.rs2].second);
        } else {
            InstAssembly_.emplace_back(G_INDEX_REGS_ABI[Layout_.R.rd].first);
            InstAssembly_.emplace_back(G_INDEX_REGS_ABI[Layout_.R.rs1].first);
            InstAssembly_.emplace_back(G_INDEX_REGS_ABI[Layout_.R.rs2].first);
        }
    }

    return InstAssembly_;
}

void RType::Assembly() { }

IBaseInstType::KeyT RType::calculateFunctKey()
{
    FunctKey_= Layout_.R.fct7 << 3 | Layout_.R.fct3;
    return FunctKey_;
}

IBaseInstType::pTable_u RType::buildTable()
{
    static auto s_instTable= std::make_shared<const BiLookupTable<KeyT>>(
        BiLookupTable<KeyT>::intMapTup_u {
            { 0x00, { "add", "RV32I" } },
    },
        BiLookupTable<KeyT>::strMapTup_u {
            { "add", { 0x00, "RV32I" } },

        });

    return s_instTable;
}
