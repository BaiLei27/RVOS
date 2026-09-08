
set(module Arch)
set(incArch
    ${CMAKE_SOURCE_DIR}/inc/Arch
    ${CMAKE_SOURCE_DIR}/inc/Util
)
set(bootPath ${CMAKE_CURRENT_LIST_DIR}/Boot CACHE PATH "boot entry directory")
set(linkerPath ${CMAKE_CURRENT_LIST_DIR}/Linker CACHE PATH "Linker script directory")

add_library(${module} STATIC
    CSR/CSRManager.cc
    Trap/Trap.cc
    Trap/trap_entry.S
)
target_include_directories(${module} PRIVATE ${incArch})

target_compile_options(${module} PRIVATE
    ${_64bitCommonOpts}
    ${specsFile}
)
SetupCodeChecks(${module})
