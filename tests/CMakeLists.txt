# tests/ — bare-metal integration tests (QEMU virt + picolibc)
include(CTest)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/RISCV_XLEN.cmake)

RiscvDetectMultilib(RVTEST_SUPPORT_RV32 RVTEST_SUPPORT_RV64)

if(RVTEST_SUPPORT_RV64 AND RVTEST_SUPPORT_RV32)
    message(STATUS "Test toolchain: RV32-gc + RV64-gc")
elseif(RVTEST_SUPPORT_RV64)
    message(STATUS "Test toolchain: RV64-gc only")
elseif(RVTEST_SUPPORT_RV32)
    message(STATUS "Test toolchain: RV32-gc only")
endif()

add_subdirectory(picolibc)
add_subdirectory(hal)
add_subdirectory(arch/trap)

add_custom_target(tests
    COMMENT "Build all test executables (use 'ctest' or 'cmake --build . -t run-<target>')"
)

foreach(_tst IN ITEMS
    picolibc_tst_c64 picolibc_tst_cc64
    uart0
    trap_sync_test
)
    if(TARGET ${_tst})
        add_dependencies(tests ${_tst})
    endif()
endforeach()

if(RVTEST_SUPPORT_RV32)
    foreach(_tst IN ITEMS picolibc_tst_c32 picolibc_tst_cc32)
        if(TARGET ${_tst})
            add_dependencies(tests ${_tst})
        endif()
    endforeach()
endif()
