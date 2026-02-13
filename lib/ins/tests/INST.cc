#include <cassert>
#include <iostream>

#include "Core/Instruction.hh"
#include "ISA/Regs.hpp"
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
#elif 00

    auto opt= isa::LOOKUP_REG_NAME(50);
    cout << opt << '\n';

    auto test= [&](std::string name) {
        auto tmp= name;
        auto idx= isa::LOOKUP_REG_IDX(name);

        if(idx) {
            std::cout << "Lookup '" << tmp
                      << "': index = " << *idx << "\n";
        } else {
            std::cout << "Lookup '" << tmp
                      << "': not found\n";
        }
    };

    test("X0");    // 0
    test("xx1");   // not found
    test("x10");   // 10
    test("x2");    // 2
    test("x99");   // not found
    test("x-19");  // not found
    test("zero");  // 0
    test("a0");    // 10
    test("s7");    // 23
    test("S0");    // 23
    test("s0/fp"); // 23
    test("foo");   // not found
    test("0x88");  // 8
    test("8");     // 8

#elif 01
    std::string sub= { "sub x3, ra, 0x8" }; // 0x408081b3

    Instruction s(sub, false);
    s.Decode();

    cout << s.GetFormat() << '\n'
         << s.GetTypePtr()->GetInstOpcode() << '\n'
         << static_cast<string>(s) << '\n'
         << std::hex << static_cast<uint32_t>(s) << '\n'
         << s.GetHexStr() << '\n'
         << s.GetXLEN() << '\n'
         << s.GetManual() << '\n';

#elif 01
    vector<Instruction> v;
    v.emplace_back(add, true);
    v.emplace_back(0x401101b3); // sub x3, x2, x1
    v.emplace_back(0x00310093); // addi x1, x2, 3

    for(auto &i: v) {
        i.Decode();
        i.ShowInfo();
        puts("--------------");
    }
    cout << static_cast<string>(v[0]) << '\n';
    const auto &t= v[0].GetType().GetInstAssembly();
    for(const auto &s: t) {
        cout << s << ' ';
    }
    cout << '\n';
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
