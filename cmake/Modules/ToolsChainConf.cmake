# Setup compiler settings
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# set(CMAKE_C_COMPILER_FORCED ON)
# set(CMAKE_CXX_COMPILER_FORCED ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()


# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-missing-field-initializer -Wpedantic -nostdlib -nostartfiles -nodefaultlibs -fdata-sections -ffunction-sections")
# set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group -Wl,--gc-sections")
# set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--print-memory-usage")

add_compile_options(
    -fdiagnostics-color=always
    -ffunction-sections
    -fdata-sections
    -ffreestanding
    -fno-builtin
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti;-fno-exceptions>"
    "$<$<CONFIG:Debug>:-ggdb3;-DDEBUG>"
    $<$<CONFIG:Release,MinSizeRel>:-DNDEBUG>
)

add_link_options(
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    $<$<CONFIG:Release,MinSizeRel>:-s>
)

message("++ Generator ${CMAKE_GENERATOR}")
message("++ Build type: ${CMAKE_BUILD_TYPE}")
message("++ CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("++ CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("++ CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message("++ CMAKE_FIND_ROOT_PATH: ${CMAKE_FIND_ROOT_PATH}")
