#pragma once

#include <string_view>

/// @brief Legacy polymorphic device port. Prefer dev::Handle + DeviceLifecycle concept.
class IDevice {
public:
    explicit IDevice(std::string_view name= "null"): Name_(name) { }

    virtual ~IDevice()= default;

    // virtual int PowerUp()= 0;
    virtual int Init()   = 0;
    virtual void Deinit()= 0;

    virtual int Suspend() { return 0; }

    virtual int Resume() { return 0; }

    virtual std::string_view GetName() const noexcept= 0;

    virtual int Control(int cmd, void *pArg) { return -1; }

protected:
    std::string_view Name_;
};

// Date:26/05/18/22:52
