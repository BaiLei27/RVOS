/**
 * @file Trap.cc
 * @brief Supervisor-mode trap dispatch
 */

#include <cstdio>
#include <bit>
#include <cstdint>
#include <string_view>

#include "CSR/CSRManager.hh"
#include "Trap/Trap.hh"

extern "C" void s_trap_vector_base(void);

namespace arch::trap {
[[noreturn]] void SupervisorTrap::PanicOnSyncException() const noexcept
{
    /*
    printf("PANIC hart%lu: sync trap code=%lu (%s) sepc=0x%lx stval=0x%lx\n",
           HartId_,
           Scause_.code_,
           exceptionName(Scause_.code_).data(),
           Epc_,
           Stval_);
    */
    while(true) {
        __asm__ volatile("wfi");
    }
}

std::string_view SupervisorTrap::exceptionName(uint64_t code) const noexcept
{
    switch(static_cast<SyncException>(code)) {
    case SyncException::INST_ADDR_MISALIGNED:  return "inst addr misaligned";
    case SyncException::INST_ACCESS_FAULT:     return "inst access fault";
    case SyncException::ILLEGAL_INST:          return "illegal instruction";
    case SyncException::BREAKPOINT:            return "breakpoint";
    case SyncException::LOAD_ADDR_MISALIGNED:  return "load addr misaligned";
    case SyncException::LOAD_ACCESS_FAULT:     return "load access fault";
    case SyncException::STORE_ADDR_MISALIGNED: return "store addr misaligned";
    case SyncException::STORE_AMO_FAULT:       return "store/amo access fault";
    case SyncException::ECALL_FROM_U_MODE:     return "ecall from U-mode";
    case SyncException::ECALL_FROM_S_MODE:     return "ecall from S-mode";
    case SyncException::ECALL_FROM_M_MODE:     return "ecall from M-mode";
    default:                                   return "unknown";
    }
}

SupervisorTrap &SupervisorTrap::TrapForHart(uint64_t hartid) noexcept
{
    static SupervisorTrap s_traps[G_K_MAX_HARTS] {};

    if(hartid >= G_K_MAX_HARTS) {
        hartid= 0;
    }
    return s_traps[hartid];
}

SupervisorTrap &SupervisorTrap::GetInstance() noexcept
{
    return GetInstance(csr::Manager::Instance().Cpuid());
}

SupervisorTrap &SupervisorTrap::GetInstance(uint64_t hartid) noexcept
{
    return TrapForHart(hartid);
}

void SupervisorTrap::SetupVector() noexcept
{
    csr::Manager::Instance().WriteStvec(std::bit_cast<uint64_t>(&s_trap_vector_base) | 1U);
}

void SupervisorTrap::Panic(const char *pMsg) noexcept
{
    //printf("Panic: %s\n", pMsg);
    while(true) {
        __asm__ volatile("wfi");
    }
}

void SupervisorTrap::OnTrap() noexcept
{
    auto &csr   = csr::Manager::Instance();
    Epc_        = csr.ReadSepc();
    Scause_.raw_= csr.ReadScause();
    Sstatus_    = csr.ReadSstatus();
    Stval_      = csr.ReadStval();
    HartId_     = csr.Cpuid();

    if(!csr.IsFromSupervisorMode(Sstatus_))
        Panic("not from supervisor mode");
    if(csr.IsSupervisorInterruptEnabled(Sstatus_))
        Panic("interrupts enabled");

    if(Scause_.interrupt_) {
        Epc_= HandleInterrupt();
    } else {
        Epc_= HandleException();
    }

    csr.WriteSstatus(Sstatus_);
    csr.WriteSepc(Epc_);
}

void SupervisorTrap::Init()
{
    GetInstance().SetupVector();
}

uint64_t SupervisorTrap::HandleInterrupt() noexcept
{
    switch(Scause_.code_) {
    case 1:
        //printf("hart%lu interrupt: supervisor software\n", HartId_);
        break;
    case 5: {
        //printf("hart%lu interrupt: supervisor timer\n", HartId_);
        auto &csr= csr::Manager::Instance();
        csr.SetStimecmpIntervalTicks();
        break;
    }
    case 9:
        //printf("hart%lu interrupt: supervisor external\n", HartId_);
        break;
    default:
        //printf("hart%lu interrupt: code=%lu\n", HartId_, Scause_.code_);
        break;
    }
    return Epc_;
}

uint64_t SupervisorTrap::HandleException() noexcept
{
    switch(static_cast<SyncException>(Scause_.code_)) {
    case SyncException::BREAKPOINT:
        //printf("hart%lu breakpoint at 0x%lx\n", HartId_, Epc_);
        return Epc_ + 4U;
    case SyncException::ECALL_FROM_S_MODE:
        //printf("hart%lu ecall from S-mode at 0x%lx\n", HartId_, Epc_);
        return Epc_ + 4U;
    case SyncException::LOAD_ACCESS_FAULT:
        break;
    case SyncException::STORE_AMO_FAULT:
        break;
    case SyncException::LOAD_ADDR_MISALIGNED:
        break;
    case SyncException::STORE_ADDR_MISALIGNED:
        break;
    case SyncException::ILLEGAL_INST:
        break;
    default:
        Panic("unknown sync exception");
    }
    PanicOnSyncException();
}
} // namespace arch::trap

extern "C" void s_trap_handler()
{
    arch::trap::SupervisorTrap::GetInstance().OnTrap();
}
