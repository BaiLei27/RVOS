#include "Core/Instruction.hh"
#include "Core/InstTypeFactory.hh"

Instruction::Instruction(std::string_view inst) { }

Instruction::Instruction(uint32_t inst): Type_(InstTypeFactory::CreateType(inst)),
                                         BitField_(inst)
{
    if(Type_) {
        const auto &[name, xlen]= Type_->GetInstNameAndXLEN();

        Name_= name;
        XLEN_= xlen;
        // XLEN_= std::to_string(xlen);
    }
}

void Instruction::Decode()
{
    std::cout << "BitField: " << BitField_ << '\n'
              << "Assembly: " << Name_ << ' ';
    Type_->Disassembly();
    Type_->Parse();
    std::cout << "Format: " << Type_->GetFormat() << '\n'
              << "Arch: " << XLEN_ << '\n'
              << "Manual: " << Manual_ << '\n';
}

void Instruction::Encode() { }
