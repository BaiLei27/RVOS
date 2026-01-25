
#include <iostream>

#include "Core/RType.hh"
#include "ISA/Regs.hpp"

// #include "Log/Logger.hpp"

RType::RType(uint32_t inst, InstFormat format, bool hasSetABI)
    : IBaseInstType(inst, format, hasSetABI)
{
    init();
}

// RType::RType(KeyT opcode, InstFormat format, bool hasSetABI)
//     : IBaseInstType(opcode, format, hasSetABI)
// {
//     // FunctKey_     = Layout_.R.fct7 << 3 | Layout_.R.fct3;
//     // instTable_= BuildTable();
//     init();
// }

RType::RType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI)
    : IBaseInstType(std::move(instAssembly), format, hasSetABI)
{
    init();
}

void RType::Parse()
{
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.R.opc));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.R.rd));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.R.fct3));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.R.rs1));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.R.rs2));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.R.fct7));

    std::cout << "opcode: 0x" << std::hex << Opcode_ << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "funct3: " << Layout_.R.fct3 << '\n'
              << "funct7: " << Layout_.R.fct7 << '\n'
              << "rs1: " << Layout_.R.rs1 << '\n'
              << "rs2: " << Layout_.R.rs2 << '\n'
              << "rd: " << Layout_.R.rd << '\n';
}

void RType::mnemonicHelper()
{
    auto rd = isa::LOOKUP_REG_NAME(Layout_.R.rd, HasSetABI_); // actually reg mnemonic only 5b (max: 31),never overflow
    auto rs1= isa::LOOKUP_REG_NAME(Layout_.R.rs1, HasSetABI_);
    auto rs2= isa::LOOKUP_REG_NAME(Layout_.R.rs2, HasSetABI_);

    appendOperands({ rd, rs1, rs2 });
}

const std::vector<std::string> &RType::Disassembly()
{
    if(!InstTable_) {
        InstTable_= buildTable();
    }

    if(InstAssembly_.empty()) {
        const auto &[_1, _2, instName]= LookupNameAndInfo();
        InstAssembly_.emplace_back(instName);
        mnemonicHelper();
    }

    return InstAssembly_;
}

const InstLayout &RType::Assembly()
{
    const auto &[_1, _2, functKey, opc]= LookupIdxAndInfo();

    Layout_.R.opc= Opcode_= opc;
    Layout_.R.fct7        = functKey >> 3;
    Layout_.R.fct3        = functKey & 7;

    if(!InstAssembly_.empty()) {
        Layout_.R.rd = *isa::LOOKUP_REG_IDX(InstAssembly_.at(1));
        Layout_.R.rs1= *isa::LOOKUP_REG_IDX(InstAssembly_.at(2));
        Layout_.R.rs2= *isa::LOOKUP_REG_IDX(InstAssembly_.at(3));
    }

    mnemonicHelper();

    return Layout_;
}

IBaseInstType::KeyT RType::calculateFunctKey()
{
    FunctKey_= Layout_.R.fct7 << 3 | Layout_.R.fct3;
    return FunctKey_;
}

IBaseInstType::pBiTable_u RType::buildTable()
{
    static auto s_instTable= [](const std::string &baseURL) -> pBiTable_u {
        BiLookupTable<KeyT>::intMapName_u code2info;
        BiLookupTable<KeyT>::strMapIndex_u name2info;

        for(const auto &entry: G_INST_TABLE) {
            // LOG_DEBUG("DEBUG");
            if(0x00 == entry.opcode_ || entry.XLEN_.empty() || entry.name_.empty()) {
                continue;
            }
            // LOG_INFO("INFO");

            auto manualURL= baseURL + std::string(entry.name_); // temp string

            // std::cout << "opcode: 0x" << std::hex << entry.opcode_ << '\n'
            //           << "functKey: 0x" << entry.funct_ << '\n'
            //           << "name: " << entry.name_ << '\n'
            //           << "XLEN: " << entry.XLEN_ << '\n'
            //           << "BaseURL: " << baseURL << '\n';

            code2info.emplace(entry.funct_,
                              BiLookupTable<KeyT>::NameInfo { .manual_= manualURL,
                                                              .XLEN_  = entry.XLEN_,
                                                              .name_  = entry.name_ });
            name2info.emplace(entry.name_,
                              BiLookupTable<KeyT>::IndexInfo { .manual_= manualURL,
                                                               .XLEN_  = entry.XLEN_,
                                                               .funct_ = entry.funct_,
                                                               .opcode_= entry.opcode_ });
        }

        return std::make_shared<const BiLookupTable<KeyT>>(std::move(code2info), std::move(name2info));
    }(BaseURL_);

    return s_instTable;
}
