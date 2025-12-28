#include <cassert>

#include "Core/InstType.hh"
using namespace std; // NOLINT

int main()
{
    // Test R-type: add x1, x2, x3 (0x003100b3)

    // uint32_t add= 0x003100b3;
    uint32_t add= 0x001161b3; // or x3, x2, x1
    // 0000 0000 0000 0000 0010 0000 0000 0011
    // 0000 0000 0000 0000 0011 0000 0000 0011
    assert((add & 0x7F) == 0x33);
    std::unique_ptr<IBaseInstType> inst1= make_unique<RType>(add, InstFormat::R, true);
    inst1->Disassembly();
    inst1->Parse();

    return 0;
}
