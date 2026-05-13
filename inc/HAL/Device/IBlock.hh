#pragma once

#include <cstdint>

class IChar {
public:
    virtual ~IChar()= default;

    uintptr_t Base() const noexcept;

protected:
    explicit IChar(uintptr_t base);

private:
    uintptr_t Base_;
};
