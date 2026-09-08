/**
 * @file Trap.cc
 * @brief Supervisor-mode trap dispatch
 */

#include <bit>
#include <cstdint>
#include <string_view>

#include "CSR/CSRManager.hh"
#include "Trap/Trap.hh"

extern "C" void s_trap_vector_base(void);

namespace arch::trap {
[[noreturn]] void SupervisorTrap::PanicOnSyncException() noexcept
{
    /*
    printf("PANIC hart%lu: sync trap code=%lu (%s) sepc=0x%lx stval=0x%lx\n",
           HartId_,
           Scause_.code_,
           ExceptionName(Scause_.code_).data(),
           Epc_,
           Stval_);
    */
    while(true) {
        __asm__ volatile("wfi");
    }
}

std::string_view SupervisorTrap::ExceptionName(uint64_t code) noexcept
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
    static SupervisorTrap s_traps[G_kMaxHarts] {};

    if(hartid >= G_kMaxHarts) {
        hartid= 0;
    }
    return s_traps[hartid];
}

SupervisorTrap &SupervisorTrap::GetInstance() noexcept
{
    return GetInstance(csr::Manager::Cpuid());
}

SupervisorTrap &SupervisorTrap::GetInstance(uint64_t hartid) noexcept
{
    return TrapForHart(hartid);
}

void SupervisorTrap::setupVector() noexcept
{
    csr::Manager::Instance().WriteStvec(std::bit_cast<uint64_t>(&s_trap_vector_base) | 1U);
}

[[noreturn]] void SupervisorTrap::panic(const char * /*pMsg*/) noexcept
{
    // printf("Panic: %s\n", pMsg);
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
    HartId_     = csr::Manager::Cpuid();

    if(!csr::Manager::IsFromSupervisorMode(Sstatus_)) {
        panic("not from supervisor mode");
    }
    if(csr::Manager::IsSupervisorInterruptEnabled(Sstatus_)) {
        panic("interrupts enabled");
    }

    if(Scause_.interrupt_ != 0U) {
        Epc_= handleInterrupt();
    } else {
        Epc_= handleException();
    }

    csr.WriteSstatus(Sstatus_);
    csr.WriteSepc(Epc_);
}

void SupervisorTrap::Init()
{
    setupVector();
}

uint64_t SupervisorTrap::handleInterrupt() noexcept
{
    switch(static_cast<InterruptCause>(Scause_.code_)) {
    case InterruptCause::SUPERVISOR_SOFTWARE:
    case InterruptCause::SUPERVISOR_EXTERNAL: break; // printf("hart%lu interrupt: supervisor software/external\n", HartId_);
    case InterruptCause::SUPERVISOR_TIMER:    {
        // printf("hart%lu interrupt: supervisor timer\n", HartId_);
        auto &csr= csr::Manager::Instance();
        csr.SetStimecmpIntervalTicks();
        break;
    }
    default:
        // printf("hart%lu interrupt: code=%lu\n", HartId_, Scause_.code_);
        break;
    }
    return Epc_;
}

uint64_t SupervisorTrap::handleException() noexcept
{
    switch(static_cast<SyncException>(Scause_.code_)) {
    case SyncException::BREAKPOINT:
    case SyncException::ECALL_FROM_S_MODE: return Epc_ + 4U; // printf("hart%lu breakpoint/ecall at 0x%lx\n", HartId_, Epc_);
    case SyncException::LOAD_ACCESS_FAULT:
    case SyncException::STORE_AMO_FAULT:
    case SyncException::LOAD_ADDR_MISALIGNED:
    case SyncException::STORE_ADDR_MISALIGNED:
    case SyncException::ILLEGAL_INST:
        break;
    default:
        panic("unknown sync exception");
    }
    //return Epc_ + 4U; //no hang on
    PanicOnSyncException();
}
} // namespace arch::trap

extern "C" void s_trap_handler()
{
    arch::trap::SupervisorTrap::GetInstance().OnTrap();
}
