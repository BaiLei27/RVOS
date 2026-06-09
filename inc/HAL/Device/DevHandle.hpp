#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "Concepts/DeviceCep.hh"
#include "Drivers/Serial/UartConfig.hh"

namespace dev {

/// Type-erased device port for dev::Manager (lifecycle + optional char + serial).
class Handle {
public:
    using InitFn_u     = int (*)(void *);
    using DeinitFn_u   = void (*)(void *);
    using GetNameFn_u  = std::string_view (*)(void *);
    using GetCharFn_u  = std::optional<uint8_t> (*)(void *);
    using PutCharFn_u  = int (*)(void *, uint8_t);
    using SetConfigFn_u= int (*)(void *, const drv::uart::Config *);
    using GetConfigFn_u= int (*)(void *, drv::uart::Config *);

    void *pObj_ {};
    InitFn_u pInit_ {};
    DeinitFn_u pDeinit_ {};
    GetNameFn_u pGetName_ {};
    GetCharFn_u pGetChar_ {};
    PutCharFn_u pPutChar_ {};
    SetConfigFn_u pSetConfig_ {};
    GetConfigFn_u pGetConfig_ {};

    Handle()= default;

    template <typename D>
        requires cep::DeviceLifecycle<D>
    explicit Handle(D &impl) noexcept
        : pObj_(static_cast<void *>(&impl)),
          pInit_(initThunk<D>),
          pDeinit_(deinitThunk<D>),
          pGetName_(getNameThunk<D>),
          pGetChar_(getCharThunk<D>),
          pPutChar_(putCharThunk<D>),
          pSetConfig_(setConfigThunk<D>),
          pGetConfig_(getConfigThunk<D>)
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept { return pObj_ != nullptr; }

    [[nodiscard]] bool IsCharDevice() const noexcept
    {
        return pGetChar_ != nullptr && pPutChar_ != nullptr;
    }

    [[nodiscard]] bool IsSerialDevice() const noexcept
    {
        return pSetConfig_ != nullptr && pGetConfig_ != nullptr;
    }

    int Init() const
    {
        return pInit_ != nullptr ? pInit_(pObj_) : -1;
    }

    void Deinit() const
    {
        if(pDeinit_ != nullptr) {
            pDeinit_(pObj_);
        }
    }

    [[nodiscard]] std::string_view GetName() const
    {
        return pGetName_ != nullptr ? pGetName_(pObj_) : std::string_view {};
    }

    [[nodiscard]] std::optional<uint8_t> GetChar() const
    {
        return pGetChar_ != nullptr ? pGetChar_(pObj_) : std::nullopt;
    }

    int PutChar(uint8_t c) const
    {
        return pPutChar_ != nullptr ? pPutChar_(pObj_, c) : -1;
    }

    int Puts(std::string_view text) const
    {
        for(const auto &c: text) {
            if(PutChar(static_cast<uint8_t>(c)) < 0) {
                return -1;
            }
        }
        return 0;
    }

    int SetConfig(const drv::uart::Config &cfg) const
    {
        return pSetConfig_ != nullptr ? pSetConfig_(pObj_, &cfg) : -1;
    }

    int GetConfig(drv::uart::Config &cfg) const
    {
        return pGetConfig_ != nullptr ? pGetConfig_(pObj_, &cfg) : -1;
    }

private:
    template <typename D>
    static int initThunk(void *pFunc)
    {
        return static_cast<D *>(pFunc)->Init();
    }

    template <typename D>
    static void deinitThunk(void *pObj)
    {
        static_cast<D *>(pObj)->Deinit();
    }

    template <typename D>
    static std::string_view getNameThunk(void *pObj)
    {
        return static_cast<D *>(pObj)->GetName();
    }

    template <typename D>
    static std::optional<uint8_t> getCharThunk(void *pObj)
    {
        if constexpr(cep::CharReadable<D>) {
            return static_cast<D *>(pObj)->GetChar();
        } else {
            return std::nullopt;
        }
    }

    template <typename D>
    static int putCharThunk(void *pObj, uint8_t c)
    {
        if constexpr(cep::CharWritable<D>) {
            return static_cast<D *>(pObj)->PutChar(c);
        } else {
            return -1;
        }
    }

    template <typename D>
    static int setConfigThunk(void *pObj, const drv::uart::Config *pCfg)
    {
        if constexpr(cep::SerialConfigurable<D>) {
            return static_cast<D *>(pObj)->SetConfig(*pCfg);
        } else {
            return -1;
        }
    }

    template <typename D>
    static int getConfigThunk(void *pObj, drv::uart::Config *pCfg)
    {
        if constexpr(cep::SerialConfigurable<D>) {
            return static_cast<D *>(pObj)->GetConfig(*pCfg);
        } else {
            return -1;
        }
    }
};

} // namespace dev
