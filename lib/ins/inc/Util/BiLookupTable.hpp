#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <tuple>

template <typename KeyT= uint16_t>
class BiLookupTable { // Bidirectional lookup table for code2name and name2code
public:
    using nameAndInfo_u= std::tuple<std::string_view, std::string_view, std::string>; // name, XLEN, manual
    using idxAndInfo_u = std::tuple<KeyT, std::string_view, std::string>;             // funct key, XLEN, manual

    using intMapTup_u= std::unordered_map<KeyT, nameAndInfo_u>;
    using strMapTup_u= std::unordered_map<std::string_view, idxAndInfo_u>;

    BiLookupTable(const intMapTup_u &code2tuple,
                  const strMapTup_u &tuple2code)
        : Code2Info_(code2tuple),
          Name2Info_(tuple2code) { }

    BiLookupTable(const intMapTup_u &&code2tuple,
                  const strMapTup_u &&tuple2code)
        : Code2Info_(code2tuple),
          Name2Info_(tuple2code) { }

    BiLookupTable(const BiLookupTable &)            = delete;
    BiLookupTable &operator= (const BiLookupTable &)= delete;
    BiLookupTable(BiLookupTable &&)                 = default;
    BiLookupTable &operator= (BiLookupTable &&)     = default;

    std::optional<nameAndInfo_u> Find(KeyT code) const noexcept
    {
        auto it= Code2Info_.find(code);
        return it != Code2Info_.end() ? std::make_optional(it->second) : std::nullopt;
    }

    std::optional<idxAndInfo_u> Find(std::string_view name) const noexcept
    {
        auto it= Name2Info_.find(name);
        return it != Name2Info_.end() ? std::make_optional(it->second) : std::nullopt;
    }

    bool Contains(KeyT code) const noexcept { return Code2Info_.contains(code); }

    bool Contains(std::string_view name) const noexcept { return Name2Info_.contains(name); }

private:
    intMapTup_u Code2Info_;
    strMapTup_u Name2Info_;
};

// Date:25/12/21/12:49
