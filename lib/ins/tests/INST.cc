#include <algorithm>
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
    std::string inst= { "sub x3, x2, x1" };

    Instruction a(inst);
    a.Decode();
    cout << a.GetFormat() << '\n'
         << static_cast<string>(a) << '\n'
         << a.GetHexStr() << '\n'
         << a.GetXLEN() << '\n'
         << a.GetManual() << '\n';
    // std::ranges::replace(inst, ',', ' ');
    // std::istringstream iss(inst);

    // vector<string> v;
    // while(iss >> v.emplace_back())
    //     ;

    // for(auto &s: v) {
    //     cout << s << '\n';
    // }
#elif 01
    vector<Instruction> v;
    v.emplace_back(add);
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
