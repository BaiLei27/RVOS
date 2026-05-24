#include "Trap/Trap.hh"
#include "Trap.hh"

int main()
{
    arch::trap::SupervisorTrap::Init();
    arch::trap::RunSyncTests();
    return 0;
}
