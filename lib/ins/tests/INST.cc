#include <cassert>
#include <iostream>

#include "Core/Instruction.hh"
using namespace std; // NOLINT

int main()
{
    // Test R-type: add x1, x2, x3 (0x003100b3)
    uint32_t add= 0x003100b3;
    // uint32_t add= 0x001161b3; // or x3, x2, x1
    // 0000 0000 0000 0000 0010 0000 0000 0011
    // 0000 0000 0000 0000 0011 0000 0000 0011
    assert((add & 0x7F) == 0x33);
#if 00
    std::unique_ptr<IBaseInstType> inst1= make_unique<RType>(add, InstFormat::R, true);
    inst1->Disassembly();
    inst1->Parse();

#elif 01
    vector<Instruction> v;
    v.emplace_back(add, false);
    v.emplace_back(0x00310093, false);

    for(auto &i: v) {
        i.Decode();
    }
    cout << static_cast<string>(v[0]) << '\n';
    const auto &t= v[0].GetType();
    cout << static_cast<uint32_t>(v[0].GetType().GetInstFormat());
    v[1].GetHexStr();
    v[1].GetBitField();
#elif 01

    std::unique_ptr<IBaseInstType> inst2= InstTypeFactory::CreateType(add);
    inst2->Disassembly();
    inst2->Parse();

#endif
    return 0;
}
