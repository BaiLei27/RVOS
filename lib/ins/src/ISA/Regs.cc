#include <algorithm>
#include <charconv>
#include <numeric>
#include <vector>
// #include <iostream>
#include "ISA/Regs.hpp"

namespace isa {
bool CheckIfDigit(std::string_view target, int (*proj)(int))
{
    return std::ranges::all_of(target, [&](unsigned char c) { return proj(c); });
}

std::optional<uint16_t> Str2Int(std::string_view target, int base)
{
    uint16_t result= 0;
    auto [ptr, ec] = std::from_chars(target.data(), target.data() + target.size(), result, base);
    if(ec == std::errc()) {
        // std::cout << "res " << result << '\n';
        return std::make_optional<uint16_t>(result);
    }
    return std::nullopt;
}

std::optional<uint16_t> LOOKUP_REG_IDX(std::string &target)
{
    // std::ranges::transform(target, target.begin(), [](unsigned char c) { return std::tolower(c); });

    // std::cout << target << "\n";
    target[0]= static_cast<char>(std::tolower(target[0])); // only lower first char
#if 0
    if(target.starts_with('x')) { // deal x*

        const auto *pIt= std::lower_bound( // NOLINT
            G_INDEX_REGS_ABI.begin(),
            G_INDEX_REGS_ABI.end(),
            target,
            REG_ENTRY_LESS);
        if(pIt != G_INDEX_REGS_ABI.end()) { //
            return std::make_optional<uint16_t>(std::distance(G_INDEX_REGS_ABI.begin(), pIt));
        }
#else
    if(target.starts_with('x') && CheckIfDigit(target.substr(1), std::isdigit)) {
        return Str2Int(target.substr(1), 10);
#endif
    } else if(CheckIfDigit(target, std::isdigit)) {
        return Str2Int(target, 10);

    } else if(target.starts_with("0x") && CheckIfDigit(target.substr(2), std::isxdigit)) {
        return Str2Int(target.substr(2), 16);

    } else {
        if("s0" == target || "fp" == target) {
            return std::make_optional<uint16_t>(8);
        }

        static const std::vector<size_t> S_SORTED_BY_NAME_SECOND= [] {
            std::vector<size_t> idx(G_REG_NUMBER);
#if __cplusplus >= 202100L
            std::ranges::iota(idx, 0);
#else
            std::iota(idx.begin(), idx.end(), 0);
#endif
            std::ranges::sort(idx, {}, [](size_t i) { return G_INDEX_REGS_ABI[i].second; });
            return idx;
        }();

        auto it= std::ranges::lower_bound(
            S_SORTED_BY_NAME_SECOND,
            target,
            std::ranges::less {},
            [](size_t i) { return G_INDEX_REGS_ABI[i].second; });
        if(it != S_SORTED_BY_NAME_SECOND.end() && G_INDEX_REGS_ABI[*it].second == target) {
            return std::make_optional<uint16_t>(*it);
        }
    }
    return std::nullopt;
}

#if 0
 std::optional<size_t> LOOKUP_REG_IDX1(std::string_view name)
{
    for(size_t i= 0; i < G_INDEX_REGS_ABI.size(); ++i) {
        if(G_INDEX_REGS_ABI[i].first == name || G_INDEX_REGS_ABI[i].second == name) {
            return i;
        }
    }
    return std::nullopt;
}

constexpr std::optional<size_t> PARSE_REG_IDX(std::string_view name) noexcept
{
    if(name.starts_with('x')) {
        std::string_view numStr(name.substr(1));
        size_t num    = 0;
        auto [ptr, ec]= std::from_chars(numStr.data(), numStr.data() + numStr.size(), num); // NOLINT
        if(ec == std::errc() && num < G_REG_NUMBER) {
            return num;
        }
        return std::nullopt;
    }

    for(size_t i= 0; i < G_REG_NUMBER; ++i) {
        if(G_INDEX_REGS_ABI[i].second == name) {
            return i;
        }
    }

    return std::nullopt;
}

template <typename Getter>
static std::vector<size_t> BuildSortedIndexes(Getter &&getter)
{
    std::vector<size_t> idx(G_REG_NUMBER);
    std::iota(idx.begin(), idx.end(), 0);

    std::ranges::sort(idx, {}, std::forward<Getter>(getter));
    return idx;
}

template <typename Getter>
static std::optional<size_t> BinarySearch(
    const std::vector<size_t> &sortedIndexes,
    std::string_view name,
    Getter &&getter)
{

    auto it= std::ranges::lower_bound(
        sortedIndexes,
        name,
        std::ranges::less {},
        std::forward<Getter>(getter));

    if(it != sortedIndexes.end() && getter(*it) == name) {
        return *it;
    }
    return std::nullopt;
}

 std::optional<size_t> LOOKUP_REG_IDX2(std::string_view name)
{
    static const auto SORTED_BY_FIRST= BuildSortedIndexes(
        [](size_t i) { return G_INDEX_REGS_ABI[i].first; });

    auto idx= BinarySearch(SORTED_BY_FIRST, name, [](size_t i) { return G_INDEX_REGS_ABI[i].first; });
    if(idx) return idx;

    static const auto SORTED_BY_SECOND= BuildSortedIndexes(
        [](size_t i) { return G_INDEX_REGS_ABI[i].second; });

    return BinarySearch(SORTED_BY_SECOND, name, [](size_t i) { return G_INDEX_REGS_ABI[i].second; });
}

 std::optional<size_t> LOOKUP_REG_IDX3(std::string_view name)
{
    static const std::vector<size_t> SORTED_BY_NAME_FIRST= [] {
        std::vector<size_t> idx(G_REG_NUMBER);
        std::iota(idx.begin(), idx.end(), 0);
        std::ranges::sort(idx, {}, [](size_t i) { return G_INDEX_REGS_ABI[i].first; });
        return idx;
    }();

    {
        auto it= std::ranges::lower_bound(
            SORTED_BY_NAME_FIRST,
            name,
            std::ranges::less {},
            [](size_t i) { return G_INDEX_REGS_ABI[i].first; });
        if(it != SORTED_BY_NAME_FIRST.end() && G_INDEX_REGS_ABI[*it].first == name) {
            return *it;
        }
    }

    static const std::vector<size_t> SORTED_BY_NAME_SECOND= [] {
        std::vector<size_t> idx(G_REG_NUMBER);
        std::iota(idx.begin(), idx.end(), 0);
        std::ranges::sort(idx, {}, [](size_t i) { return G_INDEX_REGS_ABI[i].second; });
        return idx;
    }();

    {
        auto it= std::ranges::lower_bound(
            SORTED_BY_NAME_SECOND,
            name,
            std::ranges::less {},
            [](size_t i) { return G_INDEX_REGS_ABI[i].second; });
        if(it != SORTED_BY_NAME_SECOND.end() && G_INDEX_REGS_ABI[*it].second == name) {
            return *it;
        }
    }

    return std::nullopt;
}
#endif
} // namespace isa
