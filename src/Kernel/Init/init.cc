
#include "Device/DevManager.hh"

int BoardInit()
{
    dev::Manager &mgr= dev::Manager::Instance();

    if(mgr.Register(nullptr) != 0) {
        return -1;
    }
    if(mgr.InitAll() != 0) {
        return -1;
    }
    return 0;
}

int SystemInit()
{
    return 0;
}

int main()
{
    SystemInit();
    return 0;
}
