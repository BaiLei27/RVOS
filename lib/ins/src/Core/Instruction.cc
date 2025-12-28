#include <iostream>
#include <array>

#include "Core/Instruction.hh"
#include "Core/InstTypeFactory.hh"

Instruction::Instruction(uint32_t inst, bool hasSetABI)
    : Type_(InstTypeFactory::CreateType(inst, hasSetABI)),
      BitField_(inst)
{
    if(Type_) {
        const auto &[_, xlen]= Type_->LookupNameAndXLEN();

        XLEN_  = xlen;
        Format_= GetFormat();
    }
}

Instruction::Instruction(Instruction &&that) noexcept
    : Type_(std::move(that.Type_)),
      Disassembly_(std::move(that.Disassembly_)),
      XLEN_(that.XLEN_),
      Manual_(that.Manual_),
      Format_(that.Format_),
      BitField_(that.BitField_)
{
    that.XLEN_  = "UNDEF";
    that.Format_= "Undef";
}

Instruction &Instruction::operator= (Instruction &&that) noexcept
{
    if(&that != this) {
        Type_       = std::move(that.Type_);
        Disassembly_= std::move(that.Disassembly_);
        XLEN_       = that.XLEN_;
        Manual_     = that.Manual_;
        BitField_   = that.BitField_;

        that.XLEN_  = "UNDEF";
        that.Format_= "Undef";
    }
    return *this;
}

// Instruction::Instruction(std::string_view inst) { }

Instruction::operator std::string() const { return Disassembly_.str(); }

Instruction::operator uint32_t() const { return BitField_.to_ulong(); }

const IBaseInstType &Instruction::GetType() const { return *Type_; }

const std::bitset<32> &Instruction::GetBitField() const { return BitField_; }

std::string Instruction::GetHexStr() const { return BitField_.to_string(); }

std::string_view Instruction::GetXLEN() const { return XLEN_; }

std::string_view Instruction::GetManual() const { return Manual_; }

std::string_view Instruction::GetFormat() const noexcept
{
    if(!Type_) return "Undef";

    static constexpr std::array<std::string_view, 6> S_NAMES= { "R-Type", "I-Type", "S-Type", "B-Type", "U-Type", "J-Type" };

    auto idx= static_cast<std::size_t>(Type_->GetInstFormat());

    return idx < S_NAMES.size() ? S_NAMES[idx] : "Undef";
}

void Instruction::Decode()
{
    std::cout << "BitField: " << BitField_ << '\n';

    if(Type_) {
        const auto &v= Type_->Disassembly();

        // for(char comma[] { '\0', ' ', '\0' }; const auto &i: v) {
        //     Disassembly_ << comma << i, comma[0]= ',';
        // }

        std::string sep;
        for(const auto &e: v) {
            Disassembly_ << sep << e;
            sep= (sep.empty() ? " " : ", ");
        }

        std::cout << "Assembly: " << Disassembly_.str() << '\n';
        Type_->Parse();

        std::cout << "Format: " << Format_ << '\n'
                  << "Arch: " << XLEN_ << '\n'
                  << "Manual: " << Manual_ << '\n';
    } else {
        std::cout << "unimp instruction: " << BitField_.to_ulong() << '\n';
    }
}

void Instruction::Encode() { }
