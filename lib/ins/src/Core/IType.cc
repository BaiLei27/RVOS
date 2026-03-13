#include <iostream>

#include "Core/IType.hh"
#include "ISA/Regs.hpp"

IType::IType(uint32_t inst, InstFormat format, bool hasSetABI)
    : IBaseInstType(inst, format, hasSetABI)
{
    init();
}

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
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.I.imm0tB));

    std::cout << "opcode: 0x" << std::hex << Opcode_ << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "funct3: " << Layout_.I.fct3 << '\n'
              << "rs1: " << Layout_.I.rs1 << '\n'
              << "rd: " << Layout_.I.rd << '\n'
              << "imm: " << std::dec << static_cast<int32_t>(static_cast<int16_t>(Layout_.I.imm0tB & 0xFFF)) << '\n';
}

void IType::mnemonicHelper()
{
    auto rd  = isa::LOOKUP_REG_NAME(Layout_.I.rd, HasSetABI_);
    auto rs1 = isa::LOOKUP_REG_NAME(Layout_.I.rs1, HasSetABI_);
    // Sign-extend 12-bit imm to int32 for display
    int32_t imm   = static_cast<int32_t>(static_cast<int16_t>(Layout_.I.imm0tB & 0xFFF));
    std::string immStr= std::to_string(imm);

    appendOperands({" ", rd, ",", rs1, ",", std::string_view(immStr) });
}

const std::vector<std::string> &IType::Disassembly()
{
    if(!InstTable_) {
        InstTable_= buildTable();
    }

    if(InstAssembly_.empty()) {
        const auto &info= LookupNameAndInfo();
        InstAssembly_.emplace_back(info.name_);
        mnemonicHelper();
    }

    return InstAssembly_;
}

const InstLayout &IType::Assembly()
{
    const auto &info= LookupIdxAndInfo();

    Layout_.I.opc   = Opcode_= info.opcode_;
    Layout_.I.fct3  = info.funct_ & 7;
    Layout_.I.imm0tB= (info.funct_ >> 3) << 5; // upper 7 bits for slli/srli/srai; 0 for others

    if(!InstAssembly_.empty() && InstAssembly_.size() >= 4) {
        Layout_.I.rd  = *isa::LOOKUP_REG_IDX(InstAssembly_.at(1));
        Layout_.I.rs1 = *isa::LOOKUP_REG_IDX(InstAssembly_.at(2));
        int32_t imm   = std::stoi(InstAssembly_.at(3));
        // slli/srli/srai: only lower 5 bits (shift amount); others: full 12-bit imm
        if(Layout_.I.fct3 == 1 || Layout_.I.fct3 == 5) {
            Layout_.I.imm0tB|= (imm & 0x1F);
        } else {
            Layout_.I.imm0tB|= (imm & 0xFFF);
        }
    }

    mnemonicHelper();

    return Layout_;
}

IBaseInstType::KeyT IType::calculateFunctKey()
{
    // I-type: functKey = (imm[11:5] << 3) | funct3
    FunctKey_= (Layout_.I.imm0tB >> 5) << 3 | Layout_.I.fct3;
    return FunctKey_;
}

IBaseInstType::pBiTable_u IType::buildTable()
{
    static auto s_instTable= [](const std::string &baseURL) -> pBiTable_u {
        BiLookupTable<KeyT>::intMapName_u code2info;
        BiLookupTable<KeyT>::strMapIndex_u name2info;

        for(const auto &entry: G_INST_TABLE) {
            if(0x00 == entry.opcode_ || entry.XLEN_.empty() || entry.name_.empty()) {
                continue;
            }
            auto manualURL= baseURL + std::string(entry.name_);

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
