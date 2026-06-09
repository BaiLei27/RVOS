# Picolibc / C++ STL toolchain smoke tests (no HAL dependency).

set(picolibcInc ${CMAKE_CURRENT_SOURCE_DIR})

if(RVTEST_SUPPORT_RV32)
    AddRISCVTst(picolibc_tst_c32
        SOURCES hw.c
        MARCH rv32gc
        ABI ilp32f
    )
    target_compile_definitions(picolibc_tst_c32 PRIVATE RVTEST_EXPECT_XLEN=32)

    AddRISCVTst(picolibc_tst_cc32
        SOURCES hw.cc uart.cc
        MARCH rv32gc
        ABI ilp32f
    )
else()
    message(STATUS "Skipping RV32 picolibc tests (toolchain unsupported)")
endif()

if(RVTEST_SUPPORT_RV64)
    AddRISCVTst(picolibc_tst_c64
        SOURCES hw.c
        MARCH rv64gc
        ABI lp64d
        CMODEL medany
    )
    target_compile_definitions(picolibc_tst_c64 PRIVATE RVTEST_EXPECT_XLEN=64)

    AddRISCVTst(picolibc_tst_cc64
        SOURCES hw.cc uart.cc
        MARCH rv64gc
        ABI lp64d
        CMODEL medany
    )
else()
    message(STATUS "Skipping RV64 picolibc tests (toolchain unsupported)")
endif()
