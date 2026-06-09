# HAL integration tests: NS16550A driver, dev::Handle, dev::Manager (CRTP + Concepts).

set(HALTestInc
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/inc/Arch
    ${CMAKE_SOURCE_DIR}/inc/HAL
    ${CMAKE_SOURCE_DIR}/inc/Platform
    ${CMAKE_SOURCE_DIR}/inc/Util
    ${CMAKE_SOURCE_DIR}/tests/inc
)

if(RVTEST_SUPPORT_RV64)
    AddRISCVTst(uart0
        SOURCES drv/uart0.cc
        MARCH rv64gc
        ABI lp64d
        CMODEL medany
        INCLUDE_DIRS ${HALTestInc}
        LINK_LIBS HAL
    )
else()
    message(STATUS "Skipping HAL tests (RV64 toolchain required)")
endif()
