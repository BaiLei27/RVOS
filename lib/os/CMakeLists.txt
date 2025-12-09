cmake_minimum_required(VERSION 3.20...3.27)

# list(APPEND CMAKE_MODULE_PATH "${rootPath}/cmake/Modules")
include(ToolsChainConf)

project(RVOS
    VERSION "0.0.1"
    HOMEPAGE_URL "https://github.com/Baiyi27/RVOS.git"
    LANGUAGES CXX C ASM
)
include(CodeCheck)
include(CustomTarget)
find_package(Git)

set(ldLinkPath ${CMAKE_CURRENT_LIST_DIR}/Startup CACHE PATH "Linker script directory")

add_executable(${PROJECT_NAME})
add_subdirectory(asm)

option(enableCTest "Enable Modules test" OFF)

target_sources(${PROJECT_NAME} PRIVATE
    src/kernel.cc
)

target_link_options(${PROJECT_NAME} PRIVATE
    -Ttext=0x80000000
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
)
SetupCodeChecks(${PROJECT_NAME})

DefineCodeChecksTargets("fmt" "tidy")
get_filename_component(builddir ${PROJECT_BINARY_DIR} NAME)
get_filename_component(root ${rootBuilddir} NAME)

if(ClangFormat_FOUND OR ClangTidy_FOUND)
    message(">> Optional: build the target 'tidy' or 'fmt' ('cmake --build ${root}/${builddir} -t help' to know more) before building 'all' target")
endif()
