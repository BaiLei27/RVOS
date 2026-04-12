#include <array>
#include <cstdint>
#include <string_view>

#include "Drivers/Uart.hh"

namespace {

inline uint8_t MmioRead8(uint64_t address)
{
    return *reinterpret_cast<volatile uint8_t *>(address);
}

inline void MmioWrite8(uint64_t address, uint8_t value)
{
    *reinterpret_cast<volatile uint8_t *>(address)= value;
}

} // namespace

namespace RVOSHal {
Uart::Uart(uint64_t baseAddress): IPeripheral(baseAddress) { }

PeripheralType Uart::GetType() const
{
    return PeripheralType::UART;
}

HalStatus Uart::Init()
{
    return HalStatus::HAL_OK;
}

HalStatus Uart::Write(std::span<uint8_t> data)
{
    if(data.empty()) {
        return HalStatus::HAL_ERR_INVALID_ARG;
    }

    for(const auto ONE_BYTE: data) {
        while(!isTransmitEmpty()) {
        }
        MmioWrite8(BaseAddress() + G_UART_THR, ONE_BYTE);
    }

    return HalStatus::HAL_OK;
}

HalStatus Uart::Read(std::span<uint8_t> data, uint64_t &readSize)
{
    readSize= 0;
    return HalStatus::HAL_ERR_UNSUPPORTED;
}

HalStatus Uart::WriteString(std::string_view text)
{
    if(text.empty()) {
        return HalStatus::HAL_ERR_INVALID_ARG;
    }

    std::array<uint8_t, 256> byteBuffer{};

    for(int i= 0; const auto ONE_CHAR: text) {
        byteBuffer[i++]= static_cast<uint8_t>(ONE_CHAR);
    }
    return Write(byteBuffer);
}

// std::unique_ptr<Uart> Uart::Default()
// {
//     static auto s_uart0= std::make_unique<Uart>(G_UART0_BASE);
//     return s_uart0;
// }

bool Uart::isTransmitEmpty() const
{
    return (MmioRead8(BaseAddress() + G_UART_LSR) & 0x20U) != 0U;
}

} // namespace RVOSHal

// RVOSHal::HalStatus uart_putc(char value)
// {
//     auto uart0= RVOSHal::Uart::Default();
//     if(!uart0) {
//         return RVOSHal::HalStatus::HAL_ERR_IO;
//     }
//     const std::array<uint8_t, 1> ONE_CHAR= { static_cast<uint8_t>(value) };
//     return uart0->Write(ONE_CHAR);
// }

// RVOSHal::HalStatus uart_puts(std::string_view text)
// {
//     auto uart0= RVOSHal::Uart::Default();
//     if(!uart0) {
//         return RVOSHal::HalStatus::HAL_ERR_IO;
//     }
//     return uart0->WriteString(text);
// }
