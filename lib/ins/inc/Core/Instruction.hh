#pragma once
#include <bitset>
#include <iostream>

#include "Core/IBaseInstType.hh"

class Instruction {
private:
    std::unique_ptr<IBaseInstType> Type_;
    std::string_view Name_ { "unimp" }, XLEN_ { "RV32I" }, Manual_ { "Not available" };
    std::bitset<32> BitField_; // 32-bit bit field storage

public:
    explicit Instruction(uint32_t inst);
    explicit Instruction(std::string_view inst);
    // Instruction(const Instruction & that) : Type_(std::move(that.Type_)) { }

    virtual ~Instruction()          = default;

public:
    void Decode();
    void Encode();
};

// Date:25/12/22/14:56
