set(ldLinkPath ${CMAKE_CURRENT_LIST_DIR}/Startup CACHE PATH "Linker script directory")

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    src/kernel.cc
)

target_link_options(${PROJECT_NAME} PRIVATE
    -Ttext=0x80000000
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
)

add_subdirectory(asm)

SetupCodeChecks(${PROJECT_NAME})
