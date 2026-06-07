#pragma once

#include <cstdint>

namespace drv::uart {

struct Config {
    uint32_t baudRate_ { 115200 };
    uint8_t dataBits_ { 8 };
    uint8_t stopBits_ { 1 };
    uint8_t parity_ { 0 };
    uint8_t flowControl_ { 0 };
};

} // namespace drv::uart

namespace drv::hw {

struct Config {
    uart::Config cfg_;
    uintptr_t base_ {};
    unsigned irq_ {};
    uint32_t clockHz_ {};
};

} // namespace drv::hw
