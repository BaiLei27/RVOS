#include "CSR/CSRManager.hh"

#include <bit>
#include <cstdint>

namespace arch::csr {

uint64_t Manager::ReadSstatus() noexcept
{
    return readCsr<Reg::SSTATUS>();
}

uint64_t Manager::ReadSepc() noexcept
{
    return readCsr<Reg::SEPC>();
}

uint64_t Manager::ReadScause() noexcept
{
    return readCsr<Reg::SCAUSE>();
}

uint64_t Manager::ReadStval() noexcept
{
    return readCsr<Reg::STVAL>();
}

uint64_t Manager::ReadTime() noexcept
{
    return readCsr<Reg::TIME>();
}

uint64_t Manager::ReadMhartid() noexcept
{
    return readCsr<Reg::MHARTID>();
}

void Manager::WriteSstatus(uint64_t value) noexcept
{
    writeCsr<Reg::SSTATUS>(value);
}

void Manager::WriteSepc(uint64_t value) noexcept
{
    writeCsr<Reg::SEPC>(value);
}

void Manager::WriteStvec(uint64_t value) noexcept
{
    writeCsr<Reg::STVEC>(value);
}

void Manager::SetStimecmpIntervalTicks() noexcept
{
    writeCsr<Reg::STIMECMP>(ReadTime() + static_cast<uint64_t>(Timer::INTERVAL_TICKS));
}

bool Manager::IsFromSupervisorMode(uint64_t sstatus) noexcept
{
    return (sstatus & static_cast<uint64_t>(SstatusFlag::SPP)) != 0;
}

void Manager::delegateIrqExceptionToSupervisorMode() noexcept
{
    writeCsr<Reg::MEDELEG>(static_cast<uint64_t>(MedelegFlag::ALL));
    writeCsr<Reg::MIDELEG>(static_cast<uint64_t>(MidelegFlag::ALL));
    setCsrBits<Reg::SIE>(static_cast<uint64_t>(SieFlag::SEIE) | static_cast<uint64_t>(SieFlag::STIE));
}

void Manager::setupPmp() noexcept
{
    writeCsr<Reg::PMPADDR0>(static_cast<uint64_t>(PmpFlag::ADDR0_ALL));
    writeCsr<Reg::PMPCFG0>(static_cast<uint64_t>(PmpFlag::CFG0_NAPOT_RWX));
}

void Manager::initializeTimer() noexcept
{
    setCsrBits<Reg::MENVCFG>(static_cast<uint64_t>(MenvcfgFlag::STCE));
    setCsrBits<Reg::MCOUNTEREN>(static_cast<uint64_t>(McounterenFlag::TIME));
    SetStimecmpIntervalTicks();
}

void Manager::StartSupervisorMode(void (*pEntry)()) noexcept
{
    // set M Previous Privilege mode to Supervisor, for mret.
    uint64_t mstatus= readCsr<Reg::MSTATUS>();
    mstatus&= ~static_cast<uint64_t>(MstatusMppFlag::MASK);
    mstatus|= static_cast<uint64_t>(MstatusMppFlag::S);
    // set M Exception Program Counter to entry, for mret.
    writeCsr<Reg::MEPC>(std::bit_cast<uint64_t>(pEntry));

    writeCsr<Reg::MSTATUS>(mstatus);

    // disable paging for now.
    writeCsr<Reg::SATP>(0);
    // delegate all interrupts and exceptions to supervisor mode.
    delegateIrqExceptionToSupervisorMode();
    // configure Physical Memory Protection to give supervisor mode
    // access to all of physical memory.
    setupPmp();
    // ask for clock interrupts.
    initializeTimer();

    // keep each CPU's hartid in its tp register, for cpuid().
    writeTp(ReadMhartid());
    // switch to supervisor mode and jump to pEntry().
    __asm__ volatile("mret");
    __builtin_unreachable();
}

uint64_t Manager::Cpuid() noexcept
{
    uint64_t hartid= 0;
    __asm__ volatile("mv %0, tp" : "=r"(hartid));
    return hartid;
}

void Manager::writeTp(uint64_t value) noexcept
{
    __asm__ volatile("mv tp, %0" : : "r"(value));
}

void Manager::EnableInterrupts() noexcept
{
    setCsrBits<Reg::SSTATUS>(static_cast<uint64_t>(SstatusFlag::SIE));
}

void Manager::DisableInterrupts() noexcept
{
    clearCsrBits<Reg::SSTATUS>(static_cast<uint64_t>(SstatusFlag::SIE));
}

void Manager::EnableTimerInterrupt() noexcept
{
    setCsrBits<Reg::SIE>(static_cast<uint64_t>(SieFlag::STIE));
}

void Manager::DisableTimerInterrupt() noexcept
{
    clearCsrBits<Reg::SIE>(static_cast<uint64_t>(SieFlag::STIE));
}
} // namespace arch::csr
