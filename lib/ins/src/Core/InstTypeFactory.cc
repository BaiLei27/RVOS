#include <iostream>

#include "Core/InstTypeFactory.hh"
#include "Core/InstType.hh"

std::unique_ptr<IBaseInstType> InstTypeFactory::CreateType(uint32_t inst, bool hasSetABI)
{
    uint16_t opcode= inst & 0x7F; // Extract lower 7 bits of opcode

    return createHelper(opcode, hasSetABI);
}

std::unique_ptr<IBaseInstType> InstTypeFactory::CreateType(std::vector<std::string> &instAssembly, bool hasSetABI)
{
    auto matchOpc= matchInstOpcode(instAssembly[0]);

    if(matchOpc) {
        return createHelper(*matchOpc, hasSetABI);
    }
    // return createHelper(instName, hasSetABI);
    // std::make_unique<RType>(instName, hasSetABI);
    std::cout << "Unsupported instruction name: " << instAssembly[0] << '\n';

    return nullptr;
}

template <typename T>
std::unique_ptr<IBaseInstType> InstTypeFactory::createHelper(T key, bool hasSetABI)
{
    if(auto it= G_Opcode2Format.find(key); it != G_Opcode2Format.end()) {
        std::unique_ptr<IBaseInstType> tmp;
        switch(it->second) {
        case InstFormat::R:
            tmp= std::make_unique<RType>(key, it->second, hasSetABI);
            tmp->SetFormat(it->second);
            return tmp;
        default:
            std::cout << "Unsupported instruction format\n";
        }

    } else {
        std::cout << "not found key: " << key << '\n';
    }
    return nullptr;
}

std::optional<uint16_t> InstTypeFactory::matchInstOpcode(std::string_view instName)
{
    for(const auto &entry: RType::G_INST_TABLE) {
        if(std::get<2>(entry) == instName) {
            return std::make_optional(std::get<1>(entry)); // get opcode
        }
    }

    return std::nullopt;
}
