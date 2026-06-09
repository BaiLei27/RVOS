/**
 * @file ns16550A.hh
 * @author Swan BaiLei27 (https://github.com/BaiLei27)
 * @brief NS16550A serial driver implementation
 * Ref: [1] TECHNICAL DATA ON 16550, http://byterunner.com/16550.html
 *      [2] https://senjlearning.space/1-Theory/3-System/03-Operating-System/rCore-tutorial/9-IO-devices-management/30-UART-driver
 *      [3] https://github.com/zephyrproject-rtos/zephyr/blob/main/drivers/serial/uart_ns16550.c
 *      [4] https://www.lammertbies.nl/comm/info/serial-uart
 * @version 0.1
 * @date 2026/05/20/22:05
 *
 * @copyright Copyright (c) 2026 Swan BaiLei27 <SwanBaiLei2Seven@foxmail.com>
 *
 */
#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "Concepts/DeviceCep.hh"
#include "Device/DeviceCRTP.hpp"
#include "MMIO/MMIO.hpp"

namespace ns16x50 {
struct Regs {
    MMIO<uint8_t> rthrDll_; // (r) Receiver Holding Register | (w) Transmitter Holding Register | DLL(lcr DLAB=1)
    MMIO<uint8_t> ierDlm_;  // (rw) Interrupt Enable Register | DLM(lcr DLAB=1)
    MMIO<uint8_t> iirFcr_;  // (r)Interrupt Status Register | (w) FIFO Control Register
    MMIO<uint8_t> lcr_;     // (rw) Line Control Register
    MMIO<uint8_t> mcr_;     // (rw) Modem Control Register
    MMIO<uint8_t> lsr_;     // (r) Line Status Register
    MMIO<uint8_t> msr_;     // (r) Modem Status Register
    MMIO<uint8_t> scr_;     // (rw) Scratch Register, it has only limited use.
};

enum IER : uint8_t {
    RXEN= 0x01, // Received data available
    TXEN= 0x02, // Transmitter holding register enable
    RLSC= 0x04, // Receiver line status register change
    MSC = 0x08, // Modem status register change
    SLP = 0x10, // Sleep mode (16750 only)
    LPM = 0x20, // Low power mode (16750 only)

    // IER_NU1 = 0x40, // Reserved
    // IER_NU2 = 0x80, // Reserved
};

enum IIR : uint8_t {
    IP  = 0x01, // Interrupt pending
    MSC1= 0x02, // Modem status change
    MSC2= 0x04,
    MSC3= 0x08,

    // IIR_NU1   = 0x10, // Reserved
    FIFO64= 0x20, // 64 byte FIFO enabled (16750 only)

    FE1= 0x40, // FIFO enabled
    FE2= 0x80, // FIFO enabled
};

enum FCR : uint8_t {
    FIFOEN = 0x01, // Enable FIFO's
    RCVRCLR= 0x02, // Clear receive FIFO
    XMITCLR= 0x04, // Clear transmit FIFO
    DMA_M  = 0x08, // Select DMA mode
    // RCR_NU1    = 0x10, // Reserved
    // FIFO64 = 0x20, // Enable 64 byte FIFO (16750 only)

    RCVR_LSB= 0x40, // byte in RCVR FIFO 0b00= 1B, 0b01= 4B, 0b10= 8B, 0b11= 14B
    RCVR_MSB= 0x80, //
};

enum LCR : uint8_t {
    WLS1= 0x01, // Word length select, 0b00= 5b, 0b01= 6b, 0b10= 7b, 0b11= 8b
    WLS2= 0x02,

    STB= 0x04, // Number of stop bits

    PEN= 0x08, // Parity enable 0b000 = none, 0b001= odd, 0b011= even, 0b101= High(stick), 0b111= Low(stick)
    EPS= 0x10, // Even parity select
    SP = 0x20, // Stick parity

    SB  = 0x40, // Set break
    DLAB= 0x80, // Divisor latch access bit
};

enum MCR : uint8_t {
    DTR = 0x01, // Data terminal ready
    RTS = 0x02, // Request to send
    OP1 = 0x04, // Auxiliary output 1
    OP2 = 0x08, // Auxiliary output 2
    LOOP= 0x10, // Loopback mode
    AFCE= 0x20, // Autoflow control enable
    // MCR_NU1 = 0x40, // Reserved
    // MCR_NU2 = 0x80, // Reserved
};

enum LSR : uint8_t {
    DR    = 0x01, // Data ready
    OER   = 0x02, // Overrun error
    PER   = 0x04, // Parity error
    FER   = 0x08, // Framing error
    BI    = 0x10, // Break interrupt
    THE   = 0x20, // Transmitter holding is empty
    TE    = 0x40, // Transmitter is empty, and line is idle
    FIFOER= 0x80, // Error in received FIFO
};

enum MSR : uint8_t {
    DCTS= 0x01, // Delta clear to send
    DDSR= 0x02, // Delta data set ready
    DRI = 0x04, // Trailing edge ring indicator
    DDCD= 0x08, // Delta data carrier detect
    CTS = 0x10, // Clear to send
    DSR = 0x20, // Data set ready
    RI  = 0x40, // Ring indicator
    DCD = 0x80, // Data carrier detect
};

} // namespace ns16x50

class NS16550A final: public Serial<NS16550A> {
private:
    drv::uart::Config Cfg_;
    ns16x50::Regs *const Regs_ {}; // NOLINT
    uintptr_t PhysAddr_ { 0x10000000UL };
    uint32_t ClockHz_ { 1843200U };
    uint32_t Divisor_ { 1 };
    bool HasInit_ {};

public:
    NS16550A(uintptr_t base, uint8_t irq);
    NS16550A(uintptr_t base, uint8_t irq, uint32_t clockHz);

public:
    int Init();
    void Deinit();
    [[nodiscard]] std::string_view GetName() const noexcept;

    [[nodiscard]] std::optional<uint8_t> GetChar();
    int PutChar(uint8_t c);

    int SetConfig(const drv::uart::Config &cfg);
    int GetConfig(drv::uart::Config &cfg) const;

    [[nodiscard]] ns16x50::Regs *GetRegs() const;

private:
    bool matchesPowerUpDefaults() const;
    void applyUartConfig();
    void setBaudRate();
    uint8_t buildLcr() const;
    uint16_t calcDivisor();

    bool isRxReady() const;
    bool isTxEmpty() const;
};

static_assert(cep::SerialDevice<NS16550A>);
