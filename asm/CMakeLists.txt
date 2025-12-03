set(target asm)

add_executable(${target})

target_sources(${target} PRIVATE
    ${ldLinkPath}/asm_start.S
    main.c
    add.s
    addi.s
)

target_link_options(${target} PRIVATE
    -T ${ldLinkPath}/_asm_start.ld
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${target}.map
)

SetupCodeChecks(${target})
