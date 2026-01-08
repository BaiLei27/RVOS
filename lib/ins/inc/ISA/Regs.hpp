#pragma once

#include <array>
#include <optional>
#include <string>

// Fixed-size, compile-time mapping of register index -> (name, ABI)
namespace isa {

constexpr uint16_t G_REG_NUMBER= 32;

using strPair_u= std::pair<std::string_view, std::string_view>;

constexpr std::array<strPair_u, G_REG_NUMBER> G_INDEX_REGS_ABI= {
    { { "x0", "zero" },
     { "x1", "ra" },
     { "x2", "sp" },
     { "x3", "gp" },
     { "x4", "tp" },
     { "x5", "t0" },
     { "x6", "t1" },
     { "x7", "t2" },
     { "x8", "s0/fp" },
     { "x9", "s1" },
     { "x10", "a0" },
     { "x11", "a1" },
     { "x12", "a2" },
     { "x13", "a3" },
     { "x14", "a4" },
     { "x15", "a5" },
     { "x16", "a6" },
     { "x17", "a7" },
     { "x18", "s2" },
     { "x19", "s3" },
     { "x20", "s4" },
     { "x21", "s5" },
     { "x22", "s6" },
     { "x23", "s7" },
     { "x24", "s8" },
     { "x25", "s9" },
     { "x26", "s10" },
     { "x27", "s11" },
     { "x28", "t3" },
     { "x29", "t4" },
     { "x30", "t5" },
     { "x31", "t6" } }
};

// static_assert(G_INDEX_REGS_ABI.size() == 32, "G_INDEX_REGS_ABI must contain 32 entries");
std::optional<uint16_t> LOOKUP_REG_IDX(std::string &target);

// constexpr accessor: fast O(1) lookup by index at compile time
constexpr std::optional<strPair_u> PARSE_REG_NAME(size_t idx)
{
    if(idx >= G_INDEX_REGS_ABI.size()) return std::nullopt;

    return std::make_optional(G_INDEX_REGS_ABI[idx]);
}

constexpr std::string_view LOOKUP_REG_NAME(size_t regIdx, bool hasSetABI= false, std::string_view fallback= "?")
{
    auto regOpt= PARSE_REG_NAME(regIdx);
    if(!regOpt) {
        return fallback;
    }
    return hasSetABI ? regOpt->second : regOpt->first;
}

constexpr bool REG_COMPARE_LESS(std::string_view x, std::string_view y)
{
    if(x.size() != y.size()) {
        return x.size() < y.size();
    }
    return x < y;
}

constexpr bool REG_ENTRY_LESS(const strPair_u &regEntry, const std::string_view &targetName) noexcept
{
    return REG_COMPARE_LESS(regEntry.first, targetName);
}

} // namespace isa
