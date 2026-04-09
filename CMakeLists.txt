cmake_minimum_required(VERSION 3.20...3.27)

set(subProjectPath ${CMAKE_SOURCE_DIR}/subprj/inst)
set(customModulesPath ${subProjectPath}/cmake/Modules CACHE PATH "custom modules directory")
list(APPEND CMAKE_MODULE_PATH "${customModulesPath}")
include(ToolsChainConf)

project(RVOS
    VERSION "0.0.1"
    HOMEPAGE_URL "https://github.com/Baiyi27/RVOS.git"
    LANGUAGES CXX C ASM
)

set(rootPath ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "project root directory")
set(rootBuilddir ${CMAKE_BINARY_DIR} CACHE PATH "project root builddir")
set(ldLinkPath ${CMAKE_CURRENT_LIST_DIR}/Startup CACHE PATH "Linker script directory")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

include(ExternalProject)
include(CodeCheck)
include(CustomTarget)
find_package(Git)

set(INST_builddir ${CMAKE_BINARY_DIR}/build-INST)

ExternalProject_Add(
    INST
    SOURCE_DIR ${subProjectPath}
    BINARY_DIR ${INST_builddir}

    CMAKE_ARGS
    -DCMAKE_MODULE_PATH=${customModulesPath}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DrootBuilddir=${rootBuilddir}

    # -DrootPath=${rootPath}
    INSTALL_COMMAND ""
)

add_executable(${PROJECT_NAME})
add_subdirectory(asm)

target_include_directories(${PROJECT_NAME} PRIVATE
    inc
)

target_sources(${PROJECT_NAME} PRIVATE

    src/kernel.cc
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -march=rv64gcv
    -mabi=lp64d
    -mcmodel=medany
)

target_link_options(${PROJECT_NAME} PRIVATE
    -nostdlib
    # -nostartfiles
    # -nodefaultlibs
    -lstdc++
    -lc
    -lgcc

    -Ttext=0x80000000
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map
)
SetupCodeChecks(${PROJECT_NAME})

DefineCodeChecksTargets("fmt" "tidy")
get_filename_component(builddir ${PROJECT_BINARY_DIR} NAME)

# get_filename_component(root ${rootBuilddir} NAME)
if(ClangFormat_FOUND OR ClangTidy_FOUND)
    message(">> Optional: build the '${PROJECT_NAME}' target 'tidy' or 'fmt' ('cmake --build ${builddir} -t help' to know more) before building 'all' target")
endif()

MergeCompileCmdFiles(${CMAKE_BINARY_DIR} ${INST_builddir})
add_dependencies(RVOS merge_compile_commands)
add_dependencies(INST merge_compile_commands)
