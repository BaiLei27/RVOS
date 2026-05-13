project(
    picolibc_tst
    VERSION 0.0.1
    LANGUAGES C CXX
)
include(CTest)

execute_process(
    COMMAND "${CMAKE_C_COMPILER}" --print-multi-lib
    OUTPUT_VARIABLE multiLibOutput
    RESULT_VARIABLE multiLibRes
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(supportRV32 OFF)
set(supportRV64 OFF)

if(multiLibRes EQUAL 0)
    string(REGEX MATCH "(^|[;\n])[^;\n]*rv32" hasRV32 "${multiLibOutput}")
    string(REGEX MATCH "(^|[;\n])[^;\n]*rv64" hasRV64 "${multiLibOutput}")

    if(hasRV32)
        set(supportRV32 ON)
    endif()

    if(hasRV64)
        set(supportRV64 ON)
    endif()
else()
    message(WARNING "Failed to run ${CMAKE_C_COMPILER} --print-multi-lib for detection")
endif()

if(supportRV64 AND supportRV32)
    message(STATUS "Toolchain supports RV32-gc and RV64-gc")
endif()

function(AddRISCVTarget target lang)
    set(options)
    set(oneValueArgs MARCH ABI CMODEL MAP_FILE)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "add_riscv_target(${target}): SOURCES is required")
    endif()

    add_executable(${target} ${ARG_SOURCES})

    set(compileOpts
        ${specsFile}
        -march=${ARG_MARCH}
        -mabi=${ARG_ABI}
    )

    set(linkOpts
        ${specsFile}
        --oslib=semihost
        -T ${linkerPath}/qemu-virt_rv.ld
        -march=${ARG_MARCH}
        -mabi=${ARG_ABI}
    )

    if(ARG_CMODEL)
        list(APPEND compileOpts -mcmodel=${ARG_CMODEL})
        list(APPEND linkOpts -mcmodel=${ARG_CMODEL})
    endif()

    target_compile_options(${target} PRIVATE ${compileOpts})
    target_link_options(${target} PRIVATE
        ${linkOpts}
        -Wl,-Map=${ARG_MAP_FILE}
    )

    GenerateASMFile(${target} "-DCS")
endfunction()

if(supportRV32)
    AddRISCVTarget(${PROJECT_NAME}_c32
        C
        SOURCES hw.c
        MARCH rv32gc
        ABI ilp32f
        MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_c32.map
    )

    AddRISCVTarget(${PROJECT_NAME}_cc32
        CXX
        SOURCES hw.cc uart.cc
        MARCH rv32gc
        ABI ilp32f
        MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_cc32.map
    )
else()
    message(WARNING "Skipping RV32 target build (unsupported by toolchain)")
endif()

if(supportRV64)
    AddRISCVTarget(${PROJECT_NAME}_c64
        C
        SOURCES hw.c
        MARCH rv64gc
        ABI lp64d
        CMODEL medany
        MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_c64.map
    )

    AddRISCVTarget(${PROJECT_NAME}_cc64
        CXX
        SOURCES hw.cc uart.cc
        MARCH rv64gc
        ABI lp64d
        CMODEL medany
        MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_cc64.map
    )
else()
    message(WARNING "Skipping RV64 target build (unsupported by toolchain)")
endif()
