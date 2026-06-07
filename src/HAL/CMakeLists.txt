
set(module HAL)
set(halIncDir
    ${CMAKE_SOURCE_DIR}/inc/HAL
    ${CMAKE_SOURCE_DIR}/inc/Util
)

add_library(${module} STATIC
    Device/DevManager.cc
)

target_include_directories(${module} PUBLIC ${halIncDir})

target_compile_options(${module} PRIVATE
    ${_64bitCommonOpts}
    ${specsFile}
)
