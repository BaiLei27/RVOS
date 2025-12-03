cmake_minimum_required(VERSION 3.20...3.27)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
include(ToolsChainConf)

project("RVOS"
VERSION "0.0.1"
HOMEPAGE_URL "https://github.com/Baiyi27/RVOS.git"
LANGUAGES CXX C ASM)

option(enableCTest "Enable Modules test" OFF)

include(CodeCheck)
find_package(Git)

# Library
add_subdirectory(lib)
DefineCodeChecksTargets("fmt" "tidy")

get_filename_component(builddir ${CMAKE_BINARY_DIR} NAME)
if(ClangFormat_FOUND OR  ClangTidy_FOUND)
    message(">> Optional: build the target 'tidy' or 'fmt' ('cmake --build ${builddir} -t help' to know more) before building 'all' target")
endif()
