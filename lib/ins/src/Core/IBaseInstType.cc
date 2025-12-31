
#include "Core/IBaseInstType.hh"

IBaseInstType::IBaseInstType(uint16_t opcode, bool hasSetABI)
    : Opcode_(opcode), HasSetABI_(hasSetABI) { }

IBaseInstType::IBaseInstType(uint32_t inst,
                             InstFormat format,
                             bool hasSetABI)
    : Layout_(inst),
      Format_(format),
      HasSetABI_(hasSetABI) { }

const InstLayout &IBaseInstType::GetInstLayout() const noexcept { return Layout_; }

const std::vector<std::string> &IBaseInstType::GetInstAssembly() const noexcept { return InstAssembly_; }

uint16_t IBaseInstType::GetInstOpcode() const noexcept { return Opcode_; }

uint16_t IBaseInstType::GetInstFunctKey() const noexcept { return FunctKey_; }

InstFormat IBaseInstType::GetInstFormat() const noexcept { return Format_; }

// std::string_view IBaseInstType::GetInstDisassembly() const { return InstDisassembly_; }
void IBaseInstType::SetInstAssembly(std::vector<std::string> &&instAssembly) { InstAssembly_= std::move(instAssembly); }

void IBaseInstType::init()
{
    // calculateFunctKey();
    InstTable_= buildTable();

    if(InstTable_) {
        NameAndXlenCache_= InstTable_->Find(FunctKey_);
        // FunctOpcAndXlenCache_= InstTable_->Find(InstAssembly_.at(0));
    }
}

BiLookupTable<IBaseInstType::KeyT>::nameAndInfo_u IBaseInstType::LookupNameAndInfo()
{
    if(NameAndXlenCache_) {
        return *NameAndXlenCache_;
    }
    return { "unimp", "UNDEF", "Not available" };
}

BiLookupTable<IBaseInstType::KeyT>::idxAndInfo_u IBaseInstType::LookupIdxAndInfo()
{
    FunctOpcAndXlenCache_= InstTable_->Find(InstAssembly_.at(0));
    if(FunctOpcAndXlenCache_) {
        return *FunctOpcAndXlenCache_;
    }

    return { FunctKey_, Opcode_, "UNDEF", "Not available" };
}
