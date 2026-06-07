/**
 * @file MMIO.hpp
 * @author Swan BaiLei27 (https://github.com/BaiLei27)
 * @brief 1. Template class over plain class/function: each instance’s bit-width is fixed at compile time (uint8_t/uint16_t/uint32_t).
             giving type-safe access with full inlining and zero runtime overhead.
          2. Wraps volatile T*: guarantees every access is treated as a hardware operation – the compiler cannot optimize it away.
          3. Member functions: read()/write() for clear register I/O; setBits()/clearBits() for atomic read-modify-write without manual masking mistakes.
          4. Object semantics: one MMIO object = one physical register, making drivers Self-documenting (e.g. lcr.write(0x03) is far clearer than a raw address macro).
 * @version 0.1
 * @date 2026/05/17/23:05
 *
 * @copyright Copyright (c) 2026 Swan BaiLei27 <SwanBaiLei2Seven@foxmail.com>
 *
 */
#pragma once

#include <cstdint>

template <typename T>
class PMMIO {
public:
    explicit PMMIO(uintptr_t addr): Addr_(reinterpret_cast<volatile T *>(addr)) { }

    PMMIO &operator= (T val)
    {
        this->Write(val);
        return *this;
    }

    explicit operator T() const { return this->Read(); }

public:
    T Read() const { return *Addr_; }

    void Write(T val) { *Addr_= val; }

    void SetBits(T mask) { *Addr_|= mask; }

    void ClearBits(T mask) { *Addr_&= ~mask; }

    void Set(uint8_t pos= 0) { *Addr_|= 1 << pos; }

    void Reset(uint8_t pos= 0) { *Addr_&= ~(1 << pos); }

    void ToggleBits(T mask) { *Addr_^= mask; }

    template <typename Self>
    auto *GetAddr(this Self &&s) { return s.Addr_; }

private:
    volatile T *Addr_ {};
};

template <typename T>
class MMIO {
public:
    MMIO()                        = delete;
    ~MMIO()                       = delete;
    MMIO(const MMIO &)            = delete;
    MMIO &operator= (const MMIO &)= delete;

    MMIO &operator= (T val)
    {
        Write(val);
        return *this;
    }

    explicit operator T() const { return this->Read(); }

public:
    [[nodiscard]] T Read() const { return Val_; }

    void Write(T val) { Val_= val; }

    void SetBits(T mask) { Val_|= mask; }

    void ClearBits(T mask) { Val_&= ~mask; }

    void Set(uint8_t pos= 0) { Val_|= 1 << pos; }

    void Reset(uint8_t pos= 0) { Val_&= ~(1 << pos); }

    void ToggleBits(T mask) { Val_^= mask; }

    template <typename Self>
    auto *GetAddr(this Self &&s) { return &s.Val_; }

private:
    volatile T Val_ {};
};

// Date:26/05/17/23:09
