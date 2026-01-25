#pragma once
#include <bitset>
#include <sstream>

#include "Core/IBaseInstType.hh"

class Instruction {
private:
    std::unique_ptr<IBaseInstType> Type_;
    std::stringstream Disassembly_ { "unimp" };
    std::string Format_ { "UNKNOW" }, XLEN_ { "UNDEF" }, Manual_ { "Not available" };
    std::bitset<32> BitField_; // 32-bit bit field storage

public:
    explicit Instruction(uint32_t inst, bool hasSetABI= false);
    explicit Instruction(std::string &assembly, bool hasSetABI= false);
    // explicit Instruction(std::stringstream assembly, bool hasSetABI= false);
    Instruction(const Instruction &)            = delete;
    Instruction &operator= (const Instruction &)= delete;
    Instruction(Instruction &&that) noexcept;
    Instruction &operator= (Instruction &&that) noexcept;
    virtual ~Instruction()= default;

    explicit operator std::string () const;
    explicit operator uint32_t () const;

    const IBaseInstType *GetTypePtr() const;
    const IBaseInstType &GetType() const;
    const std::bitset<32> &GetBitField() const;

    [[nodiscard]] std::string GetHexStr() const;
    [[nodiscard]] std::string_view GetXLEN() const;
    [[nodiscard]] std::string_view GetManual() const;
    [[nodiscard]] std::string_view GetFormat() const noexcept;

    bool Decode();
    void ShowInfo() const;

private:
    // static std::vector<std::string> handleAssembly(std::string &assembly);
    void resetStream();
};

// Date:25/12/22/14:56
