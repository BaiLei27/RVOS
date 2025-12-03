#[=======================================================================[.rst:
FindClangFormat
---------------
The module defines the following variables:

``clangFormatExe``
  Path to ClangFormat command-line client.
``ClangFormat_FOUND``, ``CLANGFORMAT_FOUND``
  True if the ClangFormat command-line client was found.
``ClangFormat_VERSION_STRING``
  The version of ClangFormat found.

.. versionadded:: 3.14
  The module defines the following ``IMPORTED`` targets (when
  :prop_gbl:`CMAKE_ROLE` is ``PROJECT``):

``ClangFormat::ClangFormat``
  Executable of the ClangFormat command-line client.

Example usage:

.. code-block:: cmake

   find_package(ClangFormat QUIET)
   if(ClangFormat_FOUND)
     message("ClangFormat found: ${clangFormatExe}")
   endif()
   
#]=======================================================================]

# Look for 'clang-format'
#
set(clangFormatNames clang-format)

# Prefer .cmd variants on Windows unless running in a Makefile
# in the MSYS shell.
#
if(CMAKE_HOST_WIN32)
    if(NOT CMAKE_GENERATOR MATCHES "MSYS")
        set(clangFormatNames clang-format.exe clang-format)

        # VSCode C/C++ extension search path for Windows
        file(GLOB vscodeClangToolsPath
            "$ENV{USERPROFILE}/.vscode/extensions/ms-vscode.cpptools-*/LLVM/bin"
        )
    endif()
endif()

# First search the PATH and specific locations for clang-format.
find_program(clangFormatExe
    NAMES ${clangFormatNames}
    PATHS ${vscodeClangToolsPath}
    DOC "clang-format command line client"
)

if(CMAKE_HOST_WIN32)
    # Now look for installations in Clang/ directories under typical installation
    # prefixes on Windows.  Exclude PATH from this search because VS 2017's
    # command prompt happens to have a PATH entry with a Clang/ subdirectory
    # containing a minimal clang not meant for general use.
    find_program(clangFormatExe
        NAMES ${clangFormatNames}
        PATH_SUFFIXES Clang/bin
        NO_SYSTEM_ENVIRONMENT_PATH
        DOC "clang-format command line client"
    )
endif()

mark_as_advanced(clangFormatExe)

unset(clangFormatNames)

if(clangFormatExe)
    set(doClangFormatVersionCheck ON)
    get_property(clangFormatVersionProp GLOBAL
        PROPERTY _CMAKE_FindClangFormat_clangFormatExe_VERSION
    )

    if(clangFormatVersionProp)
        list(GET clangFormatVersionProp 0 clangFormatExe)
        list(GET clangFormatVersionProp 1 clangFormatVersion)

        if(clangFormatExe STREQUAL clangFormatExe AND NOT clangFormatVersion STREQUAL "")
            set(clangFormatVersionString "${clangFormatVersion}")
            set(doClangFormatVersionCheck FALSE)
        endif()

        unset(clangFormatExe)
        unset(clangFormatVersion)
    endif()

    unset(clangFormatVersionProp)

    if(doClangFormatVersionCheck)
        execute_process(COMMAND ${clangFormatExe} --version
            OUTPUT_VARIABLE clangFormatVersion
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        if(clangFormatVersion MATCHES "^clang-format version [0-9]")
            string(REGEX MATCH "clang-format version [0-9]+\\.[0-9]+\\.[0-9]+" clangFormatVersionString "${clangFormatVersion}")
            string(REPLACE "clang-format version " "" clangFormatVersionString "${clangFormatVersionString}")
            set_property(GLOBAL PROPERTY _CMAKE_FindClangFormat_clangFormatExe_VERSION
                "${clangFormatExe};${clangFormatVersionString}"
            )
        endif()

        unset(clangFormatVersion)
    endif()

    unset(doClangFormatVersionCheck)

    get_property(findClangFormatRole GLOBAL PROPERTY CMAKE_ROLE)

    if(findClangFormatRole STREQUAL "PROJECT")
        if(NOT TARGET ClangFormat::ClangFormat)
            add_executable(ClangFormat::ClangFormat IMPORTED)
            set_property(TARGET ClangFormat::ClangFormat PROPERTY IMPORTED_LOCATION "${clangFormatExe}")
        endif()
    endif()

    unset(findClangFormatRole)
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    ClangFormat
    REQUIRED_VARS clangFormatExe
    VERSION_VAR clangFormatVersionString)
