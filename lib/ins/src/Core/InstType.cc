
#include <iostream>

#include "Core/InstType.hh"
#include "ISA/Regs.hh"

// #include "Log/Logger.hpp"

RType::RType(uint32_t inst, std::string_view format): IBaseInstType(inst, format)
{
    init();
}

RType::RType(KeyT opcode, std::string_view format): IBaseInstType(opcode, format)
{
    // FunctKey_     = Layout_.R.fct7 << 3 | Layout_.R.fct3;
    // InstTypeTable_= initInstTable();
    init();
}

void RType::Parse()
{
    std::cout << "opcode: 0x" << std::hex << GetOpcode() << '\n'
              << "Hexadecimal: 0x" << Layout_.entity_ << '\n'
              << "funct3: " << Layout_.R.fct3 << '\n'
              << "funct7: " << Layout_.R.fct7 << '\n'
              << "rs1: " << Layout_.R.rs1 << '\n'
              << "rs2: " << Layout_.R.rs2 << '\n'
              << "rd: " << Layout_.R.rd << '\n';
}

void RType::Disassembly()
{
    // std::cout << Layout_.entity_ << '\n';
    if(!InstTypeTable_) {
        InstTypeTable_= initInstTable();
    }

    // const auto &[instName, xlen]= GetInstNameAndXLEN();
    if(HasSetABI_) {
        std::cout << G_INDEX_REGS_ABI[Layout_.R.rd].second << ", "
                  << G_INDEX_REGS_ABI[Layout_.R.rs1].second << ", "
                  << G_INDEX_REGS_ABI[Layout_.R.rs2].second << '\n';
    } else {
        std::cout << G_INDEX_REGS_ABI[Layout_.R.rd].first << ", "
                  << G_INDEX_REGS_ABI[Layout_.R.rs1].first << ", "
                  << G_INDEX_REGS_ABI[Layout_.R.rs2].first << '\n';
    }
}

void RType::Assembly() { }

void RType::calculateFunctKey()
{
    FunctKey_= Layout_.R.fct7 << 3 | Layout_.R.fct3;
}

IType::IType(uint32_t inst, std::string_view format): IBaseInstType(inst, format)
{
    init();
}

IType::IType(KeyT opcode, std::string_view format): IBaseInstType(opcode, format)
{
    // FunctKey_     = Layout_.R.fct7 << 3 | Layout_.R.fct3;
    // InstTypeTable_= initInstTable();
    init();
}

void IType::Parse()
{
    std::cout
        << "Hexadecimal: 0x" << Layout_.entity_ << '\n';
    std::cout << "opcode: " << GetOpcode() << '\n';
    std::cout << "funct3: " << Layout_.I.fct3 << '\n';
    std::cout << "rs1: " << Layout_.I.rs1 << '\n';
    std::cout << "rd: " << Layout_.I.rd << '\n';
    std::cout << "imm12: " << Layout_.I.imm0tB << '\n';
}

void IType::Disassembly()
{
    std::cout << G_INDEX_REGS_ABI[Layout_.I.rd].first << ", "
              << G_INDEX_REGS_ABI[Layout_.I.rs1].first << ", "
              << Layout_.I.imm0tB << '\n';
}

void IType::calculateFunctKey()
{
    FunctKey_= Layout_.I.fct3;
}

void IType::Assembly() { }
#if 0
void SType::Decode()
{
    std::cout << "S-Type Instruction\n";
    std::cout << "opcode: " << GetOpcode() << '\n';
    std::cout << "funct3: " << Layout_.S.fct3 << '\n';
    std::cout << "rs1: " << Layout_.S.rs1 << '\n';
    std::cout << "rs2: " << Layout_.S.rs2 << '\n';
    KeyT imm= Layout_.S.imm5tB << 5 | Layout_.S.imm0t4;
    std::cout << "imm: " << imm << '\n';
}

std::shared_ptr<const BiLookupTable<KeyT>> SType::initInstTable()
{
    return nullptr;
}

void SType::ProcessFunct() { }

std::shared_ptr<const BiLookupTable<KeyT>> BType::initInstTable()
{
    return nullptr;
}

void BType::Decode()
{
    std::cout << "B-Type Instruction\n";
    std::cout << "opcode: " << GetOpcode() << '\n';
    std::cout << "funct3: " << Layout_.B.fct3 << '\n';
    std::cout << "rs1: " << Layout_.B.rs1 << '\n';
    std::cout << "rs2: " << Layout_.B.rs2 << '\n';
    uint32_t imm= (Layout_.B.immC << 0xC) | (Layout_.B.immB << 0xB) | (Layout_.B.imm5tA << 5) | (Layout_.B.imm1t4 << 1);
    std::cout << "imm: " << imm << '\n';
}

void BType::ProcessFunct() { }

std::shared_ptr<const BiLookupTable<KeyT>> UType::initInstTable()
{
    return nullptr;
}

void UType::Decode()
{
    std::cout << "U-Type Instruction\n";
    std::cout << "opcode: " << GetOpcode() << '\n';
    std::cout << "rd: " << Layout_.U.rd << '\n';
    std::cout << "imm: " << Layout_.U.immCt1F << '\n';
}

void UType::ProcessFunct() { }

std::shared_ptr<const BiLookupTable<KeyT>> JTypeInst::initInstTable()
{
    return nullptr;
}

void JTypeInst::Decode()
{
    std::cout << "J-Type Instruction\n";
    std::cout << "opcode: " << GetOpcode() << '\n';
    std::cout << "rd: " << Layout_.J.rd << '\n';
    uint32_t imm= (Layout_.J.imm14 << 0x14) | (Layout_.J.immCt13 << 0xC) | (Layout_.J.immB << 0xB) | (Layout_.J.imm1tA << 1);
    std::cout << "imm: " << imm << '\n';
}

void JTypeInst::ProcessFunct() { }
#endif

IBaseInstType::pTable_u RType::initInstTable()
{
    static auto s_instTable= std::make_shared<const BiLookupTable<KeyT>>(
        BiLookupTable<KeyT>::intMapTup_u {
            { 0x00,  { "add", "RV32I" }  },
            { 0x100, { "sub", "RV32I" }  },
            { 0x01,  { "sll", "RV32I" }  },
            { 0x02,  { "slt", "RV32I" }  },
            { 0x03,  { "sltu", "RV32I" } },
            { 0x04,  { "xor", "RV32I" }  },
            { 0x05,  { "srl", "RV32I" }  },
            { 0x25,  { "sra", "RV32I" }  },
            { 0x06,  { "or", "RV32I" }   },
            { 0x07,  { "and", "RV32I" }  },
    },
        BiLookupTable<KeyT>::strMapTup_u {
            { "add", { 0x00, "RV32I" } },
            { "sub", { 0x100, "RV32I" } },
            { "sll", { 0x01, "RV32I" } },
            { "slt", { 0x02, "RV32I" } },
            { "sltu", { 0x03, "RV32I" } },
            { "xor", { 0x04, "RV32I" } },
            { "srl", { 0x05, "RV32I" } },
            { "sra", { 0x25, "RV32I" } },
            { "or", { 0x06, "RV32I" } },
            { "and", { 0x07, "RV32I" } },
        });

    return s_instTable;
}

IBaseInstType::pTable_u IType::initInstTable()
{
    static auto s_instTable= std::make_shared<const BiLookupTable<KeyT>>(
        BiLookupTable<KeyT>::intMapTup_u {
            { 0x00, { "addi", "RV32I" }  },
            { 0x01, { "slli", "RV32I" }  },
            { 0x02, { "slti", "RV32I" }  },
            { 0x03, { "sltiu", "RV32I" } },
            { 0x04, { "xori", "RV32I" }  },
            { 0x05, { "srli", "RV32I" }  },
            { 0x25, { "srai", "RV32I" }  },
            { 0x06, { "ori", "RV32I" }   },
            { 0x07, { "andi", "RV32I" }  },
    },
        BiLookupTable<KeyT>::strMapTup_u {
            { "addi", { 0x00, "RV32I" } },
            { "slli", { 0x01, "RV32I" } },
            { "slti", { 0x02, "RV32I" } },
            { "sltiu", { 0x03, "RV32I" } },
            { "xori", { 0x04, "RV32I" } },
            { "srli", { 0x05, "RV32I" } },
            { "srai", { 0x25, "RV32I" } },
            { "ori", { 0x06, "RV32I" } },
            { "andi", { 0x07, "RV32I" } },
        });
    return s_instTable;
}
