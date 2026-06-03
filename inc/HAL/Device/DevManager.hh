/**
 * @file DevManager.hh
 * @brief Central registry and lifecycle for HAL devices (type-erased handles + legacy IDevice*)
 */
#pragma once
#include <array>
#include <cstddef>
#include <string_view>

#include "Device/DevHandle.hpp"
#include "Device/IDevice.hh"
#include "ISingleton.hpp"

namespace dev {

class Manager: public ISingleton<Manager> {
public:
    explicit Manager(Access a): ISingleton<Manager>(a) { } // This is the only valid singleton entry for construction.

public:
    static constexpr std::size_t G_kMaxDevices { 32 };

    int Register(const dev::Handle &handle);
    int Register(IDevice *pDev);

    int InitAll();
    void DeinitAll();

    [[nodiscard]] dev::Handle FindHandle(std::string_view name) const;
    IDevice *Find(std::string_view name) const;

    [[nodiscard]] std::size_t GetCount() const noexcept;

private:
    struct Entry {
        dev::Handle devHandle_;
        IDevice *pLegacy_ {};
    };

    std::array<Entry, G_kMaxDevices> Entries_ {};
    size_t Count_ { 0 };
};

} // namespace dev
