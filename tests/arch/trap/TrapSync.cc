/**
 * @file Trap.cc
 * @brief S-mode synchronous trap tests (scause / sepc / stval after trap)
 */

#include <print>
#include <string_view>
#include <array>

#include "Trap/Trap.hh"
#include "CSR/CSRManager.hh"

namespace arch::trap {
int g_started= 0;

/** Deliberate sync exceptions; verify scause after handler returns via sret. */
class SyncTrapTest {
public:
    static void RunAll();

private:
    static constexpr uintptr_t G_kFaultAddress= 0x200U;

    [[nodiscard]] static uint64_t readScause() noexcept;
    [[nodiscard]] static uint64_t readSepc() noexcept;
    [[nodiscard]] static uint64_t readStval() noexcept;
    [[nodiscard]] static uint64_t syncCauseCode(uint64_t scause) noexcept;
    [[nodiscard]] static bool isInterrupt(uint64_t scause) noexcept;

    static void triggerStoreAccessFault() noexcept;
    static void triggerLoadAccessFault() noexcept;
    static void triggerStoreMisaligned() noexcept;
    static void triggerLoadMisaligned() noexcept;
    static void triggerIllegalInst() noexcept;
    static void triggerBreakpoint() noexcept;
    static void triggerEcall() noexcept;

    struct Case {
        std::string_view label_;
        std::optional<SyncException> expect_;
        void (*trigger_)() noexcept;
    };

    static constexpr std::array<Case, 7> G_kCases {
        {
         { .label_= "store access fault", .expect_= SyncException::STORE_AMO_FAULT, .trigger_= triggerStoreAccessFault },
         { .label_= "load access fault", .expect_= SyncException::LOAD_ACCESS_FAULT, .trigger_= triggerLoadAccessFault },
         { .label_= "store addr misaligned", .expect_= SyncException::STORE_ADDR_MISALIGNED, .trigger_= triggerStoreMisaligned },
         { .label_= "load addr misaligned", .expect_= SyncException::LOAD_ADDR_MISALIGNED, .trigger_= triggerLoadMisaligned },
         { .label_= "illegal instruction", .expect_= SyncException::ILLEGAL_INST, .trigger_= triggerIllegalInst },
         { .label_= "breakpoint", .expect_= SyncException::BREAKPOINT, .trigger_= triggerBreakpoint },
         { .label_= "ecall S-mode", .expect_= SyncException::ECALL_FROM_S_MODE, .trigger_= triggerEcall },
         }
    }; // note: constexpr requires all members to be literal types

    static void runOne(const Case &tc);
};

void RunSyncTests();

uint64_t SyncTrapTest::readScause() noexcept
{
    uint64_t value= 0;
    __asm__ volatile("csrr %0, scause" : "=r"(value));
    return value;
}

uint64_t SyncTrapTest::readSepc() noexcept
{
    uint64_t value= 0;
    __asm__ volatile("csrr %0, sepc" : "=r"(value));
    return value;
}

uint64_t SyncTrapTest::readStval() noexcept
{
    uint64_t value= 0;
    __asm__ volatile("csrr %0, stval" : "=r"(value));
    return value;
}

uint64_t SyncTrapTest::syncCauseCode(uint64_t scause) noexcept
{
    return scause & 0x7FFF'FFFF'FFFF'FFFFULL;
}

bool SyncTrapTest::isInterrupt(uint64_t scause) noexcept
{
    return (scause >> 63) != 0;
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
    uint64_t tmp= 0;
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

void SyncTrapTest::triggerEcall() noexcept
{
    __asm__ volatile("ecall");
}

void SyncTrapTest::runOne(const Case &tc)
{
    tc.trigger_();

    const uint64_t SCAUSE= readScause();
    const uint64_t CODE  = syncCauseCode(SCAUSE);
    const uint64_t SEPC  = readSepc();
    const uint64_t STVAL = readStval();

    if(isInterrupt(SCAUSE)) {
        std::println("[{}] FAIL: interrupt scause=0x{:x}", tc.label_, SCAUSE);
        return;
    }

    const bool OK= !tc.expect_.has_value()
                || static_cast<uint64_t>(*tc.expect_) == CODE;

    std::println("[{}] {} scause={} ({}) sepc=0x{:x} stval=0x{:x}",
                 tc.label_,                                       // string_view safe, no null-term risk
                 OK ? "OK" : "FAIL",                              // const char* literals are fine
                 CODE,                                            // decimal integer
                 arch::trap::SupervisorTrap::ExceptionName(CODE), // returns string_view, safe
                 SEPC,                                            // hex with 0x prefix
                 STVAL);                                          // hex with 0x prefix

    if(tc.expect_.has_value() && !OK) {
        auto expected= static_cast<uint64_t>(*tc.expect_); // extract once, avoid repeated cast
        std::println("  -> expected scause={} ({})",
                     expected,                                             // decimal integer
                     arch::trap::SupervisorTrap::ExceptionName(expected)); // string_view safe
    }
}

void SyncTrapTest::RunAll()
{
    for(int i= 0; const auto &e: G_kCases) {
        std::println("=== S-mode trap test (case {}) ===", i);
        runOne(e);
        ++i;
    }
    // runOne(G_kCases[2]);
    std::println("=== done ===");
}

} // namespace arch::trap

int main()
{
    using namespace arch; // NOLINT

    csr::Manager::Instance().StartSupervisorMode([] {
        if(csr::Manager::Cpuid() == 0) {
            trap::SupervisorTrap::Init();
            std::println("Hello, RVOS SupervisorTrap");
            trap::SyncTrapTest::RunAll();
            trap::g_started= 1;
        } else {
            while(trap::g_started == 0)
                ;
            trap::SupervisorTrap::Init();
            // std::println("hart {} starting...", arch::csr::Manager::Instance().Cpuid());
        }
    });
    return 0;
}
