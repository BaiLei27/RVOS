#include <iostream>

#include "Core/InstTypeFactory.hh"
#include "Core/RType.hh"
#include "Core/IType.hh"
#include "Core/JType.hh"
#include "Core/UType.hh"
#include "Core/SType.hh"
#include "Core/BType.hh"

std::unique_ptr<IBaseInstType> InstTypeFactory::CreateType(uint32_t inst, bool hasSetABI)
{
    uint16_t opcode= inst & 0x7F; // Extract lower 7 bits of opcode
    if(auto it= G_Opcode2Format.find(opcode); it != G_Opcode2Format.end()) {

        switch(it->second) {
        case InstFormat::R:
            return std::make_unique<RType>(inst, it->second, hasSetABI);
        case InstFormat::I:
            return std::make_unique<IType>(inst, it->second, hasSetABI);
        case InstFormat::J:
            return std::make_unique<JType>(inst, it->second, hasSetABI);
        case InstFormat::U:
            return std::make_unique<UType>(inst, it->second, hasSetABI);
        case InstFormat::S:
            return std::make_unique<SType>(inst, it->second, hasSetABI);
        case InstFormat::B:
            return std::make_unique<BType>(inst, it->second, hasSetABI);
        default:
            std::cout << "Unsupported instruction format\n";
        }
    }

    return nullptr;
}

const InstTypeFactory::name2FormatOpcode_u &InstTypeFactory::getName2FormatOpcode()
{
    static const name2FormatOpcode_u OPC_CACHE= [] {
        name2FormatOpcode_u m;
        for(const auto &entry: RType::G_INST_TABLE) {
            if(entry.opcode_ != 0 && !entry.name_.empty()) {
                m.emplace(entry.name_, std::pair { InstFormat::R, entry.opcode_ });
            }
        }
        for(const auto &entry: IType::G_INST_TABLE) {
            if(entry.opcode_ != 0 && !entry.name_.empty()) {
                m.emplace(entry.name_, std::pair { InstFormat::I, entry.opcode_ });
            }
        }
        for(const auto &entry: JType::G_INST_TABLE) {
            if(entry.opcode_ != 0 && !entry.name_.empty()) {
                m.emplace(entry.name_, std::pair { InstFormat::J, entry.opcode_ });
            }
        }
        for(const auto &entry: UType::G_INST_TABLE) {
            if(entry.opcode_ != 0 && !entry.name_.empty()) {
                m.emplace(entry.name_, std::pair { InstFormat::U, entry.opcode_ });
            }
        }
        for(const auto &entry: SType::G_INST_TABLE) {
            if(entry.opcode_ != 0 && !entry.name_.empty()) {
                m.emplace(entry.name_, std::pair { InstFormat::S, entry.opcode_ });
            }
        }
        for(const auto &entry: BType::G_INST_TABLE) {
            if(entry.opcode_ != 0 && !entry.name_.empty()) {
                m.emplace(entry.name_, std::pair { InstFormat::B, entry.opcode_ });
            }
        }
        return m;
    }();
    return OPC_CACHE;
}

std::unique_ptr<IBaseInstType> InstTypeFactory::CreateType(std::vector<std::string> instAssembly, bool hasSetABI)
{
    if(instAssembly.empty()) {
        return nullptr;
    }

    auto match= matchInstName(instAssembly[0]);

    if(match) {
        const auto [fmt, opc]= *match;
        switch(fmt) {
        case InstFormat::R:
            return std::make_unique<RType>(std::move(instAssembly), fmt, hasSetABI);
        case InstFormat::I:
            return std::make_unique<IType>(std::move(instAssembly), fmt, hasSetABI);
        case InstFormat::J:
            return std::make_unique<JType>(std::move(instAssembly), fmt, hasSetABI);
        case InstFormat::U:
            return std::make_unique<UType>(std::move(instAssembly), fmt, hasSetABI);
        case InstFormat::S:
            return std::make_unique<SType>(std::move(instAssembly), fmt, hasSetABI);
        case InstFormat::B:
            return std::make_unique<BType>(std::move(instAssembly), fmt, hasSetABI);
        default:
            std::cout << "Unsupported instruction format\n";
            break;
        }
    } else {
        std::cout << "Unsupported instruction name: " << instAssembly[0] << '\n';
    }

    return nullptr;
}

template <typename T>
std::unique_ptr<IBaseInstType> InstTypeFactory::createHelper(T key, bool hasSetABI)
{
    if(auto it= G_Opcode2Format.find(key); it != G_Opcode2Format.end()) {
        switch(it->second) {
        case InstFormat::R:

            return std::make_unique<RType>(key, it->second, hasSetABI);
        default:
            std::cout << "Unsupported instruction format\n";
        }

    } else {
        std::cout << "not found key: " << key << '\n';
    }
    return nullptr;
}

std::optional<std::pair<InstFormat, uint16_t>> InstTypeFactory::matchInstName(std::string_view instName)
{
    const auto &cache= getName2FormatOpcode();
    auto it          = cache.find(instName);
    return it != cache.end() ? std::make_optional(it->second) : std::nullopt;
}
