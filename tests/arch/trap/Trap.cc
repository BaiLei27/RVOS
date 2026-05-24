/**
 * @file Trap.cc
 * @brief S-mode synchronous trap tests (scause / sepc / stval after trap)
 */

#include <cstdio>

#include "Trap/Trap.hh"
#include "Trap.hh"

namespace arch::trap {

std::uint64_t SyncTrapTest::readScause() noexcept
{
    std::uint64_t value= 0;
    __asm__ volatile("csrr %0, scause" : "=r"(value));
    return value;
}

std::uint64_t SyncTrapTest::readSepc() noexcept
{
    std::uint64_t value= 0;
    __asm__ volatile("csrr %0, sepc" : "=r"(value));
    return value;
}

std::uint64_t SyncTrapTest::readStval() noexcept
{
    std::uint64_t value= 0;
    __asm__ volatile("csrr %0, stval" : "=r"(value));
    return value;
}

std::uint64_t SyncTrapTest::syncCauseCode(std::uint64_t scause) noexcept
{
    return scause & 0x7FFF'FFFF'FFFF'FFFFULL;
}

bool SyncTrapTest::isInterrupt(std::uint64_t scause) noexcept
{
    return (scause >> 63) != 0;
}

SyncTrapTest::SyncTrapTest(SupervisorTrap &trap) noexcept
    : Trap_(trap)
{
}

void SyncTrapTest::triggerStoreAccessFault() noexcept
{
    *reinterpret_cast<volatile int *>(G_kFaultAddress)= 100;
}

void SyncTrapTest::triggerLoadAccessFault() noexcept
{
    (void)*reinterpret_cast<volatile int *>(G_kFaultAddress);
}

void SyncTrapTest::triggerStoreMisaligned() noexcept
{
    alignas(8) static std::uint8_t s_buffer[16] {};
    __asm__ volatile(
        "addi t0, %0, 2\n"
        "sw   zero, 0(t0)\n"
        :
        : "r"(s_buffer)
        : "memory", "t0");
}

void SyncTrapTest::triggerLoadMisaligned() noexcept
{
    alignas(8) static std::uint8_t s_buffer[16] {};
    std::uint64_t tmp= 0;
    __asm__ volatile(
        "addi t0, %0, 3\n"
        "ld   %1, 0(t0)\n"
        : "=r"(tmp)
        : "r"(s_buffer)
        : "memory", "t0");
}

void SyncTrapTest::triggerIllegalInst() noexcept
{
    __asm__ volatile(".word 0x00000000");
}

void SyncTrapTest::triggerBreakpoint() noexcept
{
    __asm__ volatile("ebreak");
}

void SyncTrapTest::triggerEcallS() noexcept
{
    __asm__ volatile("ecall");
}

void SyncTrapTest::runOne(const Case &tc) const
{
    tc.trigger_();

    const std::uint64_t SCAUSE= readScause();
    const std::uint64_t CODE  = syncCauseCode(SCAUSE);
    const std::uint64_t SEPC  = readSepc();
    const std::uint64_t STVAL = readStval();

    if(isInterrupt(SCAUSE)) {
        std::printf("[%s] FAIL: interrupt scause=0x%lx\n", tc.label_.data(), SCAUSE);
        return;
    }

    const bool ok= !tc.expect_.has_value()
                || static_cast<std::uint64_t>(*tc.expect_) == CODE;

    std::printf("[%s] %s scause=%lu (%s) sepc=0x%lx stval=0x%lx\n",
                tc.label_.data(),
                ok ? "OK" : "FAIL",
                CODE,
                Trap_.exceptionName(CODE).data(),
                SEPC,
                STVAL);

    if(tc.expect_.has_value() && !ok) {
        std::printf("  -> expected scause=%lu (%s)\n",
                    static_cast<std::uint64_t>(*tc.expect_),
                    Trap_.exceptionName(static_cast<std::uint64_t>(*tc.expect_)).data());
    }
}

void SyncTrapTest::RunAll() const
{
    static_assert(G_kOnlyCaseIndex < std::size(G_kCases));

    std::printf("=== S-mode trap test (case %u) ===\n",
                static_cast<unsigned>(G_kOnlyCaseIndex));
    runOne(G_kCases[G_kOnlyCaseIndex]);
    std::printf("=== done ===\n");
}

void RunSyncTests()
{
    SyncTrapTest trapTest;
    trapTest.RunAll();
}

} // namespace arch::trap
