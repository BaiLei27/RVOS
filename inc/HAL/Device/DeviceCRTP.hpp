#pragma once

#include <cstdint>
#include <string_view>
#include <utility>

#include "Drivers/Serial/UartConfig.hh"

class Char {
public:
    template <typename Self>
    int Puts(this Self &&self, std::string_view text)
    {
        for(const auto &c: text) {
            if(std::forward<Self>(self).PutChar(static_cast<uint8_t>(c)) < 0) {
                return -1;
            }
        }
        return 0;
    }

protected:
    Char()= default;
    ~Char()= default;
};

/// Deducing-this base for UART-class drivers; Derived implements SetConfig / GetConfig.
class Serial: public Char {
public:
    template <typename Self>
    int SetConfig(this Self &&self, const drv::uart::Config &cfg)
    {
        return std::forward<Self>(self).SetConfig(cfg);
    }

    template <typename Self>
    int GetConfig(this Self &&self, drv::uart::Config &cfg)
    {
        return std::forward<Self>(self).GetConfig(cfg);
    }

    [[nodiscard]] virtual int GetIRQ() const { return IRQ_; }

protected:
    Serial()= default;

    explicit Serial(uint8_t irq): IRQ_(irq) { }

    ~Serial()= default;

protected:
    uint8_t IRQ_ {};
};
