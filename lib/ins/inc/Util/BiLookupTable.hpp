#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

template <typename KeyT= uint16_t>
class BiLookupTable { // Bidirectional lookup table for code2name and name2code
public:
    struct NameInfo {
        std::string manual_;
        std::string_view XLEN_; // NOLINT
        std::string_view name_;
    };

    struct IndexInfo {
        std::string manual_;
        std::string_view XLEN_; // NOLINT
        KeyT funct_ {};
        uint16_t opcode_ {};
    };

    using intMapName_u = std::unordered_map<KeyT, NameInfo>;
    using strMapIndex_u= std::unordered_map<std::string_view, IndexInfo>;

    BiLookupTable(const intMapName_u &code2info,
                  const strMapIndex_u &name2info)
        : Code2NameInfo_(code2info),
          Name2IdxInfo_(name2info) { }

    BiLookupTable(const intMapName_u &&code2info,
                  const strMapIndex_u &&name2info)
        : Code2NameInfo_(std::move(code2info)),
          Name2IdxInfo_(std::move(name2info)) { }

    BiLookupTable(const BiLookupTable &)            = delete;
    BiLookupTable &operator= (const BiLookupTable &)= delete;
    BiLookupTable(BiLookupTable &&)                 = default;
    BiLookupTable &operator= (BiLookupTable &&)     = default;

public:
    std::optional<NameInfo> Find(KeyT code) const noexcept
    {
        auto it= Code2NameInfo_.find(code);
        return it != Code2NameInfo_.end() ? std::make_optional(it->second) : std::nullopt;
    }

    std::optional<IndexInfo> Find(std::string_view name) const noexcept
    {
        auto it= Name2IdxInfo_.find(name);
        return it != Name2IdxInfo_.end() ? std::make_optional(it->second) : std::nullopt;
    }

    bool Contains(KeyT code) const noexcept { return Code2NameInfo_.contains(code); }

    bool Contains(std::string_view name) const noexcept { return Name2IdxInfo_.contains(name); }

    void PrintCode2NameMap(std::ostream &os= std::cout) const noexcept
    {
        os << "==================================== Code -> Name Info Map ====================================\n";
        if(Code2NameInfo_.empty()) {
            os << "Map is empty.\n";
            return;
        }

        for(const auto &[key, name_info]: Code2NameInfo_) {
            os << "--------------------------------------------------------------------------------------------\n"
               << "  Code (functKey): 0x" << std::hex << std::setw(4) << std::setfill('0') << key << std::dec << '\n'
               << "  Instruction Name: " << name_info.name_ << '\n'
               << "  XLEN Architecture: " << name_info.XLEN_ << '\n'
               << "  Manual URL: " << name_info.manual_ << '\n';
        }
        os << "==============================================================================================\n\n";
    }

    void PrintName2IndexMap(std::ostream &os= std::cout) const noexcept
    {
        os << "==================================== Name -> Index Info Map ===================================\n";
        if(Name2IdxInfo_.empty()) {
            os << "Map is empty.\n";
            return;
        }

        for(const auto &[name, index_info]: Name2IdxInfo_) {
            os << "--------------------------------------------------------------------------------------------\n"
               << "  Instruction Name: " << name << '\n'
               << "  XLEN Architecture: " << index_info.XLEN_ << '\n'
               << "  FunctKey: 0x" << std::hex << std::setw(4) << std::setfill('0') << index_info.funct_ << std::dec << '\n'
               << "  Opcode: 0x" << std::hex << std::setw(2) << std::setfill('0') << index_info.opcode_ << std::dec << '\n'
               << "  Manual URL: " << index_info.manual_ << '\n';
        }
        os << "==============================================================================================\n\n";
    }

private:
    strMapIndex_u Name2IdxInfo_;
    intMapName_u Code2NameInfo_;
};

// Date:25/12/21/12:49
