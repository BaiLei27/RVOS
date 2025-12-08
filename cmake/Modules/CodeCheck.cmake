# CodeCheck.cmake — Simultaneously manage clang-format 和 clang-tidy

find_package(ClangFormat)
find_package(ClangTidy)

set_property(GLOBAL PROPERTY checkFmtFiles)
set_property(GLOBAL PROPERTY checkTidyFiles)
set(excludeExtsBase .ld .s)

function(_srcFilesParse target propertyName exclude)
    if(TARGET ${target})
        get_target_property(fmtFiles ${target} SOURCES)
        get_property(allParseFiles GLOBAL PROPERTY ${propertyName})

        foreach(f ${fmtFiles})
            get_filename_component(fileExt ${f} EXT)
            get_filename_component(abs_f ${f} ABSOLUTE)
            file(RELATIVE_PATH rel_f ${CMAKE_SOURCE_DIR} ${abs_f})
            string(TOLOWER ${fileExt} fExtLow) # convert to lowercase

            if(fExtLow IN_LIST excludeExtsBase)
                message(VERBOSE "CodeCheck '${target}': Excluding '${f}' (ext:${fileExt})")
                continue()
            endif()

            set(isExcl FALSE)

            foreach(p ${exclude})
                if(${rel_f} MATCHES ${p})
                    set(isExcl TRUE)
                    message(VERBOSE "CodeCheck '${target}': Excluding '${f}' (pattern: ${p})")
                    break()
                endif()
            endforeach()

            if(isExcl)
                continue()
            endif()

            list(APPEND allParseFiles ${rel_f})
        endforeach()

        list(REMOVE_DUPLICATES allParseFiles)
        message(VERBOSE "CodeCheck: final ${propertyName} colletc: ${allParseFiles}")

        set_property(GLOBAL PROPERTY ${propertyName} ${allParseFiles})

    else()
        message(WARNING "'${CMAKE_CURRENT_FUNCTION}' : target '${target}' does not exist")
    endif()
endfunction()

# === Format ===
if(ClangFormat_FOUND)
    function(FormatCode target)
        set(options)
        set(oneValueArgs)
        set(multiValueArgs EXCLUDES)
        cmake_parse_arguments(fmt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        _srcFilesParse(${target} checkFmtFiles "${fmt_EXCLUDES}")
    endfunction()

    function(DefFormatTarget name)
        get_property(allFmtFiles GLOBAL PROPERTY checkFmtFiles)

        if(allFmtFiles)
            set(cmds ${clangFormatExe} -i -style=file -fallback-style=webkit -verbose ${allFmtFiles})
            message(VERBOSE ++> ${cmds})
            list(LENGTH allFmtFiles cnt)
            add_custom_target(
                ${name}
                ${cmds}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running clang-format on ${cnt} files"
            )
        else()
            message(STATUS "'${CMAKE_CURRENT_FUNCTION}' : no files to ${name}")
        endif()
    endfunction()
else()
    message(WARNING "clang-format not found; format target disabled")
endif()

# === Tidy ===
if(ClangTidy_FOUND) # don't use ${ClangTidy_FOUND}
    function(TidyCode target)
        option(enTidyAuto "Enable clang-tidy on compile" OFF)
        set(doClangTidy "${clangTidyExe}" CACHE STRING "clang-tidy command")
        _srcFilesParse(${target} checkTidyFiles "")

        if(enTidyAuto)
            set_property(TARGET ${target} PROPERTY
                C_CLANG_TIDY "${doClangTidy}"
                CXX_CLANG_TIDY "${doClangTidy}")
        endif()
    endfunction()

    function(DefTidyTarget name)
        get_property(allTidyFiles GLOBAL PROPERTY checkTidyFiles)

        if(allTidyFiles)
            set(cmds ${doClangTidy} --system-headers=false -p ${CMAKE_BINARY_DIR} ${allTidyFiles})

            # set(cmds ${doClangTidy} ${allTidyFiles})
            message(VERBOSE ++> ${cmds})

            list(LENGTH allTidyFiles cnt)
            add_custom_target(
                ${name}
                ${cmds}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running clang-tidy on ${cnt} files"
            )
        else()
            message(STATUS "'${CMAKE_CURRENT_FUNCTION}' : no files to ${name}")
        endif()
    endfunction()

else()
    message(WARNING "clang-tidy not found; tidy target disabled")
endif()

function(SetupCodeChecks)
    foreach(target IN LISTS ARGV)
        if(ClangFormat_FOUND AND TARGET ${target})
            FormatCode(${target})
        endif()

        if(ClangTidy_FOUND AND TARGET ${target})
            TidyCode(${target})
        endif()
    endforeach()
endfunction()

function(DefineCodeChecksTargets fmtName tidyName)
    if(ClangFormat_FOUND)
        DefFormatTarget(${fmtName})
    endif()

    if(ClangTidy_FOUND)
        DefTidyTarget(${tidyName})
    endif()
endfunction()
