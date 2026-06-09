# Shared helpers for bare-metal RISC-V test executables (picolibc / HAL).

if(NOT DEFINED linkerPath)
    message(FATAL_ERROR "RISCV_XLEN.cmake: linkerPath is not set (include src/Arch first)")
endif()

function(RiscvDetectMultilib outRv32 outRv64)
    execute_process(
        COMMAND "${CMAKE_C_COMPILER}" --print-multi-lib
        OUTPUT_VARIABLE multiLibOutput
        RESULT_VARIABLE multiLibRes
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(_rv32 OFF)
    set(_rv64 OFF)

    if(multiLibRes EQUAL 0)
        string(REGEX MATCH "(^|[;\n])[^;\n]*rv32" hasRV32 "${multiLibOutput}")
        string(REGEX MATCH "(^|[;\n])[^;\n]*rv64" hasRV64 "${multiLibOutput}")

        if(hasRV32)
            set(_rv32 ON)
        endif()

        if(hasRV64)
            set(_rv64 ON)
        endif()
    else()
        message(WARNING "Failed to run ${CMAKE_C_COMPILER} --print-multi-lib for detection")
    endif()

    set(${outRv32} ${_rv32} PARENT_SCOPE)
    set(${outRv64} ${_rv64} PARENT_SCOPE)
endfunction()

# AddRISCVTst(<target>
#   SOURCES <files...>
#   [MARCH rv64gc] [ABI lp64d] [CMODEL medany]
#   [LINK_LIBS <libs...>]
#   [INCLUDE_DIRS <dirs...>]
#   [SEMHOST ON|OFF]         # default ON for picolibc-style tests
# )
function(AddRISCVTst target)
    set(options)
    set(oneValueArgs MARCH ABI CMODEL SEMHOST)
    set(multiValueArgs SOURCES LINK_LIBS INCLUDE_DIRS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "AddRISCVTst(${target}): SOURCES is required")
    endif()

    if(NOT ARG_MARCH)
        set(ARG_MARCH rv64gc)
    endif()

    if(NOT ARG_ABI)
        if(ARG_MARCH MATCHES "rv32")
            set(ARG_ABI ilp32f)
        else()
            set(ARG_ABI lp64d)
        endif()
    endif()

    if(NOT ARG_SEMHOST)
        set(ARG_SEMHOST ON)
    endif()

    add_executable(${target} ${ARG_SOURCES})

    set(compileOpts
        ${specsFile}
        -march=${ARG_MARCH}
        -mabi=${ARG_ABI}
    )

    set(linkOpts
        ${specsFile}
        -T ${linkerPath}/qemu-virt_rv.ld
        -march=${ARG_MARCH}
        -mabi=${ARG_ABI}
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${target}.map
    )

    if(ARG_SEMHOST)
        list(APPEND linkOpts --oslib=semihost)
    endif()

    if(ARG_CMODEL)
        list(APPEND compileOpts -mcmodel=${ARG_CMODEL})
        list(APPEND linkOpts -mcmodel=${ARG_CMODEL})
    endif()

    target_compile_options(${target} PRIVATE ${compileOpts})
    target_link_options(${target} PRIVATE ${linkOpts})

    if(ARG_INCLUDE_DIRS)
        target_include_directories(${target} PRIVATE ${ARG_INCLUDE_DIRS})
    endif()

    if(ARG_LINK_LIBS)
        target_link_libraries(${target} PRIVATE ${ARG_LINK_LIBS})
    endif()

    GenerateASMFile(${target} "-DCS")
    RunInQEMU(${target})

    add_test(
        NAME ${target}
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/shell/qemu_run.sh
                $<TARGET_FILE:${target}>
    )
    set_tests_properties(${target} PROPERTIES TIMEOUT 60 LABELS "qemu")
endfunction()
