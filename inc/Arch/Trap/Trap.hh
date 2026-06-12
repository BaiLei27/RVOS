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

/** scause: bit63 = interrupt, bits[62:0] = code */
union Scause {
    uint64_t raw_= 0;
    struct {
        uint64_t code_     : 63;
        uint64_t interrupt_: 1;
    };
};

inline constexpr size_t G_K_MAX_HARTS= 8;

class SupervisorTrap {
public:
    static SupervisorTrap &GetInstance() noexcept;
    static SupervisorTrap &GetInstance(uint64_t hartid) noexcept;
    static SupervisorTrap &TrapForHart(uint64_t hartid) noexcept;

    void OnTrap() noexcept;
    static void Init();

    [[nodiscard]] std::string_view exceptionName(uint64_t code) const noexcept;
    [[noreturn]] void PanicOnSyncException() const noexcept;

private:
    void SetupVector() noexcept;
    uint64_t HandleInterrupt() noexcept;
    uint64_t HandleException() noexcept;
    void Panic(const char *pMsg) noexcept;

private:
    Scause Scause_ {};
    uint64_t HartId_= 0;
    uint64_t Epc_= 0;
    uint64_t Sstatus_= 0;
    uint64_t Stval_= 0;
};

} // namespace arch::trap
