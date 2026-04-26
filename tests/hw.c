#include <stdio.h>
#include <stdlib.h>

#if !defined(__riscv)
    #warning  "This program is for RISC-V only"
#endif
#if __riscv_xlen == 32
    #define XLEN 32
#elif __riscv_xlen == 64
    #define XLEN 64
#else
    #define XLEN 64
    #warning "Unsupported RISC-V XLEN, Default to 64-bit"
#endif

int main(void)
{
    printf("Hello RISCV-%d\n", XLEN);
    exit(0);
}
