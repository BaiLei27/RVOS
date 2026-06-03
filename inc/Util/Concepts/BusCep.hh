#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace hal::concepts {

template <typename B>
concept Bus= requires(B &b, uintptr_t phys, std::size_t len) {
    { b.MapPhys(phys, len) } -> std::convertible_to<void *>;
    { b.Unmap(std::declval<void *>(), len) } -> std::same_as<void>;
};

} // namespace hal::concepts
