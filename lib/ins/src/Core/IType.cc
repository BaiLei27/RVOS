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
    const uint32_t opc= Layout_.I.opc;
    if(opc == 0x73) {
        if(InstAssembly_.size() > 1) {
            InstAssembly_.resize(1);
        }
        return;
    }

    const int32_t imm= static_cast<int32_t>(static_cast<int16_t>(Layout_.I.imm0tB & 0xFFF));
    const std::string immStr= std::to_string(imm);

    if(opc == 0x0F) {
        auto z= isa::LOOKUP_REG_NAME(0, HasSetABI_);
        appendOperands({" ", z, ",", z, ",", std::string_view(immStr) });
        return;
    }

    auto rd = isa::LOOKUP_REG_NAME(Layout_.I.rd, HasSetABI_);
    auto rs1= isa::LOOKUP_REG_NAME(Layout_.I.rs1, HasSetABI_);
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

    Layout_.I.opc= Opcode_= info.opcode_;
    const uint16_t key= info.funct_;

    if(info.opcode_ == 0x13) {
        Layout_.I.fct3  = key & 7;
        Layout_.I.imm0tB= (static_cast<uint32_t>((key >> 3) & 0x7F) << 5);
    } else if(info.opcode_ == 0x73) {
        Layout_.I.fct3  = 0;
        Layout_.I.imm0tB= static_cast<uint32_t>(key & 0xFFF);
    } else {
        Layout_.I.fct3  = key & 7;
        Layout_.I.imm0tB= 0;
    }

    if(info.opcode_ == 0x73) {
        Layout_.I.rd = 0;
        Layout_.I.rs1= 0;
    }

    if(!InstAssembly_.empty() && InstAssembly_.size() >= 4) {
        if(auto rdOpt= isa::LOOKUP_REG_IDX(InstAssembly_.at(1))) {
            Layout_.I.rd= *rdOpt;
        }
        if(auto rs1Opt= isa::LOOKUP_REG_IDX(InstAssembly_.at(2))) {
            Layout_.I.rs1= *rs1Opt;
        }
        const int32_t imm= std::stoi(InstAssembly_.at(3));
        if(info.opcode_ == 0x13) {
            if(Layout_.I.fct3 == 1 || Layout_.I.fct3 == 5) {
                Layout_.I.imm0tB= (Layout_.I.imm0tB & UINT32_C(0xFE0)) | (static_cast<uint32_t>(imm) & 0x1F);
            } else {
                Layout_.I.imm0tB= static_cast<uint32_t>(imm) & 0xFFF;
            }
        } else if(info.opcode_ != 0x73) {
            Layout_.I.imm0tB= static_cast<uint32_t>(imm) & 0xFFF;
        }
    }

    mnemonicHelper();

    return Layout_;
}

IBaseInstType::KeyT IType::calculateFunctKey()
{
    switch(Layout_.I.opc) {
    case 0x13:
        FunctKey_= static_cast<KeyT>(((Layout_.I.imm0tB >> 5) << 3) | Layout_.I.fct3);
        break;
    case 0x73:
        FunctKey_= static_cast<KeyT>((0x73u << 8) | (Layout_.I.imm0tB & 0xFFFu));
        break;
    default:
        FunctKey_= static_cast<KeyT>((static_cast<uint32_t>(Layout_.I.opc) << 8) | Layout_.I.fct3);
        break;
    }
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
