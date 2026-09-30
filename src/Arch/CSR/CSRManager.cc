#include "CSR/CSRManager.hh"

#include <cassert>
#include <bit>
#include <cstdint>

namespace arch::csr {

void Manager::WriteReg(Reg csr, uint64_t value) noexcept
{
    switch(csr) {
    case Reg::SSTATUS:    writeCSR<Reg::SSTATUS>(value); break;
    case Reg::SEDELEG:    writeCSR<Reg::SEDELEG>(value); break;
    case Reg::SIDELEG:    writeCSR<Reg::SIDELEG>(value); break;
    case Reg::SIE:        writeCSR<Reg::SIE>(value); break;
    case Reg::STVEC:      writeCSR<Reg::STVEC>(value); break;
    case Reg::SCOUNTEREN: writeCSR<Reg::SCOUNTEREN>(value); break;
    case Reg::SSCRATCH:   writeCSR<Reg::SSCRATCH>(value); break;
    case Reg::SEPC:       writeCSR<Reg::SEPC>(value); break;
    case Reg::SCAUSE:     writeCSR<Reg::SCAUSE>(value); break;
    case Reg::STVAL:      writeCSR<Reg::STVAL>(value); break;
    case Reg::SIP:        writeCSR<Reg::SIP>(value); break;
    case Reg::SATP:       writeCSR<Reg::SATP>(value); break;
    case Reg::MSTATUS:    writeCSR<Reg::MSTATUS>(value); break;
    case Reg::MISA:       writeCSR<Reg::MISA>(value); break;
    case Reg::MEDELEG:    writeCSR<Reg::MEDELEG>(value); break;
    case Reg::MIDELEG:    writeCSR<Reg::MIDELEG>(value); break;
    case Reg::MIE:        writeCSR<Reg::MIE>(value); break;
    case Reg::MTVEC:      writeCSR<Reg::MTVEC>(value); break;
    case Reg::MCOUNTEREN: writeCSR<Reg::MCOUNTEREN>(value); break;
    case Reg::MSTATUSH:   writeCSR<Reg::MSTATUSH>(value); break;
    case Reg::MENVCFG:    writeCSR<Reg::MENVCFG>(value); break;
    case Reg::MSCRATCH:   writeCSR<Reg::MSCRATCH>(value); break;
    case Reg::MEPC:       writeCSR<Reg::MEPC>(value); break;
    case Reg::MCAUSE:     writeCSR<Reg::MCAUSE>(value); break;
    case Reg::MTVAL:      writeCSR<Reg::MTVAL>(value); break;
    case Reg::MIP:        writeCSR<Reg::MIP>(value); break;
    case Reg::MTINST:     writeCSR<Reg::MTINST>(value); break;
    case Reg::MTVAL2:     writeCSR<Reg::MTVAL2>(value); break;
    case Reg::PMPCFG0:    writeCSR<Reg::PMPCFG0>(value); break;
    case Reg::PMPADDR0:   writeCSR<Reg::PMPADDR0>(value); break;
    case Reg::STIMECMP:   writeCSR<Reg::STIMECMP>(value); break;
    default:
        assert(false && "Unsupported CSR write");
        break;
    }
}

uint64_t Manager::ReadReg(Reg csr) noexcept
{
    uint64_t value= 0;
    switch(csr) {
    case Reg::SSTATUS:    value= readCSR<Reg::SSTATUS>(); break;
    case Reg::SEDELEG:    value= readCSR<Reg::SEDELEG>(); break;
    case Reg::SIDELEG:    value= readCSR<Reg::SIDELEG>(); break;
    case Reg::SIE:        value= readCSR<Reg::SIE>(); break;
    case Reg::STVEC:      value= readCSR<Reg::STVEC>(); break;
    case Reg::SCOUNTEREN: value= readCSR<Reg::SCOUNTEREN>(); break;
    case Reg::SSCRATCH:   value= readCSR<Reg::SSCRATCH>(); break;
    case Reg::SEPC:       value= readCSR<Reg::SEPC>(); break;
    case Reg::SCAUSE:     value= readCSR<Reg::SCAUSE>(); break;
    case Reg::STVAL:      value= readCSR<Reg::STVAL>(); break;
    case Reg::SIP:        value= readCSR<Reg::SIP>(); break;
    case Reg::SATP:       value= readCSR<Reg::SATP>(); break;
    case Reg::MVENDORID:  value= readCSR<Reg::MVENDORID>(); break;
    case Reg::MARCHID:    value= readCSR<Reg::MARCHID>(); break;
    case Reg::MIMPID:     value= readCSR<Reg::MIMPID>(); break;
    case Reg::MHARTID:    value= readCSR<Reg::MHARTID>(); break;
    case Reg::MSTATUS:    value= readCSR<Reg::MSTATUS>(); break;
    case Reg::MISA:       value= readCSR<Reg::MISA>(); break;
    case Reg::MEDELEG:    value= readCSR<Reg::MEDELEG>(); break;
    case Reg::MIDELEG:    value= readCSR<Reg::MIDELEG>(); break;
    case Reg::MIE:        value= readCSR<Reg::MIE>(); break;
    case Reg::MTVEC:      value= readCSR<Reg::MTVEC>(); break;
    case Reg::MCOUNTEREN: value= readCSR<Reg::MCOUNTEREN>(); break;
    case Reg::MSTATUSH:   value= readCSR<Reg::MSTATUSH>(); break;
    case Reg::MENVCFG:    value= readCSR<Reg::MENVCFG>(); break;
    case Reg::MSCRATCH:   value= readCSR<Reg::MSCRATCH>(); break;
    case Reg::MEPC:       value= readCSR<Reg::MEPC>(); break;
    case Reg::MCAUSE:     value= readCSR<Reg::MCAUSE>(); break;
    case Reg::MTVAL:      value= readCSR<Reg::MTVAL>(); break;
    case Reg::MIP:        value= readCSR<Reg::MIP>(); break;
    case Reg::MTINST:     value= readCSR<Reg::MTINST>(); break;
    case Reg::MTVAL2:     value= readCSR<Reg::MTVAL2>(); break;
    case Reg::PMPCFG0:    value= readCSR<Reg::PMPCFG0>(); break;
    case Reg::PMPADDR0:   value= readCSR<Reg::PMPADDR0>(); break;
    case Reg::TIME:       value= readCSR<Reg::TIME>(); break;
    case Reg::TIMEH:      value= readCSR<Reg::TIMEH>(); break;
    case Reg::STIMECMP:   value= readCSR<Reg::STIMECMP>(); break;
    default:
        assert(false && "Unsupported CSR read");
        break;
    }
    return value;
}

template <Reg CSR>
void Manager::writeCSR(uint64_t value) noexcept
{
    __asm__ volatile("csrw %0, %1" : : "i"(static_cast<uint16_t>(CSR)), "r"(value) : "memory");
}

template <Reg CSR>
uint64_t Manager::readCSR() noexcept
{
    uint64_t value= 0;
    __asm__ volatile("csrr %0, %1" : "=r"(value) : "i"(static_cast<uint16_t>(CSR)) : "memory");
    return value;
}

template <Reg CSR>
void Manager::setCSRBits(uint64_t mask) noexcept
{
    __asm__ volatile("csrrs zero, %0, %1" : : "i"(static_cast<uint16_t>(CSR)), "r"(mask) : "memory");
}

template <Reg CSR>
void Manager::clearCSRBits(uint64_t mask) noexcept
{
    __asm__ volatile("csrrc zero, %0, %1" : : "i"(static_cast<uint16_t>(CSR)), "r"(mask) : "memory");
}

uint64_t Manager::ReadSstatus() noexcept
{
    return readCSR<Reg::SSTATUS>();
}

uint64_t Manager::ReadSepc() noexcept
{
    return readCSR<Reg::SEPC>();
}

uint64_t Manager::ReadScause() noexcept
{
    return readCSR<Reg::SCAUSE>();
}

uint64_t Manager::ReadStval() noexcept
{
    return readCSR<Reg::STVAL>();
}

uint64_t Manager::ReadTime() noexcept
{
    return readCSR<Reg::TIME>();
}

uint64_t Manager::ReadMhartid() noexcept
{
    return readCSR<Reg::MHARTID>();
}

void Manager::WriteSstatus(uint64_t value) noexcept
{
    writeCSR<Reg::SSTATUS>(value);
}

void Manager::WriteSepc(uint64_t value) noexcept
{
    writeCSR<Reg::SEPC>(value);
}

void Manager::WriteStvec(uint64_t value) noexcept
{
    writeCSR<Reg::STVEC>(value);
}

void Manager::SetStimecmpIntervalTicks() noexcept
{
    writeCSR<Reg::STIMECMP>(ReadTime() + static_cast<uint64_t>(Timer::INTERVAL_TICKS));
}

bool Manager::IsFromSupervisorMode(uint64_t sstatus) noexcept
{
    return (sstatus & static_cast<uint64_t>(SstatusFlag::SPP)) != 0;
}

void Manager::delegateIrqExceptionToSupervisorMode() noexcept
{
    writeCSR<Reg::MEDELEG>(static_cast<uint64_t>(MedelegFlag::ALL));
    writeCSR<Reg::MIDELEG>(static_cast<uint64_t>(MidelegFlag::ALL));
    setCSRBits<Reg::SIE>(static_cast<uint64_t>(SieFlag::SEIE) | static_cast<uint64_t>(SieFlag::STIE));
}

void Manager::setupPmp() noexcept
{
    writeCSR<Reg::PMPADDR0>(static_cast<uint64_t>(PmpFlag::ADDR0_ALL));
    writeCSR<Reg::PMPCFG0>(static_cast<uint64_t>(PmpFlag::CFG0_NAPOT_RWX));
}

void Manager::initializeTimer() noexcept
{
    setCSRBits<Reg::MENVCFG>(static_cast<uint64_t>(MenvcfgFlag::STCE));
    setCSRBits<Reg::MCOUNTEREN>(static_cast<uint64_t>(McounterenFlag::TIME));
    SetStimecmpIntervalTicks();
}

void Manager::StartSupervisorMode(void (*pEntry)()) noexcept
{
    // set M Previous Privilege mode to Supervisor, for mret.
    uint64_t mstatus= readCSR<Reg::MSTATUS>();
    mstatus&= ~static_cast<uint64_t>(MstatusMppFlag::MASK);
    mstatus|= static_cast<uint64_t>(MstatusMppFlag::S);
    // set M Exception Program Counter to entry, for mret.
    writeCSR<Reg::MEPC>(std::bit_cast<uint64_t>(pEntry));

    writeCSR<Reg::MSTATUS>(mstatus);

    // disable paging for now.
    writeCSR<Reg::SATP>(0);
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
    setCSRBits<Reg::SSTATUS>(static_cast<uint64_t>(SstatusFlag::SIE));
}

void Manager::DisableInterrupts() noexcept
{
    clearCSRBits<Reg::SSTATUS>(static_cast<uint64_t>(SstatusFlag::SIE));
}

void Manager::EnableTimerInterrupt() noexcept
{
    setCSRBits<Reg::SIE>(static_cast<uint64_t>(SieFlag::STIE));
}

void Manager::DisableTimerInterrupt() noexcept
{
    clearCSRBits<Reg::SIE>(static_cast<uint64_t>(SieFlag::STIE));
}
} // namespace arch::csr
