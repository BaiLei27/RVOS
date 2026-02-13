#pragma once

#include <memory>
#include <vector>

#include "Util/BiLookupTable.hpp"
#include "ISA/InstFormat.hh"

class IBaseInstType {
public:
    // friend class InstTypeFactory;
    using KeyT      = uint16_t; // NOLINT
    using pBiTable_u= std::shared_ptr<const BiLookupTable<KeyT>>;
    using infoTup_u = std::tuple<KeyT, uint16_t, std::string_view, std::string_view>; // functKey, opcode, name, XLEN

    struct InstInfo {
        std::string_view name_;
        std::string_view XLEN_; // NOLINT
        KeyT funct_ {};
        uint16_t opcode_ {};
    };

protected:
    pBiTable_u InstTable_;
    std::optional<BiLookupTable<KeyT>::IndexInfo> FunctOpcAndXlenCache_;
    std::optional<BiLookupTable<KeyT>::NameInfo> NameAndXlenCache_;
    std::vector<std::string> InstAssembly_;
    std::string BaseURL_ { R"(https://riscv-software-src.github.io/riscv-unified-db/manual/html/isa/isa_20240411/insts/)" };
    InstFormat Format_ { InstFormat::UNKOWN };
    InstLayout Layout_;
    uint16_t Opcode_ {};
    KeyT FunctKey_ { 0xffff };
    bool HasSetABI_ {};

public:
    IBaseInstType(uint16_t opcode, bool hasSetABI);
    IBaseInstType(uint32_t inst, InstFormat format, bool hasSetABI);
    /**
     * @brief Construct a new IBaseInstType object
     * @param instAssembly , why rvalues ?
     * 1. Store objects in constructors or setters.
     * 2. Want to support both lvalues ​​and rvalues.
     *    2.1 For rvalues ​​(temporary objects), the entire process involves only a move operation (a lightweight operation),
     *        avoiding unnecessary deep copies.
     *    2.2 For lvalues, a necessary copy is made before the move operation is performed on the member.
     *        Only unavoidable copy operations are performed.
     * 3. Types with low construction overhead (e.g., string/vector).
     * 4. Don't want to write two overload statements.
     * @param format
     * @param hasSetABI
     */
    IBaseInstType(std::vector<std::string> instAssembly, InstFormat format, bool hasSetABI);
    // IBaseInstType(std::vector<std::string> &&instAssembly, bool hasSetABI);
    // IBaseInstType(std::string_view assembly, InstFormat format, bool hasSetABI);
    virtual ~IBaseInstType()= default;

public:
    const InstLayout &GetInstLayout() const noexcept;
    const std::vector<std::string> &GetInstAssembly() const noexcept;
    [[nodiscard]] uint16_t GetInstOpcode() const noexcept;
    [[nodiscard]] KeyT GetInstFunctKey() const noexcept;
    [[nodiscard]] InstFormat GetInstFormat() const noexcept;
    // std::string_view GetInstDisassembly() const;

    void SetInstAssembly(std::vector<std::string> assembly);

    void SetFormat(InstFormat format) noexcept;

    // virtual std::tuple<std::string_view, std::string_view> LookupNameAndXLEN();
    virtual BiLookupTable<KeyT>::NameInfo LookupNameAndInfo();
    virtual BiLookupTable<KeyT>::IndexInfo LookupIdxAndInfo();
    virtual void Parse()= 0;
    // virtual std::string_view GetInstXLEN() const              = 0;
    virtual const InstLayout &Assembly()                 = 0;
    virtual const std::vector<std::string> &Disassembly()= 0; // hex -> add

protected:
    virtual pBiTable_u buildTable() = 0;
    virtual KeyT calculateFunctKey()= 0;
    virtual void mnemonicHelper() { };
    void init();
    void appendOperands(std::initializer_list<std::string_view> regs);
};

// Date:25/12/19/16:23
