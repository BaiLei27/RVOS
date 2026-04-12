// #include <cstdio>
#include "Drivers/Uart.hh"

extern "C" int KernelInit()
{
    // puts("Hello, RVOS");
    // auto uart0= RVOSHal::Uart::Default();
    // if(uart0) {
    //     (void)RVOSHal::InitPeripheral(*uart0);
    // }
    RVOSHal::Uart uart0;
    uart0.WriteString("Hello RISC-V!\n");
    // (void)uart_puts("Hello RISC-V!\n");
    return 0;
}
