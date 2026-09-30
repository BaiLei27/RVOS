#include "CSR/CSRManager.hh"
#include "Trap/Trap.hh"
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
    if(init() != 0) {
        return -1;
    }

    // Install the S-mode trap vector, register the timer tick hook, then
    // drop from M-mode into S-mode. StartSupervisorMode does not return.
    arch::trap::SupervisorTrap::SetTickHandler(Kernel::OnTimerTick);
    arch::trap::SupervisorTrap::Init();
    arch::csr::Manager::Instance().StartSupervisorMode(Kernel::SupervisorMain);
}

void Kernel::SupervisorMain() noexcept
{
    // Running in S-mode: enable delegated interrupts, then idle.
    arch::csr::Manager::Instance().EnableInterrupts();
    for(;;) {
        __asm__ volatile("wfi" ::: "memory");
    }
}

void Kernel::OnTimerTick() noexcept
{
    ++Kernel::Instance().TickCount_;
}
