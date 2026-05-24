
#include "Kernel/Kernel.hh"
#include "Trap/Trap.hh"
#include "CSR/CSRManager.hh"

volatile static int g_started= 0;

extern "C" void KernelInit(void)
{
    auto &csr= arch::csr::Manager::Instance();

    if(csr.Cpuid() == 0) {
        arch::trap::SupervisorTrap::Init();
        //std::printf("Hello, RVOS\n");
        g_started= 1;
    } else {
        while(g_started == 0)
            ;
        arch::trap::SupervisorTrap::Init();
        //printf("hart %d starting \n", arch::csr::Manager::Instance().Cpuid());
    }
}

int main()
{
    arch::csr::Manager::Instance().StartSupervisorMode(&KernelInit);
    return Kernel::Instance().Run();
}

