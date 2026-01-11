
#include <iostream>

#include "Core/IType.hh"
#include "ISA/Regs.hpp"

// #include "Log/Logger.hpp"

IType::IType(uint32_t inst, InstFormat format, bool hasSetABI)
    : IBaseInstType(inst, format, hasSetABI)
{
    init();
}

// IType::IType(KeyT opcode, InstFormat format, bool hasSetABI)
//     : IBaseInstType(opcode, format, hasSetABI)
// {
//     // FunctKey_     = Layout_.I.fct7 << 3 | Layout_.I.fct3;
//     // instTable_= BuildTable();
//     init();
// }

IType::IType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI)
    : IBaseInstType(std::move(instAssembly), format, hasSetABI)
{
    init();
}

void IType::Parse()
{
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.I.opc));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.I.rd));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.I.fct3));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.I.rs1));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.I.rs2));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.I.fct7));
    std::cout << "opcode: 0x" << std::hex << GetInstOpcode() << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "funct3: " << Layout_.I.fct3 << '\n'
              << "funct7: " << Layout_.I.fct7 << '\n'
              << "rs1: " << Layout_.I.rs1 << '\n'
              << "rs2: " << Layout_.I.rs2 << '\n'
              << "rd: " << Layout_.I.rd << '\n';
}

void IType::mnemonicHelper()
{
    auto rd = isa::LOOKUP_REG_NAME(Layout_.I.rd, HasSetABI_); // actually reg mnemonic only 5b (max: 31),never overflow
    auto rs1= isa::LOOKUP_REG_NAME(Layout_.I.rs1, HasSetABI_);
    auto rs2= isa::LOOKUP_REG_NAME(Layout_.I.rs2, HasSetABI_);

    appendOperands({ " ", rd, ", ", rs1, ", ", rs2 });
}

const std::vector<std::string> &IType::Disassembly()
{
    if(!InstTable_) {
        InstTable_= buildTable();
    }

    if(InstAssembly_.empty()) {
        const auto &[instName, _1, _2]= LookupNameAndInfo();
        InstAssembly_.emplace_back(instName);
        mnemonicHelper();
    }

    return InstAssembly_;
}

const InstLayout &IType::Assembly()
{
    const auto &[functKey, _1, _2]= LookupIdxAndInfo();

    Layout_.I.opc = Opcode_;
    Layout_.I.fct7= functKey >> 3;
    Layout_.I.fct3= functKey & 7;

    if(!InstAssembly_.empty()) {
        Layout_.I.rd = *isa::LOOKUP_REG_IDX(InstAssembly_.at(1));
        Layout_.I.rs1= *isa::LOOKUP_REG_IDX(InstAssembly_.at(2));
        Layout_.I.rs2= *isa::LOOKUP_REG_IDX(InstAssembly_.at(3));
    }

    mnemonicHelper();

    return Layout_;
}

IBaseInstType::KeyT IType::calculateFunctKey()
{
    FunctKey_= Layout_.I.fct7 << 3 | Layout_.I.fct3;
    std::cout << "FunctKey: 0x" << std::hex << FunctKey_ << '\n';
    return FunctKey_;
}

IBaseInstType::pTable_u IType::buildTable()
{
    Opcode_= Layout_.entity_ & 0x7F;

    static auto s_instTable= [](const std::string &baseURL) -> pTable_u {
        BiLookupTable<KeyT>::intMapTup_u code2info;
        BiLookupTable<KeyT>::strMapTup_u name2info;
        for(const auto &entry: G_INST_TABLE) {
            auto functKey= std::get<0>(entry);
            auto name    = std::get<2>(entry);
            auto xlen    = std::get<3>(entry);

            if(xlen.empty() || name.empty()) {
                continue;
            }

            auto manualURL= baseURL + std::string(name); // temp string

            // std::cout << "opcode: 0x" << std::hex << Opcode_ << '\n'
            //           << "name: " << name << '\n'
            //           << "xlen: " << xlen << '\n'
            //           << "functKey: 0x" << functKey << '\n'
            //           << "BaseURL: " << BaseURL_ << '\n';

            code2info.emplace(functKey, std::make_tuple(name, xlen, manualURL));
            name2info.emplace(name, std::make_tuple(functKey, xlen, manualURL));
        }

        return std::make_shared<const BiLookupTable<KeyT>>(
            std::move(code2info),
            std::move(name2info));
    }(BaseURL_);

    return s_instTable;
}
