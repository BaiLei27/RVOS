# Arch trap test sources (reused by kernel integration flow).

set(TrapTestInc
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/inc/Arch
    ${CMAKE_SOURCE_DIR}/inc/HAL
    ${CMAKE_SOURCE_DIR}/inc/Platform
    ${CMAKE_SOURCE_DIR}/inc/Util
    ${CMAKE_CURRENT_LIST_DIR}
)

if(RVTEST_SUPPORT_RV64)
    AddRISCVTst(trap_sync_test
        SOURCES
            Trap.cc
            main.cc
        MARCH rv64gc
        ABI lp64d
        CMODEL medany
        INCLUDE_DIRS ${TrapTestInc}
        LINK_LIBS Arch
    )
else()
    message(STATUS "Skipping trap tests (RV64 toolchain required)")
endif()