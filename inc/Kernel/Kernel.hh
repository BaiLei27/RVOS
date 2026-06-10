#pragma once
#include "Device/DevManager.hh"

class Kernel: public ISingleton<Kernel> {
private:
    dev::Manager *PdevMgr_ {};
    bool Ready_ { false };

public:
    using ISingleton<Kernel>::ISingleton;

public:
    [[noreturn]] int Run();

private:
    int boardInit();
    int init();
};
