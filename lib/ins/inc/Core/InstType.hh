#pragma once

#include "IBaseInstType.hh"

class RType: public IBaseInstType {
public:
    // using IBaseInstType::IBaseInstType;
    RType(uint32_t inst, std::string_view format);
    RType(uint16_t opcode, std::string_view format);

public:
    // std::tuple<std::string_view, uint8_t> GetInstNameAndXLEN() override;
    // std::tuple<std::string_view, std::string_view> GetInstNameAndXLEN() override;
    // std::optional<std::string_view> GetInstName() override;
    void Parse() override;
    void Disassembly() override;
    void Assembly() override;

private:
    void calculateFunctKey() override;
    pTable_u initInstTable() override;
};

class IType: public IBaseInstType {
public:
    // using IBaseInstType::IBaseInstType;

    IType(uint32_t inst, std::string_view format);
    IType(uint16_t opcode, std::string_view format);

public:
    // std::tuple<std::string_view, std::string_view> GetInstNameAndXLEN() override;
    void Parse() override;
    void Assembly() override;
    void Disassembly() override;

private:
    void calculateFunctKey() override;
    pTable_u initInstTable() override;
};

#if 0
class SType: public IBaseInstType {
public:
    using IBaseInstType::IBaseInstType;

    SType(uint32_t inst, InstFormat format)
        : IBaseInstType(inst, format)
    {
        InstTable_= initInstTable();
    }

    void Decode() override;
    void ProcessFunct() override;

private:
   pTable_u  initInstTable() override;
};

class BType: public IBaseInstType {
public:
    using IBaseInstType::IBaseInstType;

    BType(uint32_t inst, InstFormat format)
        : IBaseInstType(inst, format)
    {
        InstTable_= initInstTable();
    }

    void Decode() override;
    void ProcessFunct() override;

private:
   pTable_u  initInstTable() override;
};

class UType: public IBaseInstType {
public:
    using IBaseInstType::IBaseInstType;

    UType(uint32_t inst, InstFormat format)
        : IBaseInstType(inst, format)
    {
        InstTable_= initInstTable();
    }

    void Decode() override;
    void ProcessFunct() override;

private:
   pTable_u  initInstTable() override;
};

class JTypeInst: public IBaseInstType {
public:
    using IBaseInstType::IBaseInstType;

    void Decode() override;
    void ProcessFunct() override;

private:
   pTable_u  initInstTable() override;
};

#endif

// Date:25/12/22/23:52
