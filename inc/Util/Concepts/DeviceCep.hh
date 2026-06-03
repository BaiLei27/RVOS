#pragma once
#include <concepts>
#include <cstdint>
#include <optional>
#include <string_view>

#include "Drivers/Serial/UartConfig.hh"

namespace cep {

template <typename D>
concept CharReadable= requires(D &d) {
    { d.GetChar() } -> std::same_as<std::optional<uint8_t>>;
};

template <typename D>
concept CharWritable= requires(D &d, uint8_t c) {
    { d.PutChar(c) } -> std::same_as<int>;
};

template <typename D>
concept BlockDevice= requires(D &d, std::size_t off, void *pBuf, std::size_t len) {
    { d.Read(off, pBuf, len) } -> std::same_as<int>;
    { d.Write(off, pBuf, len) } -> std::same_as<int>;
    { d.GetCapacity() } -> std::convertible_to<std::size_t>;
};

template <typename D>
concept DeviceLifecycle= requires(D &d) {
    { d.Init() } -> std::same_as<int>;
    { d.Deinit() } -> std::same_as<void>;
    { d.GetName() } -> std::convertible_to<std::string_view>;
};

/// Serial port configuration (baud, data bits, parity, flow control).
/// Implement SetConfigImpl / GetConfigImpl on Derived, or use public SetConfig / GetConfig (see Serial CRTP).
template <typename D>
concept SerialConfigurable= requires(D &d, const drv::uart::Config &in, drv::uart::Config &out) {
    { d.SetConfig(in) } -> std::same_as<int>;
    { d.GetConfig(out) } -> std::same_as<int>;
};

template <typename D>
concept CharDevice= CharReadable<D> && CharWritable<D>;

/// Full UART: character stream + serial configuration (typical 16550-class driver).
template <typename D>
concept SerialDevice= DeviceLifecycle<D> && CharDevice<D> && SerialConfigurable<D>;

} // namespace cep

// Date:26/06/02/22:46
