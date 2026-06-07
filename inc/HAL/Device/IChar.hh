#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "IDevice.hh"

/// @brief Legacy character device (virtual). Prefer hal::CrtpCharDevice + CharDevice concept.
class IChar: public IDevice {
public:
    using IDevice::IDevice;

    virtual ~IChar()= default;

public:
    virtual std::optional<uint8_t> GetChar()= 0;
    virtual int PutChar(uint8_t c)          = 0;

    virtual int Puts(std::string_view text)
    {
        for(const auto &c: text) {
            if(this->PutChar(c) < 0) {
                return -1;
            }
        }
        return 0;
    }
};

// Date:26/05/11/23:45
