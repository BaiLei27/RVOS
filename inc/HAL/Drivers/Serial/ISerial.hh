#pragma once

#include "Device/IChar.hh"
#include "Drivers/Serial/UartConfig.hh"

/// @brief Legacy virtual serial port. CRTP drivers use cep::SerialConfigurable (see NS16550A).
class ISerial: public IChar {
public:
    explicit ISerial(uint8_t irq): IRQ_(irq) { }

    virtual ~ISerial()= default;

public:
    virtual int SetConfig(const drv::uart::Config &cfg)= 0;
    virtual int GetConfig(drv::uart::Config &cfg) const= 0;

    virtual int GetIRQ() const { return IRQ_; }

protected:
    uint8_t IRQ_ {};
};

// Date:26/05/18/22:59
