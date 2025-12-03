set(target instructions)

add_executable(${target}
    Startup/start.S
    main.c
    asm/add.s
    asm/addi.s
)

target_link_options(${target} PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/Startup/_start.ld
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${target}.map
)
SetupCodeChecks(${target})
