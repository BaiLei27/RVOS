#[=======================================================================[.rst:
FindClangTidy
-------------

The module defines the following variables:

``clangTidyExe``
  Path to clangTidy command-line client.
``clangTidy_FOUND``, ``clangTidy_FOUND``
  True if the clangTidy command-line client was found.
``clangTidy_VERSION_STRING``
  The version of clangTidy found.

.. versionadded:: 3.14
  The module defines the following ``IMPORTED`` targets (when
  :prop_gbl:`CMAKE_ROLE` is ``PROJECT``):

``clangTidy::clangTidy``
  Executable of the clangTidy command-line client.

Example usage:

.. code-block:: cmake

   find_package(clangTidy QUIET)
   if(clangTidy_FOUND)
     message("clangTidy found: ${clangTidyExe}")
   endif()

#]=======================================================================]

# Look for 'clang-tidy'
#
set(clangTidyNames clang-tidy)

# Prefer .cmd variants on Windows unless running in a Makefile
# in the MSYS shell.
#
set(extensionPathSuffix "extensions/ms-vscode.cpptools-*/LLVM/bin")

if(CMAKE_HOST_WIN32)
    set(clangTidyNames clang-tidy.exe clang-tidy)
    set(vscodeFolder "$ENV{USERPROFILE}/.vscode")
    set(vscodeServerFolder "$ENV{USERPROFILE}/.vscode-server")
else()
    set(vscodeFolder "$ENV{HOME}/.vscode")
    set(vscodeServerFolder "$ENV{HOME}/.vscode-server")
endif()

if(EXISTS "${vscodeFolder}" AND IS_DIRECTORY "${vscodeFolder}")
    file(GLOB matchedPaths "${vscodeFolder}/${extensionPathSuffix}")
elseif(EXISTS "${vscodeServerFolder}" AND IS_DIRECTORY "${vscodeServerFolder}")
    file(GLOB matchedPaths "${vscodeServerFolder}/${extensionPathSuffix}")
endif()

message(VERBOSE "matchedPaths: '${matchedPaths}'")

if(matchedPaths)
    list(LENGTH matchedPaths pathCount)
    math(EXPR lastIdx "${pathCount} - 1")
    list(GET matchedPaths ${lastIdx} targetSearchPath)
endif()

message(VERBOSE "targetSearchPath: '${targetSearchPath}'")

# First search the PATH and specific locations for clang-tidy.
find_program(clangTidyExe
    NAMES ${clangTidyNames}
    PATHS ${targetSearchPath}
    DOC "clang-tidy command line client"
)

if(CMAKE_HOST_WIN32)
    # Now look for installations in Clang/ directories under typical installation
    # prefixes on Windows.  Exclude PATH from this search because VS 2017's
    # command prompt happens to have a PATH entry with a Clang/ subdirectory
    # containing a minimal clang not meant for general use.
    find_program(clangTidyExe
        NAMES ${clangTidyNames}
        PATH_SUFFIXES Clang/bin
        NO_SYSTEM_ENVIRONMENT_PATH
        DOC "clang-tidy command line client"
    )
endif()

mark_as_advanced(clangTidyExe)

unset(clangTidyNames)

if(clangTidyExe)
    set(doClangTidyVersionCheck ON)
    get_property(clangTidyVersionProp GLOBAL
        PROPERTY _CMAKE_FindClangTidy_clangTidyExe_VERSION
    )

    if(clangTidyVersionProp)
        list(GET clangTidyVersionProp 0 clangTidyExe)
        list(GET clangTidyVersionProp 1 clangTidyVersion)

        if(clangTidyExe STREQUAL clangTidyExe AND NOT clangTidyVersion STREQUAL "")
            set(clangTidyVersionString "${clangTidyVersion}")
            set(doClangTidyVersionCheck FALSE)
        endif()

        unset(clangTidyExe)
        unset(clangTidyVersion)
    endif()

    unset(clangTidyVersionProp)

    if(doClangTidyVersionCheck)
        execute_process(COMMAND ${clangTidyExe} --version
            OUTPUT_VARIABLE clangTidyVersion
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        if(clangTidyVersion MATCHES "LLVM version [0-9]+\\.[0-9]+\\.[0-9]+")
            string(REGEX MATCH "LLVM version [0-9]+\\.[0-9]+\\.[0-9]+" clangTidyVersionString "${clangTidyVersion}")
            string(REPLACE "LLVM version " "" clangTidyVersionString "${clangTidyVersionString}")
            set_property(GLOBAL PROPERTY _CMAKE_FindClangTidy_clangTidyExe_VERSION
                "${clangTidyExe};${clangTidyVersionString}"
            )
        endif()

        unset(clangTidyVersion)
    endif()

    unset(doClangTidyVersionCheck)

    get_property(findClangTidyRole GLOBAL PROPERTY CMAKE_ROLE)

    if(findClangTidyRole STREQUAL "PROJECT")
        if(NOT TARGET ClangTidy::ClangTidy)
            add_executable(ClangTidy::ClangTidy IMPORTED)
            set_property(TARGET ClangTidy::ClangTidy PROPERTY IMPORTED_LOCATION "${clangTidyExe}")
        endif()
    endif()

    unset(findClangTidyRole)

    # set(CMAKE_CXX_CLANG_TIDY ${clangTidyExe})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(
    ClangTidy
    REQUIRED_VARS clangTidyExe
    VERSION_VAR clangTidyVersionString)
