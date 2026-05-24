#include "CSR/CSRManager.hh"

#include <cassert>
#include <bit>
#include <cstdint>

namespace arch::csr {

[[nodiscard]] Manager &Manager::Instance() noexcept
{
    static Manager s_instance {};
    return s_instance;
}

void Manager::WriteReg(Reg csr, uint64_t value) noexcept
{
    switch(csr) {
    case Reg::SSTATUS:    writeCsr<Reg::SSTATUS>(value); break;
    case Reg::SEDELEG:    writeCsr<Reg::SEDELEG>(value); break;
    case Reg::SIDELEG:    writeCsr<Reg::SIDELEG>(value); break;
    case Reg::SIE:        writeCsr<Reg::SIE>(value); break;
    case Reg::STVEC:      writeCsr<Reg::STVEC>(value); break;
    case Reg::SCOUNTEREN: writeCsr<Reg::SCOUNTEREN>(value); break;
    case Reg::SSCRATCH:   writeCsr<Reg::SSCRATCH>(value); break;
    case Reg::SEPC:       writeCsr<Reg::SEPC>(value); break;
    case Reg::SCAUSE:     writeCsr<Reg::SCAUSE>(value); break;
    case Reg::STVAL:      writeCsr<Reg::STVAL>(value); break;
    case Reg::SIP:        writeCsr<Reg::SIP>(value); break;
    case Reg::SATP:       writeCsr<Reg::SATP>(value); break;
    case Reg::MSTATUS:    writeCsr<Reg::MSTATUS>(value); break;
    case Reg::MISA:       writeCsr<Reg::MISA>(value); break;
    case Reg::MEDELEG:    writeCsr<Reg::MEDELEG>(value); break;
    case Reg::MIDELEG:    writeCsr<Reg::MIDELEG>(value); break;
    case Reg::MIE:        writeCsr<Reg::MIE>(value); break;
    case Reg::MTVEC:      writeCsr<Reg::MTVEC>(value); break;
    case Reg::MCOUNTEREN: writeCsr<Reg::MCOUNTEREN>(value); break;
    case Reg::MSTATUSH:   writeCsr<Reg::MSTATUSH>(value); break;
    case Reg::MENVCFG:    writeCsr<Reg::MENVCFG>(value); break;
    case Reg::MSCRATCH:   writeCsr<Reg::MSCRATCH>(value); break;
    case Reg::MEPC:       writeCsr<Reg::MEPC>(value); break;
    case Reg::MCAUSE:     writeCsr<Reg::MCAUSE>(value); break;
    case Reg::MTVAL:      writeCsr<Reg::MTVAL>(value); break;
    case Reg::MIP:        writeCsr<Reg::MIP>(value); break;
    case Reg::MTINST:     writeCsr<Reg::MTINST>(value); break;
    case Reg::MTVAL2:     writeCsr<Reg::MTVAL2>(value); break;
    case Reg::PMPCFG0:    writeCsr<Reg::PMPCFG0>(value); break;
    case Reg::PMPADDR0:   writeCsr<Reg::PMPADDR0>(value); break;
    case Reg::STIMECMP:   writeCsr<Reg::STIMECMP>(value); break;
    default:
        assert(false && "Unsupported CSR write");
        break;
    }
}

uint64_t Manager::ReadReg(Reg csr) noexcept
{
    uint64_t value= 0;
    switch(csr) {
    case Reg::SSTATUS:    value= readCsr<Reg::SSTATUS>(); break;
    case Reg::SEDELEG:    value= readCsr<Reg::SEDELEG>(); break;
    case Reg::SIDELEG:    value= readCsr<Reg::SIDELEG>(); break;
    case Reg::SIE:        value= readCsr<Reg::SIE>(); break;
    case Reg::STVEC:      value= readCsr<Reg::STVEC>(); break;
    case Reg::SCOUNTEREN: value= readCsr<Reg::SCOUNTEREN>(); break;
    case Reg::SSCRATCH:   value= readCsr<Reg::SSCRATCH>(); break;
    case Reg::SEPC:       value= readCsr<Reg::SEPC>(); break;
    case Reg::SCAUSE:     value= readCsr<Reg::SCAUSE>(); break;
    case Reg::STVAL:      value= readCsr<Reg::STVAL>(); break;
    case Reg::SIP:        value= readCsr<Reg::SIP>(); break;
    case Reg::SATP:       value= readCsr<Reg::SATP>(); break;
    case Reg::MVENDORID:  value= readCsr<Reg::MVENDORID>(); break;
    case Reg::MARCHID:    value= readCsr<Reg::MARCHID>(); break;
    case Reg::MIMPID:     value= readCsr<Reg::MIMPID>(); break;
    case Reg::MHARTID:    value= readCsr<Reg::MHARTID>(); break;
    case Reg::MSTATUS:    value= readCsr<Reg::MSTATUS>(); break;
    case Reg::MISA:       value= readCsr<Reg::MISA>(); break;
    case Reg::MEDELEG:    value= readCsr<Reg::MEDELEG>(); break;
    case Reg::MIDELEG:    value= readCsr<Reg::MIDELEG>(); break;
    case Reg::MIE:        value= readCsr<Reg::MIE>(); break;
    case Reg::MTVEC:      value= readCsr<Reg::MTVEC>(); break;
    case Reg::MCOUNTEREN: value= readCsr<Reg::MCOUNTEREN>(); break;
    case Reg::MSTATUSH:   value= readCsr<Reg::MSTATUSH>(); break;
    case Reg::MENVCFG:    value= readCsr<Reg::MENVCFG>(); break;
    case Reg::MSCRATCH:   value= readCsr<Reg::MSCRATCH>(); break;
    case Reg::MEPC:       value= readCsr<Reg::MEPC>(); break;
    case Reg::MCAUSE:     value= readCsr<Reg::MCAUSE>(); break;
    case Reg::MTVAL:      value= readCsr<Reg::MTVAL>(); break;
    case Reg::MIP:        value= readCsr<Reg::MIP>(); break;
    case Reg::MTINST:     value= readCsr<Reg::MTINST>(); break;
    case Reg::MTVAL2:     value= readCsr<Reg::MTVAL2>(); break;
    case Reg::PMPCFG0:    value= readCsr<Reg::PMPCFG0>(); break;
    case Reg::PMPADDR0:   value= readCsr<Reg::PMPADDR0>(); break;
    case Reg::TIME:       value= readCsr<Reg::TIME>(); break;
    case Reg::TIMEH:      value= readCsr<Reg::TIMEH>(); break;
    case Reg::STIMECMP:   value= readCsr<Reg::STIMECMP>(); break;
    default:
        assert(false && "Unsupported CSR read");
        break;
    }
    return value;
}

void Manager::SetBits(Reg csr, uint64_t mask) noexcept
{
    this->WriteReg(csr, ReadReg(csr) | mask);
}

void Manager::ClearBits(Reg csr, uint64_t mask) noexcept
{
    this->WriteReg(csr, ReadReg(csr) & ~mask);
}

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
    writeCsr<Reg::STIMECMP>(readCsr<Reg::TIME>() + static_cast<uint64_t>(Timer::INTERVAL_TICKS));
}

bool Manager::IsFromSupervisorMode(uint64_t sstatus) const noexcept
{
    return (sstatus & static_cast<uint64_t>(SstatusFlag::SPP)) != 0;
}

bool Manager::IsSupervisorInterruptEnabled(uint64_t sstatus) const noexcept
{
    return (sstatus & static_cast<uint64_t>(SstatusFlag::SIE)) != 0;
}

void Manager::delegateIrqExceptionToSupervisorMode() noexcept
{
    // delegate all interrupts and exceptions to supervisor mode.
    this->WriteReg(Reg::MEDELEG, static_cast<uint64_t>(MedelegFlag::ALL));
    this->WriteReg(Reg::MIDELEG, static_cast<uint64_t>(MidelegFlag::ALL));
    this->SetBits(Reg::SIE, static_cast<uint64_t>(SieFlag::SEIE) | static_cast<uint64_t>(SieFlag::STIE));
}

void Manager::setupPmp() noexcept
{
    this->WriteReg(Reg::PMPADDR0, static_cast<uint64_t>(PmpFlag::ADDR0_ALL));
    this->WriteReg(Reg::PMPCFG0, static_cast<uint64_t>(PmpFlag::CFG0_NAPOT_RWX));
}

void Manager::initializeTimer() noexcept
{
    // enable the sstc extension (i.e. stimecmp).
    this->SetBits(Reg::MENVCFG, static_cast<uint64_t>(MenvcfgFlag::STCE));
    // allow supervisor to use stimecmp and time.
    this->SetBits(Reg::MCOUNTEREN, static_cast<uint64_t>(McounterenFlag::TIME));
    // ask for the very first timer interrupt.
    this->SetStimecmpIntervalTicks();
}

void Manager::StartSupervisorMode(void (*pEntry)()) noexcept
{
    // set M Previous Privilege mode to Supervisor, for mret.
    uint64_t mstatus= this->ReadReg(Reg::MSTATUS);
    mstatus&= ~static_cast<uint64_t>(MstatusMppFlag::MASK);
    mstatus|= static_cast<uint64_t>(MstatusMppFlag::S);
    // set M Exception Program Counter to entry, for mret.
    this->WriteReg(Reg::MEPC, std::bit_cast<uint64_t>(pEntry));

    this->WriteReg(Reg::MSTATUS, mstatus);

    // disable paging for now.
    this->WriteReg(Reg::SATP, 0);
    // delegate all interrupts and exceptions to supervisor mode.
    this->delegateIrqExceptionToSupervisorMode();
    // configure Physical Memory Protection to give supervisor mode
    // access to all of physical memory.
    this->setupPmp();
    // ask for clock interrupts.
    this->initializeTimer();

    // set tp to 0, for hart0.// keep each CPU's hartid in its tp register, for cpuid().
    uint64_t hartid= this->ReadMhartid();
    this->writeTp(hartid);
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

void Manager::EnableTimerInterrupt() noexcept
{
    this->SetBits(Reg::SIE, static_cast<uint64_t>(SieFlag::STIE));
}

void Manager::DisableTimerInterrupt() noexcept
{
    this->ClearBits(Reg::SIE, static_cast<uint64_t>(SieFlag::STIE));
}
} // namespace arch::csr
