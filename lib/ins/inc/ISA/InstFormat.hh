#pragma once

#include <cstdint>

enum class InstFormat : int8_t { // Instruction format enumeration
    UNKOWN= -1,
    R, // Register-Register
    I, // Immediate-Register
    S, // Store instruction
    B, // Branch instruction
    U, // Upper immediate (lui/auipc)
    J, // Jump instruction (jal)
};

// NOLINTBEGIN

// 32-bit instruction parsing union
union InstLayout {
    uint32_t entity_; // Complete 32-bit instruction

    struct { // R(opcode:[0:6] | rd:[7:11] | funct3:[12:14] | rs1:[15:19] | rs2:[20:24] | funct7:[25:31])
        uint32_t opc : 7;
        uint32_t rd  : 5;
        uint32_t fct3: 3;
        uint32_t rs1 : 5;
        uint32_t rs2 : 5;
        uint32_t fct7: 7;
    } R;

    struct { // I(opcode:[0:6] | rd:[7:11] | funct3:[12:14] | rs1:[15:19] | imm0tB:[20:31])
        uint32_t opc   : 7;
        uint32_t rd    : 5;
        uint32_t fct3  : 3;
        uint32_t rs1   : 5;
        uint32_t imm0tB: 12; // imm0~imm11
    } I;

    struct { // S(opcode:[0:6] | imm0t4:[7:11] | funct3:[12:14] | rs1:[15:19] | rs2:[20:24] | imm5tB:[25:31])
        uint32_t opc   : 7;
        uint32_t imm0t4: 5; // imm0~imm4
        uint32_t fct3  : 3;
        uint32_t rs1   : 5;
        uint32_t rs2   : 5;
        uint32_t imm5tB: 7; // imm5~imm11
    } S;

    struct { // B(opcode:[0:6] | immB:[7:7] | imm1t4:[8:11] | funct3:[12:14] | rs1:[15:19] | rs2:[20:24] | imm5tA:[25:30] | immC:[31:31])
        uint32_t opc   : 7;
        uint32_t immB  : 1; // imm11
        uint32_t imm1t4: 4; // imm1~imm4
        uint32_t fct3  : 3;
        uint32_t rs1   : 5;
        uint32_t rs2   : 5;
        uint32_t imm5tA: 6; // imm5~imm10
        uint32_t immC  : 1; // imm12
    } B;

    struct { // U(opcode:[0:6] | rd:[7:11] | immCt1F:[12:31])
        uint32_t opc    : 7;
        uint32_t rd     : 5;
        uint32_t immCt1F: 20; // imm12~imm31
    } U;

    struct { // J(opcode:[0:6] | rd:[7:11] | immCt13:[12:19] | immB:[20:20] | imm1tA:[21:30] | imm14:[31:31])
        uint32_t opc    : 7;
        uint32_t rd     : 5;
        uint32_t immCt13: 8;  // imm12~imm19
        uint32_t immB   : 1;  // imm11
        uint32_t imm1tA : 10; // imm1~imm10
        uint32_t imm14  : 1;  // imm20
    } J;

    // Constructor: Initialize from 32-bit instruction
    InstLayout(uint32_t instruction= 0);

    InstLayout &operator= (uint32_t instruction);

    operator uint32_t() const;
};

// NOLINTEND

// Date:25/12/21/22:34
