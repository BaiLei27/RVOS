/** qemu-virt_rv.ld */

__flash      = 0x80000000;
__flash_size = 64M;
__ram        = 0x84000000;
__ram_size   = 256M;

__stack_size    = 16k;
__heap_size_min = 16M;

INCLUDE picolibcpp.ld

PROVIDE(_stack_top = __stack);
PROVIDE(_sidata = __data_source);
PROVIDE(_sdata = __data_start);
PROVIDE(_edata = __edata);
PROVIDE(_sbss = __non_tls_bss_start);
PROVIDE(_ebss = __bss_end);
PROVIDE(__sbss_start = __bss_end);
PROVIDE(__sbss_end = __bss_end);
