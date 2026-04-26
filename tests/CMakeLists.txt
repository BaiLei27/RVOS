project(
    picolibc_tst
    VERSION 0.0.1
    LANGUAGES C CXX
)

add_executable(${PROJECT_NAME}_c32 hw.c)
add_executable(${PROJECT_NAME}_c64 hw.c)

set(commonCompileOptions
    --specs=picolibc.specs
    --oslib=semihost
)

set(commonLinkOptions
    ${commonCompileOptions}
    -T ${ldLinkPath}/qemu-virt_rv.ld
)
set(_32bitCompileAndLinkOpts -march=rv32imac -mabi=ilp32)
set(_64bitCompileAndLinkOpts -march=rv64gcv -mabi=lp64d -mcmodel=medany)

target_compile_options(${PROJECT_NAME}_c32 PRIVATE
    ${commonCompileOptions}
    ${_32bitCompileAndLinkOpts}
)

target_link_options(${PROJECT_NAME}_c32 PRIVATE
    ${commonLinkOptions}
    ${_32bitCompileAndLinkOpts}
)

target_compile_options(${PROJECT_NAME}_c64 PRIVATE
    ${commonCompileOptions}
    ${_64bitCompileAndLinkOpts}
)

target_link_options(${PROJECT_NAME}_c64 PRIVATE
    ${commonLinkOptions}
    ${_64bitCompileAndLinkOpts}
)
