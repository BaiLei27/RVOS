
#include "Core/IBaseInstType.hh"

IBaseInstType::IBaseInstType(uint16_t opcode, bool hasSetABI)
    : Opcode_(opcode), HasSetABI_(hasSetABI) { }

IBaseInstType::IBaseInstType(uint32_t inst,
                             InstFormat format,
                             bool hasSetABI)
    : Layout_(inst),
      Opcode_(inst & 0x7F),
      Format_(format),
      HasSetABI_(hasSetABI) { }

const InstLayout &IBaseInstType::GetInstLayout() const noexcept { return Layout_; }

const std::vector<std::string_view> &IBaseInstType::GetInstAssembly() const noexcept { return InstAssembly_; }

uint16_t IBaseInstType::GetInstOpcode() const noexcept { return Opcode_; }

uint16_t IBaseInstType::GetInstFunctKey() const noexcept { return FunctKey_; }

InstFormat IBaseInstType::GetInstFormat() const noexcept { return Format_; }

// std::string_view IBaseInstType::GetInstDisassembly() const { return InstDisassembly_; }

void IBaseInstType::init()
{
    calculateFunctKey();
    InstTable_= buildTable();

    if(InstTable_) {
        NameAndXlenCache_= InstTable_->Find(FunctKey_);
    }
}

std::tuple<std::string_view, std::string_view> IBaseInstType::LookupNameAndXLEN()
{
    if(NameAndXlenCache_) {
        return *NameAndXlenCache_;
    }
    return { "unimp", "UNDEF" };
}
