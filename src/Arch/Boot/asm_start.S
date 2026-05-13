    .section .text._start
    .global _start
    .type _start, @function

_start:
    la sp, _stack_top       /* load address of stack top into sp */
    la  t0, .bss            /* bss start (symbol from linker) */
    la  t1, _bss_end        /* bss end */

1:  bge  t0, t1, 2f
    sd   zero, 0(t0)
    addi t0, t0, 8
    j    1b

2:
    call main
    li   a0, 0             /* exit code 0 */
    li   a7, 93            /* syscall: exit */
    ecall
	.end
