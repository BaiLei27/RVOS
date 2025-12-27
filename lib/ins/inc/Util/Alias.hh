#pragma once
#include <tuple>
#include <unordered_map>
#include <string_view>

using nameAndXLEN_u= std::tuple<std::string_view, std::string_view>; // name, XLEN



using idxAndXLEN_u = std::tuple<KeyT, std::string_view>;

using intMapTup_u= std::unordered_map<KeyT, nameAndXLEN_u>;
using strMapTup_u= std::unordered_map<std::string_view, idxAndXLEN_u>;

// Date:25/12/26/16:32
