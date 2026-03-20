#include <format>
#include <iostream>
#include <algorithm>
#include <array>

#include "Core/Instruction.hh"
#include "Core/InstTypeFactory.hh"
#include "ISA/InstFormat.hh"

Instruction::Instruction(uint32_t inst, bool hasSetABI)
    : Type_(InstTypeFactory::CreateType(inst, hasSetABI)),
      BitField_(inst)
{
    if(Type_) {
        const auto &[MAN_URL, XLEN, _1]= Type_->LookupNameAndInfo();

        XLEN_  = XLEN;
        Manual_= MAN_URL;
        Format_= GetFormat();
    }
}

Instruction::Instruction(std::string_view assembly, bool hasSetABI)
{
    std::string s(assembly);
    std::ranges::replace(s, ',', ' ');
    std::istringstream tmp(s);

    std::vector<std::string> parts;
    for(std::string token; tmp >> token;) {
        parts.emplace_back(std::move(token));
    }

    Type_= InstTypeFactory::CreateType(std::move(parts), hasSetABI);

    if(Type_) {
        const auto &[MAN_URL, XLEN, _1, opc]= Type_->LookupIdxAndInfo();

        XLEN_  = XLEN;
        Manual_= MAN_URL;
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
    that.Format_= "UNKNOW";
    that.Manual_= "Not available";
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
        that.Format_= "UNKNOW";
        that.Manual_= "Not available";
    }
    return *this;
}

Instruction::operator std::string() const { return Disassembly_.str(); }

Instruction::operator uint32_t() const { return BitField_.to_ulong(); }

const IBaseInstType *Instruction::GetTypePtr() const { return Type_.get(); }

const IBaseInstType &Instruction::GetType() const { return *Type_; }

const std::bitset<32> &Instruction::GetBitField() const { return BitField_; }

std::string Instruction::GetHexStr() const
{
    return std::format("{:08X}", static_cast<unsigned>(BitField_.to_ulong()));
}

std::string Instruction::GetBinStr() const { return BitField_.to_string(); }

std::string_view Instruction::GetXLEN() const { return XLEN_; }

std::string_view Instruction::GetManual() const { return Manual_; }

std::string_view Instruction::GetFormat() const noexcept
{
    if(!Type_) return "UNKNOW";

    const auto FMT= Type_->GetInstFormat();
    if(InstFormat::UNKNOWN == FMT) return "UNKNOW";
    using namespace std::string_view_literals;
    static constexpr std::array<std::string_view, 6> S_NAMES { "R-Type"sv, "I-Type"sv, "S-Type"sv, "B-Type"sv, "U-Type"sv, "J-Type"sv };

    const auto IDX= static_cast<std::size_t>(FMT);
    return IDX < S_NAMES.size() ? S_NAMES[IDX] : "UNKNOW";
}

bool Instruction::Decode()
{
    if(Type_) {
        if(BitField_.none()) {
            std::bitset<32> tmp(Type_->Assembly());
            BitField_= std::move(tmp);
            resetStream();

        } else {
            Type_->Disassembly();
        }
        Type_->Parse();

        const auto &v= Type_->GetInstAssembly();
        for(const auto &e: v) {
            Disassembly_ << e;
            // std::cout << Disassembly_.str() << '\n';
        }
        return true;
    }

    std::cout << "unimp instruction: 0x" << std::hex << BitField_.to_ulong() << '\n';

    return false;
}

void Instruction::ShowInfo() const
{
    std::cout << "BitField: " << BitField_ << '\n'
              << "Assembly: " << Disassembly_.str() << '\n';
    std::cout << "Format: " << Format_ << '\n'
              << "Arch: " << XLEN_ << '\n'
              << "Manual: " << Manual_ << '\n';
}

void Instruction::resetStream()
{
    Disassembly_.str("");
    Disassembly_.clear();
    // Disassembly_.seekg(0);
    // Disassembly_.seekp(0);
}
