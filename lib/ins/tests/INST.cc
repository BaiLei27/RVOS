#include <algorithm>
#include <cassert>
// #include <memory>

#include "Core/Instruction.hh"
#include "Log/Logger.hpp"
using namespace std; // NOLINT

int main()
{
    // Test R-type: add x1, x2, x3 (0x003100b3)
    // // uint32_t add= 0x003100b3;
    // uint32_t add= 0x001161b3;
    // // 0000 0000 0000 0000 0010 0000 0000 0011
    // // 0000 0000 0000 0000 0011 0000 0000 0011

    // // Verify opcode is 0x33 (R-type)
    // assert((add & 0x7F) == 0x33);

    // // std::vector<Instruction> insts{};
    // // insts.emplace_back(add);
    // Instruction a { add }, ai { 0x00310093 }; // addi x1, x2, 3
    // a.Decode();
    // ai.Decode();

    std::string inst= { "add x1, x2, x3" };
    std::ranges::replace(inst, ',', ' ');
    std::istringstream iss(inst);

    vector<string> v;
    while(iss >> v.emplace_back())
        ;

    for(auto &s: v) {
        cout << s << '\n';
    }
    LOG_MSG(D, "All tests ran (manual checks).");

    return 0;
}
