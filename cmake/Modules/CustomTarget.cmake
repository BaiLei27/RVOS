# CustomTarget.cmake - Define custom targets E.g RunInQEMU ...
set(scriptsPath ${rootPath}/scripts)

function(RunInQEMU target)
    set(cmds
        qemu-system-riscv64
        -nographic
        -smp 1
        -machine virt
        -bios none
        -serial mon:stdio
        -no-reboot
        -kernel ${target}
    )

    add_custom_target(
        run-${target}
        COMMAND ${cmds}
        DEPENDS ${target}
        COMMENT "Run ${target} in QEMU"
    )
    add_custom_target(
        dbg-${target}
        COMMAND bash "${scriptsPath}/shell/qemu_debug.sh" ${target}
        DEPENDS ${target}
        COMMENT "Run ${target} in QEMU and attach GDB"
        VERBATIM
        USES_TERMINAL
    )

    if(target STREQUAL "RVOS")
        add_custom_target(run DEPENDS run-${target})
        add_custom_target(dbg DEPENDS dbg-${target})
    endif()
endfunction()

function(GenerateASMFile target args)
    add_custom_target(${target}.asm
        COMMAND ${CMAKE_OBJDUMP} ${args} ${target} > ${target}.asm
        DEPENDS ${target}
        COMMENT "Generating '${target}.asm' file"
    )
endfunction()
