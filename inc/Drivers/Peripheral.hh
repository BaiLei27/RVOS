#pragma once

#include <cstdint>
// #include <memory>
#include <span>

namespace RVOSHal { // NOLINT

enum class PeripheralType : int8_t {
    UART= 0,
};

enum class HalStatus : int8_t {
    HAL_OK             = 0,
    HAL_ERR_INVALID_ARG= -1,
    HAL_ERR_UNSUPPORTED= -2,
    HAL_ERR_IO         = -3,
};

class IPeripheral {
public:
    explicit IPeripheral(uint64_t baseAddress);
    virtual ~IPeripheral()= default;

    virtual PeripheralType GetType() const                             = 0;
    virtual HalStatus Init()                                           = 0;
    virtual HalStatus Write(std::span<uint8_t> data)                   = 0;
    virtual HalStatus Read(std::span<uint8_t> data, uint64_t &readSize)= 0;

    uint64_t BaseAddress() const;

protected:
    uint64_t BaseAddress_;
};

HalStatus InitPeripheral(IPeripheral &pDevice);
HalStatus WritePeripheral(IPeripheral &pDevice, std::span<uint8_t> data);
HalStatus ReadPeripheral(IPeripheral &pDevice, std::span<uint8_t> data, uint64_t &readSize);

} // namespace RVOSHal
