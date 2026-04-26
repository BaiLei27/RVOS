/** qemu-virt_rv.ld */

__flash = 0x80000000;
__flash_size = 128M;
__ram = 0x88000000;
__ram_size = 32M;
__stack_size = 1k;

INCLUDE picolibcpp.ld
/* INCLUDE picolibc.ld */
