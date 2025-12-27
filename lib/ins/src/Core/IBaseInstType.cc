
#include "Core/IBaseInstType.hh"

IBaseInstType::IBaseInstType(uint16_t opcode, bool hasSetABI)
    : Opcode_(opcode), HasSetABI_(hasSetABI) { }

IBaseInstType::IBaseInstType(uint32_t inst,
                             std::string_view format,
                             bool hasSetABI)
    : Layout_(inst),
      Opcode_(inst & 0x7F),
      Format_(format),
      HasSetABI_(hasSetABI) { }

uint16_t IBaseInstType::GetOpcode() const { return Opcode_; }

uint16_t IBaseInstType::GetFunctKey() const { return FunctKey_; }

std::string_view IBaseInstType::GetFormat() const { return Format_; }

void IBaseInstType::init()
{
    calculateFunctKey();
    InstTypeTable_= initInstTable();
}

std::tuple<std::string_view, std::string_view> IBaseInstType::GetInstNameAndXLEN()
{
    auto opt= InstTypeTable_->Find(FunctKey_);
    if(opt) {
        return *opt;
    }
    return {};
}
