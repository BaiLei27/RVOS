/* linker.ld — minimal RISC-V bare-metal layout */
OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    ROM (rx)  : ORIGIN = 0x80000000, LENGTH = 4M  /* code/rodata */
    RAM (rwx) : ORIGIN = 0x80100000, LENGTH = 2M  /* data/bss/stack */
}

SECTIONS
{
    /* code + read-only data */
    .text : {
        *(.text._start)   /* startup code first */
        *(.text)          /* other text (main, functions) */
        *(.rodata)
    } > ROM

    . = ALIGN(0x1000);

    .data : {
        *(.data)
    } > RAM AT > ROM

    .bss (NOLOAD) : {
        *(.bss)
        *(COMMON)
        _bss_end = .;
    } > RAM

    /* reserve stack space at end of RAM */
    .stack (NOLOAD) : {
        . = ALIGN(16);
        _stack_top = . + 0x2000; /* 8 KiB stack, adjust as needed */
    } > RAM

    /* define end symbol if needed */
    _end = .;
}
