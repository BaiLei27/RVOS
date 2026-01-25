cmake_minimum_required(VERSION 3.25...3.27)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

project(INST
    VERSION "0.0.1"
    HOMEPAGE_URL "https://github.com/Baiyi27/RVOS.git"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

include(CheckIPOSupported)
include(CodeCheck)

# check_ipo_supported()
check_ipo_supported(RESULT _ipo_supported)
if(_ipo_supported)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-flto=full)
  else()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  endif()
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

add_compile_options(
    -ffunction-sections
    -fdata-sections
    "$<$<CONFIG:Debug>:-ggdb3;-DDEBUG>"
    $<$<CONFIG:Release,MinSizeRel>:-DNDEBUG>
)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_|amd|AMD)64$")
    add_compile_options(
        -march=native
    )
endif()

add_link_options(
    -Wl,--gc-sections
    $<$<CONFIG:Release,MinSizeRel>:-s>
)

set(backend ${PROJECT_NAME}_backend)
add_library(${backend} SHARED)
add_executable(
    ${PROJECT_NAME}
    tests/INST.cc
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    SUFFIX ".exe"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${backend}
    PRIVATE ${GTKMM_LIBRARIES}
)

add_executable(
    ${PROJECT_NAME}_gui
    src/Gui/Main.cc
)

target_link_libraries(${PROJECT_NAME}_gui PRIVATE
    ${backend}
    ${GTKMM_LIBRARIES}
)

set_target_properties(${PROJECT_NAME}_gui PROPERTIES
    SUFFIX ".exe"
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/res)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui/InstStyle.css
    ${CMAKE_CURRENT_BINARY_DIR}/res/InstStyle.css
    COPYONLY
)

target_compile_definitions(${PROJECT_NAME}_gui PUBLIC
    CSS_FILE_PATH="${CMAKE_CURRENT_BINARY_DIR}/res/InstStyle.css"
)

set_target_properties(${backend} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}

    # OUTPUT_NAME "backEnd"
)

include_directories(SYSTEM
    inc
    ${GTKMM_INCLUDE_DIRS}
)

target_sources(${backend} PRIVATE
    inc/ISA/Regs.hpp
    src/ISA/Regs.cc

    inc/Util/BiLookupTable.hpp
    inc/Log/Logger.hpp

    inc/Core/IBaseInstType.hh
    src/Core/IBaseInstType.cc

    inc/ISA/InstFormat.hh
    src/ISA/InstFormat.cc

    inc/Core/RType.hh
    src/Core/RType.cc

    inc/Core/InstTypeFactory.hh
    src/Core/InstTypeFactory.cc

    inc/Core/Instruction.hh
    src/Core/Instruction.cc

)

# GUI sources belong to the GUI executable target so GUI-specific
# compile definitions (like CSS_FILE_PATH) apply to them.
target_sources(${PROJECT_NAME}_gui PRIVATE
    inc/Gui/RISCVApplication.hh
    src/Gui/RISCVApplication.cc

    inc/Gui/RISCVInstructionWindow.hh
    src/Gui/RISCVInstructionWindow.cc
)

SetupCodeChecks(${backend} ${PROJECT_NAME} ${PROJECT_NAME}_gui)

DefineCodeChecksTargets("fmt" "tidy")
get_filename_component(builddir ${PROJECT_BINARY_DIR} NAME)
get_filename_component(root ${rootBuilddir} NAME)

if(ClangFormat_FOUND OR ClangTidy_FOUND)
    message(">> Optional: build the target 'tidy' or 'fmt' ('cmake --build ${root}/${builddir} -t help' to know more) before building 'all' target")
endif()
