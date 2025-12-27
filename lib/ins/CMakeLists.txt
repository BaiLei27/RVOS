cmake_minimum_required(VERSION 3.20...3.27)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

project(INST
    VERSION "0.0.1"
    HOMEPAGE_URL "https://github.com/Baiyi27/RVOS.git"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

add_library(${PROJECT_NAME}_backEnd SHARED)
set_target_properties(${PROJECT_NAME}_backEnd PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}

    # OUTPUT_NAME "backEnd"
)

add_compile_options(
    -fdiagnostics-color=always
    -ffunction-sections
    -fdata-sections
    "$<$<CONFIG:Debug>:-ggdb3;-DDEBUG>"
    $<$<CONFIG:Release,MinSizeRel>:-DNDEBUG>
)

add_link_options(
    -Wl,--gc-sections
    -flto
    $<$<CONFIG:Release,MinSizeRel>:-s>
)

target_sources(${PROJECT_NAME}_backEnd PRIVATE

    # inc/Util/Bitops.hh
    inc/ISA/InstFormat.hh
    inc/ISA/Regs.hh

    inc/Core/Instruction.hh
    src/Core/Instruction.cc

    inc/Core/InstTypeFactory.hh
    src/Core/InstTypeFactory.cc
    inc/Core/IBaseInstType.hh
    src/Core/IBaseInstType.cc
    inc/Core/InstType.hh
    src/Core/InstType.cc
)

add_executable(
    ${PROJECT_NAME}
    tests/INST.cc
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ".exe"
)
include_directories(
    inc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}_backEnd
)
