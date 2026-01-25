
#include "Core/IBaseInstType.hh"

// #include <iostream>

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

void IBaseInstType::SetFormat(InstFormat format) noexcept { Format_= format; }

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

BiLookupTable<IBaseInstType::KeyT>::NameInfo IBaseInstType::LookupNameAndInfo()
{
    if(NameAndXlenCache_) {
        return *NameAndXlenCache_;
    }
    return { .manual_= "inst Not available", .XLEN_= "INST UNDEF", .name_= "inst unimp" };
}

BiLookupTable<IBaseInstType::KeyT>::IndexInfo IBaseInstType::LookupIdxAndInfo()
{
    if(FunctOpcAndXlenCache_) {
        return *FunctOpcAndXlenCache_;
    }

    return { .manual_= "inst Not available", .XLEN_= "INST UNDEF", .funct_= FunctKey_, .opcode_= Opcode_ };
}

void IBaseInstType::appendOperands(std::initializer_list<std::string_view> regMnemonic)
{

    // for(const auto *pIt= regMnemonic.begin(); pIt != regMnemonic.end(); ++pIt) { // NOLINT
    //     InstAssembly_.emplace_back(*pIt);
    //     if(std::next(pIt) != regMnemonic.end()) {
    //         InstAssembly_.emplace_back(", ");
    //     }
    // }

    size_t writeIdx= 1;

    for(const auto &r: regMnemonic) {
        if(writeIdx >= InstAssembly_.size()) {
            InstAssembly_.emplace_back(r);
        } else {
            InstAssembly_[writeIdx]= r;
        }
        ++writeIdx;
    }

    if(InstAssembly_.size() > writeIdx) {
        InstAssembly_.resize(writeIdx);
    }
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
