set(incDir
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/inc/Arch
    ${CMAKE_SOURCE_DIR}/inc/HAL
    ${CMAKE_SOURCE_DIR}/inc/Kernel
    ${CMAKE_SOURCE_DIR}/inc/Platform
    ${CMAKE_SOURCE_DIR}/inc/Util
)

add_executable(${CMAKE_PROJECT_NAME})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${incDir})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Kernel.cc
    Init/init.cc
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE HAL)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    ${_64bitCommonOpts}
    ${specsFile}
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    ${_64bitCommonOpts}
    ${specsFile}

    # -nostdlib
    # -nostartfiles
    # -nodefaultlibs
    -T ${linkerPath}/qemu-virt_rv.ld
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map
)
SetupCodeChecks(${CMAKE_PROJECT_NAME})
RunInQEMU(${CMAKE_PROJECT_NAME})
GenerateASMFile(${CMAKE_PROJECT_NAME} "-DCS")

add_dependencies(RVOS merge_compile_commands)
