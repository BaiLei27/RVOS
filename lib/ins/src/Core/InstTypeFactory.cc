#include <iostream>

#include "Core/InstTypeFactory.hh"
#include "Core/InstType.hh"

std::unique_ptr<IBaseInstType> InstTypeFactory::CreateType(uint32_t inst, bool hasSetABI)
{
    uint16_t opcode= inst & 0x7F; // Extract lower 7 bits of opcode

    if(auto it= G_Opcode2Format.find(opcode); it != G_Opcode2Format.end()) {
        switch(it->second) {
        case InstFormat::R:
            return std::make_unique<RType>(inst, it->second, hasSetABI);

        default:
            std::cout << "Unsupported instruction format\n";
        }
    } else {
        std::cout << "Unsupported opcode: " << opcode << '\n';
    }

    return nullptr;
}
