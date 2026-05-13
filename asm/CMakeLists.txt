set(module asm)

add_executable(${module})

target_sources(${module} PRIVATE
    ${bootPath}/asm_start.S
    main.c
    add.s
    addi.s
)

target_link_options(${module} PRIVATE
    -nostdlib
    -T ${linkerPath}/_asm_start.ld
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${module}.map
)

SetupCodeChecks(${module})
RunInQEMU(${module})
GenerateASMFile(${module} "-DCS")
