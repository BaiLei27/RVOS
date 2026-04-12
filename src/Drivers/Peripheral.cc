#include "Drivers/Peripheral.hh"

namespace RVOSHal {

IPeripheral::IPeripheral(uint64_t baseAddress)
    : BaseAddress_(baseAddress)
{
}

uint64_t IPeripheral::BaseAddress() const
{
    return BaseAddress_;
}

HalStatus InitPeripheral(IPeripheral &pDevice)
{
    return pDevice.Init();
}

HalStatus WritePeripheral(IPeripheral &pDevice, std::span<uint8_t> data)
{
    if(data.empty()) {
        return HalStatus::HAL_ERR_INVALID_ARG;
    }
    return pDevice.Write(data);
}

HalStatus ReadPeripheral(IPeripheral &pDevice, std::span<uint8_t> data, uint64_t &readSize)
{
    if(data.empty()) {
        return HalStatus::HAL_ERR_INVALID_ARG;
    }
    return pDevice.Read(data, readSize);
}

} // namespace RVOSHal
