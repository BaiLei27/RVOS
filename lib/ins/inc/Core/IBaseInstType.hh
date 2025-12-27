#pragma once

#include <memory>

#include "Util/BiLookupTable.hpp"
#include "ISA/InstFormat.hh"

class IBaseInstType {
protected:
    using KeyT    = uint16_t; // NOLINT
    using pTable_u= std::shared_ptr<const BiLookupTable<KeyT>>;
    pTable_u InstTypeTable_;
    std::string_view Format_ { "unknown" }; // Instruction format
    InstLayout Layout_;
    uint16_t Opcode_ {};
    KeyT FunctKey_ {};
    bool HasSetABI_ { false };

public:
    // explicit IBaseInstType(uint16_t opcode);
    explicit IBaseInstType(uint16_t opcode, bool hasSetABI= false);
    IBaseInstType(uint32_t inst, std::string_view format, bool hasSetABI= false);
    virtual ~IBaseInstType()= default;

public:
    uint16_t GetOpcode() const;
    KeyT GetFunctKey() const;
    std::string_view GetFormat() const;

    // virtual std::tuple<std::string_view, uint8_t> GetInstNameAndXLEN()= 0;
    virtual std::tuple<std::string_view, std::string_view> GetInstNameAndXLEN();
    virtual void Parse()      = 0;
    virtual void Assembly()   = 0;
    virtual void Disassembly()= 0; // hex -> add

protected:
    virtual pTable_u initInstTable()= 0;
    virtual void calculateFunctKey()= 0;
    void init();
};

// Date:25/12/19/16:23
