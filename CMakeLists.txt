cmake_minimum_required(VERSION 3.20...3.27)

project(multipleProjects)

include(ExternalProject)

set(rootPath ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "project root directory")
set(rootBuilddir ${CMAKE_BINARY_DIR} CACHE PATH "project root builddir")
set(customModulesPath ${rootPath}/cmake/Modules CACHE PATH "custom modules directory")

set(subProjectPath ${CMAKE_SOURCE_DIR}/lib)
set(RVOS_builddir ${CMAKE_BINARY_DIR}/build-RVOS)
set(INST_builddir ${CMAKE_BINARY_DIR}/build-INST)

list(APPEND CMAKE_MODULE_PATH "${customModulesPath}")
include(CustomTarget)

# ExternalProject_Add(
#     RVOS
#     SOURCE_DIR ${subProjectPath}/os
#     BINARY_DIR ${RVOS_builddir}
#     CMAKE_ARGS
#     -DCMAKE_C_COMPILER=riscv64-elf-gcc
#     -DCMAKE_CXX_COMPILER=riscv64-elf-g++
#     -DCMAKE_MODULE_PATH=${customModulesPath}
#     -DrootPath=${rootPath}
#     -DrootBuilddir=${rootBuilddir}
#     INSTALL_COMMAND ""
# )

ExternalProject_Add(
    INST
    SOURCE_DIR ${subProjectPath}/ins
    BINARY_DIR ${INST_builddir}
    CMAKE_ARGS
    -DCMAKE_CXX_COMPILER=g++
    -DCMAKE_MODULE_PATH=${customModulesPath}
    -DrootPath=${rootPath}
    -DrootBuilddir=${rootBuilddir}
    INSTALL_COMMAND ""
)

MergeCompileCmdFiles(${RVOS_builddir} ${INST_builddir})
# add_dependencies(RVOS merge_compile_commands)
add_dependencies(INST merge_compile_commands)
