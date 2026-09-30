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

namespace {
void (*g_tickHandler)() noexcept = nullptr;
} // namespace

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

SupervisorTrap &SupervisorTrap::GetInstance() noexcept
{
    return GetInstance(csr::Manager::Cpuid());
}

SupervisorTrap &SupervisorTrap::GetInstance(uint64_t hartid) noexcept
{
    static SupervisorTrap s_traps[G_kMaxHarts] {};
    return s_traps[hartid < G_kMaxHarts ? hartid : 0];
}

void SupervisorTrap::SetTickHandler(void (*pHandler)() noexcept) noexcept
{
    g_tickHandler= pHandler;
}

void SupervisorTrap::setupVector() noexcept
{
    csr::Manager::Instance().WriteStvec(std::bit_cast<uint64_t>(&s_trap_vector_base));
}

[[noreturn]] void SupervisorTrap::panic(const char * /*pMsg*/) noexcept
{
    while(true) {
        __asm__ volatile("wfi");
    }
}

void SupervisorTrap::OnTrap() noexcept
{
    auto &csr   = csr::Manager::Instance();
    Scause_.raw_= csr.ReadScause();
    Epc_        = csr.ReadSepc();
    Stval_      = csr.ReadStval();
    Sstatus_    = csr.ReadSstatus();
    HartId_     = csr::Manager::Cpuid();

    if(!csr::Manager::IsFromSupervisorMode(Sstatus_)) {
        panic("not from supervisor mode");
    }

    const uint64_t NewEpc= (Scause_.interrupt_ != 0U) ? handleInterrupt(csr) : handleException();
    if(NewEpc != Epc_) {
        csr.WriteSepc(NewEpc);
    }
}

void SupervisorTrap::Init()
{
    setupVector();
}

uint64_t SupervisorTrap::handleInterrupt(csr::Manager &csr) noexcept
{
    switch(static_cast<InterruptCause>(Scause_.code_)) {
    case InterruptCause::SUPERVISOR_SOFTWARE:
    case InterruptCause::SUPERVISOR_EXTERNAL:
        break;
    case InterruptCause::SUPERVISOR_TIMER: {
        csr.SetStimecmpIntervalTicks();
        if(g_tickHandler != nullptr) {
            g_tickHandler();
        }
        break;
    }
    default:
        break;
    }
    return Epc_;
}

uint64_t SupervisorTrap::handleException() noexcept
{
    switch(static_cast<SyncException>(Scause_.code_)) {
    case SyncException::BREAKPOINT:
    case SyncException::ECALL_FROM_S_MODE:
        return Epc_ + 4U;
    default:
        break;
    }
    panic("unknown sync exception");
}

} // namespace arch::trap

extern "C" void s_trap_handler()
{
    arch::trap::SupervisorTrap::GetInstance().OnTrap();
}
