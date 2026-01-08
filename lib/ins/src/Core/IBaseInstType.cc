
#include "Core/IBaseInstType.hh"

// IBaseInstType::IBaseInstType(uint16_t opcode, bool hasSetABI)
//     : Opcode_(opcode), HasSetABI_(hasSetABI) { }

IBaseInstType::IBaseInstType(uint32_t inst,
                             InstFormat format,
                             bool hasSetABI)
    : Layout_(inst),
      Format_(format),
      Opcode_(inst & 0x7F),
      HasSetABI_(hasSetABI) { }

IBaseInstType::IBaseInstType(std::vector<std::string> instAssembly,
                             InstFormat format,
                             bool hasSetABI)
    : InstAssembly_(std::move(instAssembly)),
      Format_(format),
      HasSetABI_(hasSetABI) { }

const InstLayout &IBaseInstType::GetInstLayout() const noexcept { return Layout_; }

const std::vector<std::string> &IBaseInstType::GetInstAssembly() const noexcept { return InstAssembly_; }

const std::vector<uint32_t> &IBaseInstType::GetInstBitsField() const noexcept { return InstBitsField_; }

uint16_t IBaseInstType::GetInstOpcode() const noexcept { return Opcode_; }

uint16_t IBaseInstType::GetInstFunctKey() const noexcept { return FunctKey_; }

InstFormat IBaseInstType::GetInstFormat() const noexcept { return Format_; }

// std::string_view IBaseInstType::GetInstDisassembly() const { return InstDisassembly_; }
void IBaseInstType::SetInstAssembly(std::vector<std::string> instAssembly) { InstAssembly_= std::move(instAssembly); }

void IBaseInstType::init()
{
    InstTable_= buildTable();

    if(InstTable_) {
        if(InstAssembly_.empty()) {
            calculateFunctKey();
            NameAndXlenCache_= InstTable_->Find(FunctKey_);
        } else {
            FunctOpcAndXlenCache_= InstTable_->Find(InstAssembly_.at(0));
        }
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
    if(FunctOpcAndXlenCache_) {
        return *FunctOpcAndXlenCache_;
    }

    return { FunctKey_, "UNDEF", "Not available" };
}

template <std::size_t N>
consteval auto FILTER_VALID_ENTRIES(const std::array<IBaseInstType::infoTup_u, N> &arr)
{
    std::array<IBaseInstType::infoTup_u, N> validArr {};
    size_t validCnt= 0;

    for(const auto &entry: arr) {
        const uint16_t OP          = std::get<1>(entry);
        const std::string_view NAME= std::get<2>(entry);

        if(OP == 0x00 || NAME.empty()) {
            continue;
        }
        validArr[validCnt++]= entry;
    }

    return std::make_pair(validArr, validCnt);
    // return std::span(validArr.data(), validCnt);
}
