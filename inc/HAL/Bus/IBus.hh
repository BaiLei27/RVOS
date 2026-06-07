#pragma once

/// @brief Legacy bus port. Prefer hal::concepts::Bus + platform-specific CRTP implementation.
class IBus {
public:
    virtual ~IBus()= default;
};
