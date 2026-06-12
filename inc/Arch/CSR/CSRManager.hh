/**
 * @file CSRManager.hh
 * @brief Machine / supervisor CSR access (xv6 start.c style bring-up)
 */
#pragma once

#include <cstdint>

namespace arch::csr {
class Manager {
private:
    enum class Reg : uint16_t {
        /* Supervisor trap setup */
        SSTATUS   = 0x100,
        SEDELEG   = 0x102,
        SIDELEG   = 0x103,
        SIE       = 0x104,
        STVEC     = 0x105,
        SCOUNTEREN= 0x106,
        /* Supervisor trap handling */
        SSCRATCH= 0x140,
        SEPC    = 0x141,
        SCAUSE  = 0x142,
        STVAL   = 0x143,
        SIP     = 0x144,
        SATP    = 0x180,
        /* Machine trap setup */
        MSTATUS   = 0x300,
        MISA      = 0x301,
        MEDELEG   = 0x302,
        MIDELEG   = 0x303,
        MIE       = 0x304,
        MTVEC     = 0x305,
        MCOUNTEREN= 0x306,
        MSTATUSH  = 0x310,
        MENVCFG   = 0x30A,
        /* Machine trap handling */
        MSCRATCH= 0x340,
        MEPC    = 0x341,
        MCAUSE  = 0x342,
        MTVAL   = 0x343,
        MIP     = 0x344,
        MTINST  = 0x34A,
        MTVAL2  = 0x34B,
        /* Machine information */
        MVENDORID= 0xf11,
        MARCHID  = 0xf12,
        MIMPID   = 0xf13,
        MHARTID  = 0xf14,
        /* PMP */
        PMPCFG0 = 0x3a0,
        PMPADDR0= 0x3b0,
        /* Time (S-mode / unprivileged) */
        TIME    = 0xc01,
        TIMEH   = 0xc81,
        STIMECMP= 0x14d,
    };

    /* mstatus MPP — previous privilege for mret (xv6 riscv.h) */
    /// mstatus MPP field values — previous privilege mode for mret instruction
    enum class MstatusMppFlag : uint16_t {
        MASK = 3 << 11,  ///< MPP field mask (bits 12:11)
        M    = 3 << 11,  ///< Previous mode was Machine mode
        S    = 1 << 11,  ///< Previous mode was Supervisor mode
        U    = 0 << 11,  ///< Previous mode was User mode
    };

    /// sstatus register flags — supervisor status register
    enum class SstatusFlag : uint16_t {
        SPP  = 1 << 8,  ///< Supervisor Previous Privilege mode
        SPIE = 1 << 5,  ///< Supervisor Previous Interrupt Enable
        UPIE = 1 << 4,  ///< User Previous Interrupt Enable
        SIE  = 1 << 1,  ///< Supervisor Interrupt Enable
        UIE  = 1 << 0,  ///< User Interrupt Enable
    };

    /// sie register flags — supervisor interrupt enable register
    enum class SieFlag : uint16_t {
        SEIE = 1 << 9,  ///< Supervisor External Interrupt Enable
        STIE = 1 << 5,  ///< Supervisor Timer Interrupt Enable
    };

    /// satp register flags — supervisor address translation and protection
    enum class SatpFlag : uint64_t {
        SV39 = 8ULL << 60,  ///< Sv39: 39-bit virtual addressing (3-level page table)
    };

    /// Machine-mode Exception Delegation Register flags (medeleg)
    /// Controls which exceptions are delegated from M-mode to S-mode
    enum class MedelegFlag : uint64_t {
        ALL   = 0xffffULL,  ///< Delegate all exceptions to S-mode

        // Instruction-related exceptions
        DIAME = 1ULL << 0,  ///< Delegate Instruction Address Misaligned Exception
        DIAFE = 1ULL << 1,  ///< Delegate Instruction Access Fault Exception
        DIIE  = 1ULL << 2,  ///< Delegate Illegal Instruction Exception
        DBE   = 1ULL << 3,  ///< Delegate Breakpoint Exception

        // Load/Store-related exceptions
        DLAME = 1ULL << 4,  ///< Delegate Load Address Misaligned Exception
        DLAFE = 1ULL << 5,  ///< Delegate Load Access Fault Exception
        DSAME = 1ULL << 6,  ///< Delegate Store/AMO Address Misaligned Exception
        DSAFE = 1ULL << 7,  ///< Delegate Store/AMO Access Fault Exception

        // Environment call exceptions
        DECFU = 1ULL << 8,  ///< Delegate eCall from U-mode Exception
        DECFS = 1ULL << 9,  ///< Delegate eCall from S-mode Exception

        // Page fault exceptions
        DIPF  = 1ULL << 12, ///< Delegate Instruction Page Fault Exception
        DLPF  = 1ULL << 13, ///< Delegate Load Page Fault Exception
        DSPFE = 1ULL << 15, ///< Delegate Store/AMO Page Fault Exception
    };

    /// Machine-mode Interrupt Delegation Register flags (mideleg)
    /// Controls which interrupts are delegated from M-mode to S-mode
    enum class MidelegFlag : uint64_t {
        ALL  = 0xffffULL,   ///< Delegate all interrupts to S-mode

        SSIP = 1ULL << 1,   ///< Delegate Supervisor Software Interrupt Pending
        STIP = 1ULL << 5,   ///< Delegate Supervisor Timer Interrupt Pending
        SEIP = 1ULL << 9,   ///< Delegate Supervisor External Interrupt Pending
    };

    /// Machine Environment Configuration Register flags (menvcfg)
    enum class MenvcfgFlag : uint64_t {
        STCE = 1ULL << 63,  ///< Supervisor Timer Compare Enable - enables stimecmp for S-mode
    };

    /// Machine Counter-Enable Register flags (mcounteren)
    /// Controls which performance counters are accessible from lower privilege modes
    enum class McounterenFlag : uint16_t {
        TIME = 2,  ///< Enable time counter access from S-mode/U-mode
    };

    /// Timer configuration constants
    enum class Timer : uint64_t {
        INTERVAL_TICKS = 1'000'000ULL,  ///< Timer interrupt interval in ticks (1 second at 1MHz)
    };

    /// Physical Memory Protection flags
    enum class PmpFlag : uint64_t {
        ADDR0_ALL     = 0x3fffffffffffffULL,  ///< PMP address0 covering all memory
        CFG0_NAPOT_RWX= 0xfULL,               ///< PMP cfg0: NAPOT mode with Read/Write/Execute permissions
    };

public:
    Manager(const Manager &) noexcept            = delete;
    Manager &operator= (const Manager &) noexcept= delete;
    Manager(Manager &&) noexcept                 = delete;
    Manager &operator= (Manager &&) noexcept     = delete;
    Manager() noexcept                           = default;
    ~Manager() noexcept                          = default;

    [[nodiscard]] static Manager &Instance() noexcept;

    void WriteReg(Reg csr, uint64_t value) noexcept;
    [[nodiscard]] uint64_t ReadReg(Reg csr) noexcept;
    void SetBits(Reg csr, uint64_t mask) noexcept;
    void ClearBits(Reg csr, uint64_t mask) noexcept;
    [[nodiscard]] uint64_t ReadSstatus() noexcept;
    [[nodiscard]] uint64_t ReadSepc() noexcept;
    [[nodiscard]] uint64_t ReadScause() noexcept;
    [[nodiscard]] uint64_t ReadStval() noexcept;
    [[nodiscard]] uint64_t ReadTime() noexcept;
    [[nodiscard]] uint64_t ReadMhartid() noexcept;
    void WriteSstatus(uint64_t value) noexcept;
    void WriteSepc(uint64_t value) noexcept;
    void WriteStvec(uint64_t value) noexcept;
    void SetStimecmpIntervalTicks() noexcept;
    [[nodiscard]] bool IsFromSupervisorMode(uint64_t sstatus) const noexcept;
    [[nodiscard]] bool IsSupervisorInterruptEnabled(uint64_t sstatus) const noexcept;

    /* mepc <- entry, tp <- hartid, mret into S-mode. */
    [[noreturn]] void StartSupervisorMode(void (*pEntry)()) noexcept;
    uint64_t Cpuid() noexcept;

    /** enable timer interrupt. */
    void EnableTimerInterrupt() noexcept;
    /** disable timer interrupt. */
    void DisableTimerInterrupt() noexcept;
private:
    void writeTp(uint64_t value) noexcept;
    void initializeTimer() noexcept;
    void setupPmp() noexcept;
    void delegateIrqExceptionToSupervisorMode() noexcept;

    template <Manager::Reg Csr>
    void writeCsr(uint64_t value) noexcept
    {
        __asm__ volatile("csrw %0, %1" : : "i"(static_cast<uint16_t>(Csr)), "r"(value) : "memory");
    }

    template <Manager::Reg Csr>
    [[nodiscard]]  uint64_t readCsr() noexcept
    {
        uint64_t value= 0;
        __asm__ volatile("csrr %0, %1" : "=r"(value) : "i"(static_cast<uint16_t>(Csr)) : "memory");
        return value;
    }
};

} // namespace arch::csr
