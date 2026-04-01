#include <iostream>

#include "Core/JType.hh"
#include "ISA/Regs.hpp"

namespace {

int32_t decodeJImm(const InstLayout &L)
{
    uint32_t u= (static_cast<uint32_t>(L.J.imm14) << 20) | (static_cast<uint32_t>(L.J.imm1tA) << 1)
                | (static_cast<uint32_t>(L.J.immB) << 11) | (static_cast<uint32_t>(L.J.immCt13) << 12);
    if(u & (1u << 20)) {
        u|= 0xFFE00000u;
    }
    return static_cast<int32_t>(u);
}

void encodeJImm(InstLayout &L, int32_t imm)
{
    uint32_t u= static_cast<uint32_t>(imm) & 0x1FFFFFu;
    L.J.imm14  = (u >> 20) & 1;
    L.J.imm1tA = (u >> 1) & 0x3FF;
    L.J.immB   = (u >> 11) & 1;
    L.J.immCt13= (u >> 12) & 0xFF;
}

} // namespace

JType::JType(uint32_t inst, InstFormat format, bool hasSetABI)
    : IBaseInstType(inst, format, hasSetABI)
{
    init();
}

JType::JType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI)
    : IBaseInstType(std::move(instAssembly), format, hasSetABI)
{
    init();
}

void JType::Parse()
{
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.J.opc));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.J.rd));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.J.immCt13));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.J.immB));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.J.imm1tA));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.J.imm14));

    const int32_t imm= decodeJImm(Layout_);
    std::cout << "opcode: 0x" << std::hex << Opcode_ << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "rd: " << std::dec << Layout_.J.rd << '\n'
              << "imm: " << imm << '\n';
}

void JType::mnemonicHelper()
{
    auto rd       = isa::LOOKUP_REG_NAME(Layout_.J.rd, HasSetABI_);
    int32_t imm   = decodeJImm(Layout_);
    std::string immStr= std::to_string(imm);

    appendOperands({" ", rd, ",", std::string_view(immStr) });
}

const std::vector<std::string> &JType::Disassembly()
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

const InstLayout &JType::Assembly()
{
    const auto &info= LookupIdxAndInfo();

    Layout_.J.opc= Opcode_= info.opcode_;

    if(!InstAssembly_.empty() && InstAssembly_.size() >= 3) {
        if(auto rdOpt= isa::LOOKUP_REG_IDX(InstAssembly_.at(1))) {
            Layout_.J.rd= *rdOpt;
        }
        int32_t imm= std::stoi(InstAssembly_.at(2));
        encodeJImm(Layout_, imm);
    }

    mnemonicHelper();

    return Layout_;
}

IBaseInstType::KeyT JType::calculateFunctKey()
{
    FunctKey_= 0;
    return FunctKey_;
}

IBaseInstType::pBiTable_u JType::buildTable()
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
