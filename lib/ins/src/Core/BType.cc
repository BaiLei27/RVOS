#include <iostream>
#include <string>

#include "Core/BType.hh"
#include "ISA/Regs.hpp"

namespace {

int32_t decodeBImm13(const InstLayout &L)
{
    const uint32_t u= (static_cast<uint32_t>(L.B.immC) << 12) | (static_cast<uint32_t>(L.B.immB) << 11)
                      | (static_cast<uint32_t>(L.B.imm5tA) << 5) | (static_cast<uint32_t>(L.B.imm1t4) << 1);
    return static_cast<int32_t>(u << 19) >> 19;
}

void encodeBImm13(InstLayout &L, int32_t imm)
{
    const uint32_t u= static_cast<uint32_t>(imm) & 0x1FFFU;
    L.B.imm1t4= (u >> 1) & 0xFU;
    L.B.immB  = (u >> 11) & 1U;
    L.B.imm5tA= (u >> 5) & 0x3FU;
    L.B.immC  = (u >> 12) & 1U;
}

} // namespace

BType::BType(uint32_t inst, InstFormat format, bool hasSetABI)
    : IBaseInstType(inst, format, hasSetABI)
{
    init();
}

BType::BType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI)
    : IBaseInstType(std::move(instAssembly), format, hasSetABI)
{
    init();
}

void BType::Parse()
{
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.opc));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.immB));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.imm1t4));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.fct3));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.rs1));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.rs2));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.imm5tA));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.B.immC));

    std::cout << "opcode: 0x" << std::hex << Opcode_ << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "funct3: " << std::dec << Layout_.B.fct3 << '\n'
              << "rs1: " << Layout_.B.rs1 << '\n'
              << "rs2: " << Layout_.B.rs2 << '\n'
              << "imm: " << decodeBImm13(Layout_) << '\n';
}

void BType::mnemonicHelper()
{
    auto rs1= isa::LOOKUP_REG_NAME(Layout_.B.rs1, HasSetABI_);
    auto rs2= isa::LOOKUP_REG_NAME(Layout_.B.rs2, HasSetABI_);
    const std::string immStr= std::to_string(decodeBImm13(Layout_));
    appendOperands({" ", rs1, ",", rs2, ",", std::string_view(immStr) });
}

const std::vector<std::string> &BType::Disassembly()
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

const InstLayout &BType::Assembly()
{
    const auto &info= LookupIdxAndInfo();

    Layout_.B.opc= Opcode_= info.opcode_;
    Layout_.B.fct3= static_cast<uint32_t>(info.funct_ & 7U);

    if(InstAssembly_.size() >= 4U) {
        if(auto r1= isa::LOOKUP_REG_IDX(InstAssembly_.at(1))) {
            Layout_.B.rs1= *r1;
        }
        if(auto r2= isa::LOOKUP_REG_IDX(InstAssembly_.at(2))) {
            Layout_.B.rs2= *r2;
        }
        const int32_t imm= static_cast<int32_t>(std::stol(InstAssembly_.at(3), nullptr, 0));
        encodeBImm13(Layout_, imm);
    }

    mnemonicHelper();

    return Layout_;
}

IBaseInstType::KeyT BType::calculateFunctKey()
{
    FunctKey_= static_cast<KeyT>((static_cast<uint16_t>(Opcode_) << 8) | (Layout_.B.fct3 & 7U));
    return FunctKey_;
}

IBaseInstType::pBiTable_u BType::buildTable()
{
    static auto s_instTable= [](const std::string &baseURL) -> pBiTable_u {
        BiLookupTable<KeyT>::intMapName_u code2info;
        BiLookupTable<KeyT>::strMapIndex_u name2info;

        for(const auto &entry: G_INST_TABLE) {
            if(0U == entry.opcode_ || entry.XLEN_.empty() || entry.name_.empty()) {
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
