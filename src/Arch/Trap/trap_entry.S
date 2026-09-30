/*
 * S-mode trap entry, direct (non-vectored) stvec mode.
 *
 * On entry the hardware has already:
 *   - saved the interrupted PC into sepc
 *   - updated sstatus: SPIE = SIE, SIE = 0, SPP = previous privilege mode
 * We save the caller-saved registers plus tp (Cpuid() reads it), call the
 * C++ dispatcher, restore them and sret.
 */
.section .text.svec_table, "ax", @progbits
.align 4
.globl s_trap_vector_base

s_trap_vector_base:
    addi    sp, sp, -256

    sd      ra,  0(sp)
    sd      gp,  16(sp)
    sd      tp,  24(sp)
    sd      t0,  32(sp)
    sd      t1,  40(sp)
    sd      t2,  48(sp)
    sd      a0,  72(sp)
    sd      a1,  80(sp)
    sd      a2,  88(sp)
    sd      a3,  96(sp)
    sd      a4,  104(sp)
    sd      a5,  112(sp)
    sd      a6,  120(sp)
    sd      a7,  128(sp)
    sd      t3,  216(sp)
    sd      t4,  224(sp)
    sd      t5,  232(sp)
    sd      t6,  240(sp)

    call    s_trap_handler

    ld      ra,  0(sp)
    ld      gp,  16(sp)
    ld      tp,  24(sp)
    ld      t0,  32(sp)
    ld      t1,  40(sp)
    ld      t2,  48(sp)
    ld      a0,  72(sp)
    ld      a1,  80(sp)
    ld      a2,  88(sp)
    ld      a3,  96(sp)
    ld      a4,  104(sp)
    ld      a5,  112(sp)
    ld      a6,  120(sp)
    ld      a7,  128(sp)
    ld      t3,  216(sp)
    ld      t4,  224(sp)
    ld      t5,  232(sp)
    ld      t6,  240(sp)

    addi    sp, sp, 256
    sret
