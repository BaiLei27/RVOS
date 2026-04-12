#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "Peripheral.hh"

// #include "type.hh"
/*
 * MemoryMap
 * see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[]
 * 0x00001000 -- boot ROM, provided by qemu
 * 0x02000000 -- CLINT
 * 0x0C000000 -- PLIC
 * 0x10000000 -- UART0
 * 0x10001000 -- virtio disk
 * 0x80000000 -- boot ROM jumps here in machine mode, where we load our kernel
 */

/* This machine puts UART registers here in physical memory. */
inline constexpr uint64_t G_UART0_BASE= 0x10000000UL;
inline constexpr uint64_t G_UART_RHR  = 0; // Receive Holding Register (read mode)
inline constexpr uint64_t G_UART_THR  = 0; // Transmit Holding Register (write mode)
inline constexpr uint64_t G_UART_DLL  = 0; // LSB of Divisor Latch (write mode)
inline constexpr uint64_t G_UART_IER  = 1; // Interrupt Enable Register (write mode)
inline constexpr uint64_t G_UART_DLM  = 1; // MSB of Divisor Latch (write mode)
inline constexpr uint64_t G_UART_FCR  = 2; // FIFO Control Register (write mode)
inline constexpr uint64_t G_UART_ISR  = 2; // Interrupt Status Register (read mode)
inline constexpr uint64_t G_UART_LCR  = 3; // Line Control Register
inline constexpr uint64_t G_UART_MCR  = 4; // Modem Control Register
inline constexpr uint64_t G_UART_LSR  = 5; // Line Status Register
inline constexpr uint64_t G_UART_MSR  = 6; // Modem Status Register
inline constexpr uint64_t G_UART_SPR  = 7; // ScratchPad Register

namespace RVOSHal {

class Uart: public IPeripheral {
public:
    explicit Uart(uint64_t baseAddress= G_UART0_BASE);
    using IPeripheral::IPeripheral;

    PeripheralType GetType() const override;

    HalStatus Init() override;

    HalStatus Write(std::span<uint8_t> data) override;

    HalStatus Read(std::span<uint8_t> data, uint64_t &readSize) override;

    HalStatus WriteString(std::string_view text);

    static std::unique_ptr<Uart> Default();

private:
    bool isTransmitEmpty() const;
};

} // namespace RVOSHal

RVOSHal::HalStatus uart_putc(char value);
RVOSHal::HalStatus uart_puts(std::string_view text);
