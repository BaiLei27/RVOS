
set(module Arch)
set(incArch ${CMAKE_SOURCE_DIR}/inc/Arch)
set(bootPath ${CMAKE_CURRENT_LIST_DIR}/Boot CACHE PATH "boot entry directory")
set(linkerPath ${CMAKE_CURRENT_LIST_DIR}/Linker CACHE PATH "Linker script directory")

set(srcArch
    ${CMAKE_CURRENT_LIST_DIR}/CSR/CSRManager.cc
    ${CMAKE_CURRENT_LIST_DIR}/Trap/Trap.cc
    ${CMAKE_CURRENT_LIST_DIR}/Trap/trap_entry.S
)

add_library(${module} STATIC ${srcArch})
target_include_directories(${module} PRIVATE ${incArch})

target_compile_options(${module} PRIVATE
    ${_64bitCommonOpts}
    ${specsFile}
)
SetupCodeChecks(${module})