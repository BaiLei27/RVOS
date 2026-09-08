/**
 * @file Trap.hh
 * @brief Supervisor-mode trap (S-mode, vectored)
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace arch::trap {

enum class SyncException : uint8_t {
    INST_ADDR_MISALIGNED = 0,
    INST_ACCESS_FAULT    = 1,
    ILLEGAL_INST         = 2,
    BREAKPOINT           = 3,
    LOAD_ADDR_MISALIGNED = 4,
    LOAD_ACCESS_FAULT    = 5,
    STORE_ADDR_MISALIGNED= 6,
    STORE_AMO_FAULT      = 7,
    ECALL_FROM_U_MODE    = 8,
    ECALL_FROM_S_MODE    = 9,
    ECALL_FROM_M_MODE    = 11,
};

enum class InterruptCause : uint8_t {
    SUPERVISOR_SOFTWARE= 1,
    SUPERVISOR_TIMER   = 5,
    SUPERVISOR_EXTERNAL= 9,
};

class SupervisorTrap {
public:
    static SupervisorTrap &GetInstance() noexcept;
    static SupervisorTrap &GetInstance(uint64_t hartid) noexcept;
    static SupervisorTrap &TrapForHart(uint64_t hartid) noexcept;

    void OnTrap() noexcept;
    static void Init();

    [[nodiscard]] static std::string_view ExceptionName(uint64_t code) noexcept;
    [[noreturn]] static void PanicOnSyncException() noexcept;

private:
    static void setupVector() noexcept;
    uint64_t handleInterrupt() noexcept;
    uint64_t handleException() noexcept;
    [[noreturn]] static void panic(const char *pMsg) noexcept;

private:
    /** scause: bit63 = interrupt, bits[62:0] = code */
    union Scause {
        uint64_t raw_= 0;

        struct {
            uint64_t code_     : 63;
            uint64_t interrupt_: 1;
        };
    };

    static constexpr size_t G_kMaxHarts= 8;

    Scause Scause_ {};
    uint64_t HartId_ = 0;
    uint64_t Epc_    = 0;
    uint64_t Sstatus_= 0;
    uint64_t Stval_  = 0;
};

} // namespace arch::trap
