#pragma once

#include <cstdint>

namespace dev {

/// @brief Legacy block device marker. Prefer hal::concepts::BlockDevice.
class IBlock {
public:
    virtual ~IBlock()= default;
};

} // namespace dev
