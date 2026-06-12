/*
 * Vectored stvec: BASE + 4 * scause.
 */
.section .text.svec_table, "ax", @progbits
.align 4
.globl s_trap_vector_base

s_trap_vector_base:
    j       trap_save_entry         /* 0  : synchronous exception */
    j       trap_save_entry         /* 1  : supervisor software interrupt */
    j       trap_resv               /* 2  : reserved */
    j       trap_resv               /* 3  */
    j       trap_resv               /* 4  */
    j       trap_save_entry         /* 5  : supervisor timer interrupt */
    j       trap_resv               /* 6  */
    j       trap_resv               /* 7  */
    j       trap_resv               /* 8  */
    j       trap_save_entry         /* 9  : supervisor external interrupt */
    .rept   22
    j       trap_unknown
    .endr

trap_resv:
trap_unknown:
    j       trap_save_entry

.text
.align 4
.globl s_trap_handler

trap_save_entry:
    addi    sp, sp, -256

    sd      ra,  0(sp)
    sd      gp,  16(sp)
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
