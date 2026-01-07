#include <algorithm>
#include <cassert>
#include <iostream>

#include "Core/Instruction.hh"
using namespace std; // NOLINT

int main()
{
    uint32_t add= 0x008100b3; // add x1 x2, x8
    // uint32_t add= 0x001161b3; // or x3, x2, x1
    // 0000 0000 0000 0000 0010 0000 0000 0011
    // 0000 0000 0000 0000 0011 0000 0000 0011
    assert((add & 0x7F) == 0x33);
#if 00
    std::unique_ptr<IBaseInstType> inst1= make_unique<RType>(add, InstFormat::R, true);
    inst1->Disassembly();
    inst1->Parse();

#elif 01
    std::string sub= { "sub x3, x2, x1" };

    Instruction a(sub, true);
    a.Decode();
    cout << a.GetFormat() << '\n'
         << static_cast<string>(a) << '\n'
         << a.GetHexStr() << '\n'
         << a.GetXLEN() << '\n'
         << a.GetManual() << '\n';
#elif 01
    vector<Instruction> v;
    v.emplace_back(add, true);
    // v.emplace_back(0x00310093); // addi x1, x2, 3
    v.emplace_back(0x401101b3); // sub x3, x2, x1

    for(auto &i: v) {
        i.Decode();
        puts("--------------");
    }
    cout << static_cast<string>(v[0]) << '\n';
    const auto &t= v[0].GetType().GetInstAssembly();
    for(const auto &s: t) {
        cout << s << '\n';
    }
    cout << static_cast<uint32_t>(v[0].GetType().GetInstFormat()) << '\n';
    cout << v[1].GetHexStr() << '\n';
    // cout << v[1].GetBitField();
    cout << static_cast<uint32_t>(v[0]) << '\n';
#elif 01

    std::unique_ptr<IBaseInstType> inst2= InstTypeFactory::CreateType(add);
    inst2->Disassembly();
    inst2->Parse();

#endif
    return 0;
}
