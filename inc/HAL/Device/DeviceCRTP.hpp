#pragma once

#include <cstdint>
#include <string_view>

#include "Drivers/Serial/UartConfig.hh"

template <typename Derived>
class Device {
public:
    Derived &Self() noexcept { return static_cast<Derived &>(*this); }

    const Derived &Self() const noexcept { return static_cast<const Derived &>(*this); }

protected:
    Device() = default;
    ~Device()= default;
};

template <typename Derived>
class Char: public Device<Derived> {
public:
    int Puts(std::string_view text)
    {
        for(const auto &c: text) {
            if(this->Self().PutChar(static_cast<uint8_t>(c)) < 0) {
                return -1;
            }
        }
        return 0;
    }

protected:
    Char() = default;
    ~Char()= default;
};

/// CRTP base for UART-class drivers. Derived implements SetConfig / SetConfig only.
template <typename Derived>
class Serial: public Char<Derived> {
public:
    int SetConfig(const drv::uart::Config &cfg) { return this->Self().SetConfig(cfg); }

    int GetConfig(drv::uart::Config &cfg) const { return this->Self().GetConfig(cfg); }

protected:
    Serial() = default;
    ~Serial()= default;
};
