#pragma once

#include <memory>
#include <vector>

#include "Util/BiLookupTable.hpp"
#include "ISA/InstFormat.hh"

class IBaseInstType {
public:
    using KeyT    = uint16_t; // NOLINT
    using pTable_u= std::shared_ptr<const BiLookupTable<KeyT>>;

protected:
    pTable_u InstTable_;
    std::vector<std::string_view> InstAssembly_;
    std::optional<BiLookupTable<KeyT>::nameAndXLEN_u> NameAndXlenCache_;
    InstFormat Format_ { InstFormat::UNKOWN };
    InstLayout Layout_;
    uint16_t Opcode_ {};
    KeyT FunctKey_ {};
    bool HasSetABI_ { false };

public:
    // explicit IBaseInstType(uint16_t opcode);
    IBaseInstType(uint16_t opcode, bool hasSetABI);
    IBaseInstType(uint32_t inst, InstFormat format, bool hasSetABI);
    virtual ~IBaseInstType()= default;

public:
    const InstLayout &GetInstLayout() const noexcept;
    const std::vector<std::string_view> &GetInstAssembly() const noexcept;
    [[nodiscard]] uint16_t GetInstOpcode() const noexcept;
    [[nodiscard]] KeyT GetInstFunctKey() const noexcept;
    [[nodiscard]] InstFormat GetInstFormat() const noexcept;
    // std::string_view GetInstDisassembly() const;

    // virtual std::tuple<std::string_view, std::string_view> LookupNameAndXLEN();
    virtual BiLookupTable<KeyT>::nameAndXLEN_u LookupNameAndXLEN();
    virtual void Parse()                                      = 0;
    virtual void Assembly()                                   = 0;
    virtual const std::vector<std::string_view> &Disassembly()= 0; // hex -> add

protected:
    virtual pTable_u buildTable()   = 0;
    virtual KeyT calculateFunctKey()= 0;
    void init();
};

// Date:25/12/19/16:23
