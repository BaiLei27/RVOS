cmake_minimum_required(VERSION 3.20...3.27)

set(subProjectPath ${CMAKE_SOURCE_DIR}/subprj/inst)
set(customModulesPath ${subProjectPath}/cmake/Modules CACHE PATH "custom cmake modules directory")
list(APPEND CMAKE_MODULE_PATH "${customModulesPath}")
include(ToolsChainConf)

set(CMAKE_CXX_STANDARD 23)

project(RVOS
    VERSION "0.0.2"
    HOMEPAGE_URL "https://github.com/Baiyi27/RVOS.git"
    LANGUAGES CXX C ASM
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(rootPath ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "project root directory")
set(rootBuilddir ${CMAKE_BINARY_DIR} CACHE PATH "project root builddir")
set(_64bitCommonOpts -march=rv64gc -mabi=lp64d -mcmodel=medany)

# target_compile_options as well as target_link_options. ASM needs picolibc.specs
# so Debug --oslib=semihost is recognized when assembling .S.
set(specsFile
    $<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:ASM>>:--specs=picolibc.specs>
    $<$<COMPILE_LANGUAGE:CXX>:--specs=picolibcpp.specs>
    $<$<CONFIG:Debug>:--oslib=semihost>
)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include(ExternalProject)
include(CodeCheck)
include(CustomTarget)
find_package(Git)

option(enableINST "Whether to enable 'RVINST'" OFF)
option(enableASM "Whether to enable 'asm'" OFF)
option(enableTests "Whether to enable 'tests'" OFF)

if(enableINST)
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
    add_dependencies(INST merge_compile_commands)
endif()

add_subdirectory(src/Arch)
add_subdirectory(src/HAL)
add_subdirectory(src/Kernel)

if(enableASM)
    add_subdirectory(asm)
endif(
)

if(enableTests)
    message(STATUS "Tests enabled — configure with: cmake --build <builddir> -t tests && ctest --test-dir <builddir>")
    set(CMAKE_BUILD_TYPE "Debug")
    enable_testing()
    add_subdirectory(tests)
endif()

# Must run after SetupCodeChecks() in subdirs have collected sources;
# otherwise 'fmt' 'tidy' see an empty file list and are not created.
DefineCodeChecksTargets("fmt" "tidy")

get_filename_component(builddir ${PROJECT_BINARY_DIR} NAME)

# get_filename_component(root ${rootBuilddir} NAME)
if(ClangFormat_FOUND OR ClangTidy_FOUND)
    message(">> Optional: build the '${PROJECT_NAME}' target 'tidy' or 'fmt' ('cmake --build ${builddir} -t help' to know more) before building 'all' target")
endif()

MergeCompileCmdFiles(${CMAKE_BINARY_DIR} ${INST_builddir})
