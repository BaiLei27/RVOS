
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

const std::vector<std::string> &RType::Disassembly()
{
    if(!InstTable_) {
        InstTable_= buildTable();
    }

    if(InstAssembly_.empty()) {
        const auto &[instName, _1, _2]= LookupNameAndInfo();
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

const InstLayout &RType::Assembly()
{
    // Layout_= 0x124;

    return Layout_;
}

IBaseInstType::KeyT RType::calculateFunctKey()
{
    FunctKey_= Layout_.R.fct7 << 3 | Layout_.R.fct3;
    return FunctKey_;
}

IBaseInstType::pTable_u RType::buildTable()
{
    // auto [VALID_ENTRIES, _]= FILTER_VALID_ENTRIES(G_INST_TABLE);

    static auto s_instTable= [&]() -> pTable_u {
        BiLookupTable<KeyT>::intMapTup_u code2info;
        BiLookupTable<KeyT>::strMapTup_u name2info;
        // std::cout << G_INST_TABLE.size();
        for(const auto &entry: G_INST_TABLE) {
            FunctKey_= std::get<0>(entry);
            Opcode_  = std::get<1>(entry);
            auto name= std::get<2>(entry);
            auto xlen= std::get<3>(entry);

            if(Opcode_ == 0x0 || xlen.empty() || name.empty()) {
                continue;
            }

            auto manualURL= BaseURL_ + std::string(name); // temp string

            // std::cout << "opcode: 0x" << std::hex << Opcode_ << '\n'
            //           << "name: " << name << '\n'
            //           << "xlen: " << xlen << '\n'
            //           << "functKey: 0x" << FunctKey_ << '\n'
            //           << "BaseURL: " << BaseURL_ << '\n';

            code2info.emplace(FunctKey_, std::make_tuple(name, xlen, manualURL));
            name2info.emplace(name, std::make_tuple(FunctKey_, Opcode_, xlen, manualURL));
        }

        return std::make_shared<const BiLookupTable<KeyT>>(
            std::move(code2info),
            std::move(name2info));
    }();

    return s_instTable;
}
