#pragma once

#include <memory>
#include <vector>

#include "Util/BiLookupTable.hpp"
#include "ISA/InstFormat.hh"

class IBaseInstType {
public:
    // friend class InstTypeFactory;
    using KeyT     = uint16_t; // NOLINT
    using pTable_u = std::shared_ptr<const BiLookupTable<KeyT>>;
    using infoTup_u= std::tuple<KeyT, uint16_t, std::string_view, std::string_view>; // functKey, opcode, name, XLEN

protected:
    pTable_u InstTable_;
    std::optional<BiLookupTable<KeyT>::idxAndInfo_u> FunctOpcAndXlenCache_;
    std::optional<BiLookupTable<KeyT>::nameAndInfo_u> NameAndXlenCache_;
    std::vector<std::string> InstAssembly_;
    std::string BaseURL_ { R"(https://riscv-software-src.github.io/riscv-unified-db/manual/html/isa/isa_20240411/insts/)" };
    InstFormat Format_ { InstFormat::UNKOWN };
    InstLayout Layout_;
    uint16_t Opcode_ {};
    KeyT FunctKey_ { 0xffff };
    bool HasSetABI_ { false };

public:
    IBaseInstType(uint16_t opcode, bool hasSetABI);
    IBaseInstType(uint32_t inst, InstFormat format, bool hasSetABI);
    IBaseInstType(std::vector<std::string> &instAssembly, bool hasSetABI);
    IBaseInstType(std::vector<std::string> &instAssembly, InstFormat format, bool hasSetABI);
    IBaseInstType(std::string_view &assembly, InstFormat format, bool hasSetABI);
    virtual ~IBaseInstType()= default;

public:
    const InstLayout &GetInstLayout() const noexcept;
    const std::vector<std::string> &GetInstAssembly() const noexcept;
    [[nodiscard]] uint16_t GetInstOpcode() const noexcept;
    [[nodiscard]] KeyT GetInstFunctKey() const noexcept;
    [[nodiscard]] InstFormat GetInstFormat() const noexcept;
    // std::string_view GetInstDisassembly() const;

    void SetInstAssembly(std::vector<std::string> &&assembly);

    void SetFormat(InstFormat format) { Format_= format; }

    // virtual std::tuple<std::string_view, std::string_view> LookupNameAndXLEN();
    virtual BiLookupTable<KeyT>::nameAndInfo_u LookupNameAndInfo();
    virtual BiLookupTable<KeyT>::idxAndInfo_u LookupIdxAndInfo();
    virtual void Parse()= 0;
    // virtual std::string_view GetInstXLEN() const              = 0;
    virtual const InstLayout &Assembly()                 = 0;
    virtual const std::vector<std::string> &Disassembly()= 0; // hex -> add

protected:
    virtual pTable_u buildTable()   = 0;
    virtual KeyT calculateFunctKey()= 0;
    void init();

public:
    template <std::size_t N>
    consteval auto FILTER_VALID_ENTRIES(const std::array<infoTup_u, N> &arr)

    {
        std::array<infoTup_u, N> validArr {};
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
};

// Date:25/12/19/16:23
