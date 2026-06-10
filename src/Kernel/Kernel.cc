#include "Platform/MemLayout.hh"
#include "Kernel/Kernel.hh"

int Kernel::boardInit()
{
    PdevMgr_= &dev::Manager::Instance();

    if(PdevMgr_->Register(dev::Handle { dts::qemu::g_uart0 }) != 0) {
        return -1;
    }
    if(PdevMgr_->InitAll() != 0) {
        return -1;
    }
    return 0;
}

int Kernel::init()
{
    if(Ready_) {
        return 0;
    }

    if(boardInit() != 0) {
        return -1;
    }

    if(!dev::Manager::Instance().FindHandle("uart0")) {
        return -1;
    }

    if(dts::qemu::g_uart0.Puts("RVOS: NS16550 (uart0) ready\n") != 0) {
        return -1;
    }

    Ready_= true;
    return 0;
}

int Kernel::Run()
{
    this->init();
    while(true) {
        __asm__ volatile("wfi" ::: "memory");
    }
}
