/* os_start.S */
.section .text._start
.global _start
.type _start, @function

_start:
    /* The Global Pointer (GP) must be initialized first.
     * This is a mandatory requirement of the RISC-V architecture.
     */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* 1) setup stack first */
    la sp, _stack_top

    /* 2) copy .data from ROM image to RAM */
    la t0, _sidata
    la t1, _sdata
    la t2, _edata

1:
    bgeu t1, t2, 2f
    ld t3, 0(t0)
    sd t3, 0(t1)
    addi t0, t0, 8
    addi t1, t1, 8
    j 1b

2:
    la t0, _sbss
    la t1, _ebss

3:
    bgeu t0, t1, 4f
    sd zero, 0(t0)
    addi t0, t0, 8
    j 3b

4:
    la t0, __sbss_start
    la t1, __sbss_end

5:
    bgeu t0, t1, 6f
    sd zero, 0(t0)
    addi t0, t0, 8
    j 5b

6:
    call KernelInit
    /* KernelInit should not return on bare-metal */

7:
    wfi
    j 7b

.size _start, . - _start
