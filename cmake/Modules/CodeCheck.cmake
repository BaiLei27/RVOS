# CodeCheck.cmake — Simultaneously manage clang-format 和 clang-tidy

find_package(ClangFormat)
find_package(ClangTidy)

set_property(GLOBAL PROPERTY codeCheckFormatFiles)
set_property(GLOBAL PROPERTY codeCheckTidyFiles)

# === Format ===
if(ClangFormat_FOUND)
    function(FormatCode target)
        set(options)
        set(oneValueArgs)
        set(multiValueArgs EXCLUDES)
        cmake_parse_arguments(fmt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        set(BASE_DIR_FOR_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

        if(TARGET ${target})
            get_target_property(srcFiles ${target} SOURCES)
            get_property(allFmtFiles GLOBAL PROPERTY codeCheckFormatFiles)

            foreach(f ${srcFiles})
                set(excl FALSE)
                get_filename_component(absf ${f} ABSOLUTE)
                file(RELATIVE_PATH relf ${CMAKE_SOURCE_DIR} ${absf})

                foreach(pattern ${fmt_EXCLUDES})
                    if(f MATCHES pattern)
                        set(excl TRUE)
                        message(VERBOSE "FormatCode: excluding ${f} matches ${pattern}")
                    endif()
                endforeach()

                if(NOT excl)
                    list(APPEND allFmtFiles ${relf})
                endif()

            endforeach()

            list(REMOVE_DUPLICATES allFmtFiles)
            set_property(GLOBAL PROPERTY codeCheckFormatFiles ${allFmtFiles})
        else()
            message(WARNING "FormatCode: target '${target}' does not exist")
        endif()
    endfunction()

    function(DefFormatTarget name)
        get_property(allFmtFiles GLOBAL PROPERTY codeCheckFormatFiles)

        if(allFmtFiles)
            set(cmds ${clangFormatExe} -i -style=file -fallback-style=webkit -verbose ${allFmtFiles})
            # message(++> ${cmds})
            list(LENGTH allFmtFiles cnt)
            add_custom_target(
                ${name}
                ${cmds}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running clang-format on ${cnt} files"
            )
        else()
            message(STATUS "DefFormatTarget ${name}: no files to format")
        endif()

    endfunction()
else()
    message(WARNING "clang-format not found; format target disabled")
endif()

# === Tidy ===
if(ClangTidy_FOUND) #don't use ${ClangTidy_FOUND}
    option(enTidyAuto "Enable clang-tidy on compile" OFF)
    set(doClangTidy "${clangTidyExe}" CACHE STRING "clang-tidy command")

    function(TidyCode target)
        if(TARGET ${target})
            if(enTidyAuto)
                set_property(TARGET ${target} PROPERTY
                    C_CLANG_TIDY "${doClangTidy}"
                    CXX_CLANG_TIDY "${doClangTidy}")
            endif()

            get_target_property(srcFiles ${target} SOURCES)
            get_property(allTidyFiles GLOBAL PROPERTY codeCheckTidyFiles)
            set(excludeExts ".h" ".hpp" ".hh" ".s")

            foreach(f ${srcFiles})
                get_filename_component(fileExt ${f} EXT)
                string(TOLOWER ${fileExt} fileext) # convert to lowercase

                if(NOT fileext IN_LIST excludeExts)
                    get_filename_component(absf ${f} ABSOLUTE)
                    # message(++> ${absf})
                    list(APPEND allTidyFiles ${absf})
                else()
                    message(VERBOSE "TidyCode: Excluding file ${f} (ext: ${fileext})")
                endif()

            endforeach()

            unset(excludeExts)
            list(REMOVE_DUPLICATES allTidyFiles)
            set_property(GLOBAL PROPERTY codeCheckTidyFiles ${allTidyFiles})
        else()
            message(WARNING "TidyCode: target '${target}' does not exist")
        endif()
    endfunction()

    function(DefTidyTarget name)
        get_property(allTidyFiles GLOBAL PROPERTY codeCheckTidyFiles)

        if(allTidyFiles)
            set(cmds ${doClangTidy} -p ${CMAKE_BINARY_DIR} ${allTidyFiles})
            # set(cmds ${doClangTidy} ${allTidyFiles})
            # message(++> ${cmds})
            list(LENGTH allTidyFiles cnt)
            add_custom_target(
                ${name}
                ${cmds}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running clang-tidy on ${cnt} files"
            )
        else()
            message(STATUS "DefTidyTarget ${name}: no files to tidy")
        endif()

    endfunction()
else()
    message(WARNING "clang-tidy not found; tidy target disabled")
endif()

function(SetupCodeChecks target)
    if(ClangFormat_FOUND)
        FormatCode(${target})
    endif()

    if(ClangTidy_FOUND)
        TidyCode(${target})
    endif()
endfunction()

function(DefineCodeChecksTargets fmtName tidyName)
    if(ClangFormat_FOUND)
        DefFormatTarget(${fmtName})
    endif()

    if(ClangTidy_FOUND)
        DefTidyTarget(${tidyName})
    endif()
endfunction()
