/**
 * @file Trap.hh
 * @brief S-mode synchronous trap bring-up tests (read scause after sret)
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

#include "Trap/Trap.hh"

namespace arch::trap {

/** Deliberate sync exceptions; verify scause after handler returns via sret. */
class SyncTrapTest {
public:
    /** 0..6 */
    static constexpr std::size_t G_kOnlyCaseIndex= 2;

    explicit SyncTrapTest(SupervisorTrap &trap= SupervisorTrap::GetInstance()) noexcept;

    void RunAll() const;

private:
    static constexpr std::uintptr_t G_kFaultAddress= 0x200U;

    [[nodiscard]] static std::uint64_t readScause() noexcept;
    [[nodiscard]] static std::uint64_t readSepc() noexcept;
    [[nodiscard]] static std::uint64_t readStval() noexcept;
    [[nodiscard]] static std::uint64_t syncCauseCode(std::uint64_t scause) noexcept;
    [[nodiscard]] static bool isInterrupt(std::uint64_t scause) noexcept;

    static void triggerStoreAccessFault() noexcept;
    static void triggerLoadAccessFault() noexcept;
    static void triggerStoreMisaligned() noexcept;
    static void triggerLoadMisaligned() noexcept;
    static void triggerIllegalInst() noexcept;
    static void triggerBreakpoint() noexcept;
    static void triggerEcallS() noexcept;

    struct Case {
        std::string_view label_;
        std::optional<SyncException> expect_;
        void (*trigger_)() noexcept;
    };

    static constexpr Case G_kCases[] {
        {.label_="store access fault", .expect_=SyncException::STORE_AMO_FAULT, .trigger_=triggerStoreAccessFault},
        {.label_="load access fault", .expect_=SyncException::LOAD_ACCESS_FAULT, .trigger_=triggerLoadAccessFault},
        {.label_="store addr misaligned", .expect_=SyncException::STORE_ADDR_MISALIGNED, .trigger_=triggerStoreMisaligned},
        {.label_="load addr misaligned", .expect_=SyncException::LOAD_ADDR_MISALIGNED, .trigger_=triggerLoadMisaligned},
        {.label_="illegal instruction", .expect_=SyncException::ILLEGAL_INST, .trigger_=triggerIllegalInst},
        {.label_="breakpoint", .expect_=SyncException::BREAKPOINT, .trigger_=triggerBreakpoint},
        {.label_="ecall S-mode", .expect_=SyncException::ECALL_FROM_S_MODE, .trigger_=triggerEcallS},
    };

    void runOne(const Case &tc) const;

    SupervisorTrap &Trap_;
};

void RunSyncTests();

} // namespace arch::trap
