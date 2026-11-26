#pragma once
#include <cstdint>

#include "Device/DevManager.hh"

class Kernel: public ISingleton<Kernel> {
private:
    dev::Manager *PdevMgr_ {};
    bool Ready_ { false };
    uint64_t TickCount_ { 0 };

public:
    using ISingleton<Kernel>::ISingleton;

public:
    int Run();

private:
    int boardInit();
    int init();
    static void SupervisorMain() noexcept;
    static void OnTimerTick() noexcept;
};
