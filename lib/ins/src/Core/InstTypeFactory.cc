#include <iostream>

#include "Core/InstTypeFactory.hh"
#include "Core/InstType.hh"

std::unique_ptr<IBaseInstType> InstTypeFactory::CreateType(uint32_t inst)
{
    uint16_t opcode= inst & 0x7F; // Extract lower 7 bits of opcode

    if(auto it= G_Opcode2Format.find(opcode); it != G_Opcode2Format.end()) {
        switch(it->second) {
        case InstFormat::R:
            return std::make_unique<RType>(inst, "R-Type");

        case InstFormat::I:
            return std::make_unique<IType>(inst, "I-Type");
#if 00
        case InstFormat::S:
            return std::make_unique<SType>(inst, InstFormat::S);
        case InstFormat::B:
            return std::make_unique<BType>(inst, InstFormat::B);
        case InstFormat::U:
            return std::make_unique<UType>(inst, InstFormat::U);
        case InstFormat::J:
            return std::make_unique<JTypeInst>(inst, InstFormat::J);
#endif
        default:
            std::cout << "Unsupported instruction format\n";
        }
    } else {
        std::cout << "Unsupported opcode: " << opcode << '\n';
    }

    return nullptr;
}
