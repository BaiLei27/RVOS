#include <iostream>
#include <string>

#include "Core/SType.hh"
#include "ISA/Regs.hpp"

namespace {

int32_t decodeSImm12(const InstLayout &L)
{
    const uint32_t u= (static_cast<uint32_t>(L.S.imm5tB) << 5) | static_cast<uint32_t>(L.S.imm0t4);
    return static_cast<int32_t>(u << 20) >> 20;
}

void encodeSImm12(InstLayout &L, int32_t imm)
{
    const uint32_t u= static_cast<uint32_t>(imm) & 0xFFFU;
    L.S.imm0t4= u & 0x1FU;
    L.S.imm5tB = (u >> 5) & 0x7FU;
}

bool parseStoreAddr(std::string_view tok, int32_t &immOut, std::string &regStrOut)
{
    const auto lparen= tok.find('(');
    const auto rparen= tok.find(')');
    if(lparen == std::string_view::npos || rparen == std::string_view::npos || rparen <= lparen + 1U) {
        return false;
    }
    const std::string immPart(tok.substr(0, lparen));
    regStrOut.assign(tok.substr(lparen + 1, rparen - lparen - 1));
    immOut= static_cast<int32_t>(std::stol(immPart, nullptr, 0));
    return true;
}

} // namespace

SType::SType(uint32_t inst, InstFormat format, bool hasSetABI)
    : IBaseInstType(inst, format, hasSetABI)
{
    init();
}

SType::SType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI)
    : IBaseInstType(std::move(instAssembly), format, hasSetABI)
{
    init();
}

void SType::Parse()
{
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.S.opc));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.S.imm0t4));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.S.fct3));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.S.rs1));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.S.rs2));
    InstBitsField_.emplace_back(static_cast<uint32_t>(Layout_.S.imm5tB));

    std::cout << "opcode: 0x" << std::hex << Opcode_ << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "funct3: " << std::dec << Layout_.S.fct3 << '\n'
              << "rs1: " << Layout_.S.rs1 << '\n'
              << "rs2: " << Layout_.S.rs2 << '\n'
              << "imm: " << decodeSImm12(Layout_) << '\n';
}

void SType::mnemonicHelper()
{
    auto rs2= isa::LOOKUP_REG_NAME(Layout_.S.rs2, HasSetABI_);
    auto rs1= isa::LOOKUP_REG_NAME(Layout_.S.rs1, HasSetABI_);
    const std::string immStr= std::to_string(decodeSImm12(Layout_));
    appendOperands({" ", rs2, ",", std::string_view(immStr), "(", rs1, ")" });
}

const std::vector<std::string> &SType::Disassembly()
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

const InstLayout &SType::Assembly()
{
    const auto &info= LookupIdxAndInfo();

    Layout_.S.opc= Opcode_= info.opcode_;
    Layout_.S.fct3= static_cast<uint32_t>(info.funct_ & 7U);

    if(InstAssembly_.size() >= 4U) {
        if(auto rs2Opt= isa::LOOKUP_REG_IDX(InstAssembly_.at(1))) {
            Layout_.S.rs2= *rs2Opt;
        }
        const int32_t imm= static_cast<int32_t>(std::stol(InstAssembly_.at(2), nullptr, 0));
        encodeSImm12(Layout_, imm);
        if(auto rs1Opt= isa::LOOKUP_REG_IDX(InstAssembly_.at(3))) {
            Layout_.S.rs1= *rs1Opt;
        }
    } else if(InstAssembly_.size() >= 3U) {
        if(auto rs2Opt= isa::LOOKUP_REG_IDX(InstAssembly_.at(1))) {
            Layout_.S.rs2= *rs2Opt;
        }
        int32_t imm {};
        std::string baseReg;
        if(parseStoreAddr(InstAssembly_.at(2), imm, baseReg)) {
            encodeSImm12(Layout_, imm);
            if(auto rs1Opt= isa::LOOKUP_REG_IDX(baseReg)) {
                Layout_.S.rs1= *rs1Opt;
            }
        }
    }

    mnemonicHelper();

    return Layout_;
}

IBaseInstType::KeyT SType::calculateFunctKey()
{
    FunctKey_= static_cast<KeyT>((static_cast<uint16_t>(Opcode_) << 8) | (Layout_.S.fct3 & 7U));
    return FunctKey_;
}

IBaseInstType::pBiTable_u SType::buildTable()
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
