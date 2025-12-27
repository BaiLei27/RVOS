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

function(MergeCompileCmdFiles)
    set(subBuilddir ${ARGN})

    if(NOT subBuilddir)
        message(WARNING "MergeCompileCmdFiles: At least one 'compile_commands.json' path is required")
        return()
    endif()

    set(jsonName compile_commands.json)
    set(outputFile ${CMAKE_BINARY_DIR}/${jsonName})

    foreach(dir IN LISTS subBuilddir)
        set(f "${dir}/${jsonName}")
        list(APPEND inputFiles ${f})

        if(NOT IS_DIRECTORY ${dir})
            message(WARNING "MergeCompileCmdFiles: Not a directory: ${dir}")
        endif()
    endforeach()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    add_custom_target(
        merge_compile_commands
        # ALL
        COMMAND ${Python3_EXECUTABLE} ${rootPath}/scripts/python/mg_compile_cmd_files.py ${outputFile} ${inputFiles}
        # DEPENDS RVOS INST
        COMMENT "Merging '${jsonName}' files to '${outputFile}'"
        VERBATIM
    )
endfunction()
