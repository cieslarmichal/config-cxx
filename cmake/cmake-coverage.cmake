option(
    CODE_COVERAGE
    "Builds targets with code coverage instrumentation. (Requires GCC or Clang)"
    OFF)

find_program(LLVM_COV_PATH llvm-cov-16)
find_program(LLVM_PROFDATA_PATH llvm-profdata-16)
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)
mark_as_advanced(FORCE LLVM_COV_PATH LLVM_PROFDATA_PATH LCOV_PATH GENHTML_PATH)

set(CMAKE_COVERAGE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ccov)
set_property(GLOBAL PROPERTY JOB_POOLS ccov_serial_pool=1)

if (CODE_COVERAGE AND NOT CODE_COVERAGE_ADDED)
    set(CODE_COVERAGE_ADDED ON)

    file(MAKE_DIRECTORY ${CMAKE_COVERAGE_OUTPUT_DIRECTORY})

    if (CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
        OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")

        if (CMAKE_C_COMPILER_ID MATCHES "AppleClang" OR CMAKE_CXX_COMPILER_ID
            MATCHES "AppleClang")
            message(
                STATUS
                "Building with XCode-provided llvm code coverage tools (via `xcrun`)"
            )
            set(LLVM_COV_PATH xcrun llvm-cov)
            set(LLVM_PROFDATA_PATH xcrun llvm-profdata)
        else ()
            message(STATUS "Building with llvm code coverage tools")
        endif ()

        if (NOT LLVM_COV_PATH)
            message(FATAL_ERROR "llvm-cov not found! Aborting.")
        else ()
            execute_process(COMMAND ${LLVM_COV_PATH} --version
                OUTPUT_VARIABLE LLVM_COV_VERSION_CALL_OUTPUT)
            string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" LLVM_COV_VERSION
                ${LLVM_COV_VERSION_CALL_OUTPUT})

            if (LLVM_COV_VERSION VERSION_LESS "7.0.0")
                message(
                    WARNING
                    "target_code_coverage()/add_code_coverage_all_targets() 'EXCLUDE' option only available on llvm-cov >= 7.0.0"
                )
            endif ()
        endif ()

        if (${CMAKE_VERSION} VERSION_LESS "3.17.0")
            add_custom_target(
                config-ccov-clean
                COMMAND ${CMAKE_COMMAND} -E remove -f
                ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list
                COMMAND ${CMAKE_COMMAND} -E remove -f
                ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/profraw.list)
        else ()
            add_custom_target(
                config-ccov-clean
                COMMAND ${CMAKE_COMMAND} -E rm -f
                ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list
                COMMAND ${CMAKE_COMMAND} -E rm -f
                ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/profraw.list)
        endif ()

        add_custom_target(
            config-ccov-libs
            COMMAND ;
            COMMENT "libs ready for coverage report.")

    elseif (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES
        "GNU")
        message(STATUS "Building with lcov Code Coverage Tools")

        if (CMAKE_BUILD_TYPE)
            string(TOUPPER ${CMAKE_BUILD_TYPE} upper_build_type)
            if (NOT ${upper_build_type} STREQUAL "DEBUG")
                message(
                    WARNING
                    "Code coverage results with an optimized (non-Debug) build may be misleading"
                )
            endif ()
        else ()
            message(
                WARNING
                "Code coverage results with an optimized (non-Debug) build may be misleading"
            )
        endif ()
        if (NOT LCOV_PATH)
            message(FATAL_ERROR "lcov not found! Aborting...")
        endif ()
        if (NOT GENHTML_PATH)
            message(FATAL_ERROR "genhtml not found! Aborting...")
        endif ()

        # Targets
        add_custom_target(
            config-ccov-clean COMMAND ${LCOV_PATH} --directory
            ${CMAKE_BINARY_DIR} --zerocounters)

    else ()
        message(FATAL_ERROR "Code coverage requires Clang or GCC. Aborting.")
    endif ()
endif ()

function(target_code_coverage TARGET_NAME)
    # Argument parsing
    set(options AUTO ALL EXTERNAL PUBLIC INTERFACE PLAIN)
    set(single_value_keywords COVERAGE_TARGET_NAME)
    set(multi_value_keywords EXCLUDE OBJECTS PRE_ARGS ARGS)
    cmake_parse_arguments(
        target_code_coverage "${options}" "${single_value_keywords}"
        "${multi_value_keywords}" ${ARGN})

    if (target_code_coverage_PUBLIC)
        set(TARGET_VISIBILITY PUBLIC)
        set(TARGET_LINK_VISIBILITY PUBLIC)
    elseif (target_code_coverage_INTERFACE)
        set(TARGET_VISIBILITY INTERFACE)
        set(TARGET_LINK_VISIBILITY INTERFACE)
    elseif (target_code_coverage_PLAIN)
        set(TARGET_VISIBILITY PUBLIC)
        set(TARGET_LINK_VISIBILITY)
    else ()
        set(TARGET_VISIBILITY PRIVATE)
        set(TARGET_LINK_VISIBILITY PRIVATE)
    endif ()

    if (NOT target_code_coverage_COVERAGE_TARGET_NAME)
        set(target_code_coverage_COVERAGE_TARGET_NAME ${TARGET_NAME})
    endif ()

    if (CODE_COVERAGE)

        if (CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
            OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
            target_compile_options(${TARGET_NAME} ${TARGET_VISIBILITY}
                -fprofile-instr-generate -fcoverage-mapping)
            target_link_options(${TARGET_NAME} ${TARGET_VISIBILITY}
                -fprofile-instr-generate -fcoverage-mapping)
        elseif (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID
            MATCHES "GNU")
            target_compile_options(
                ${TARGET_NAME}
                ${TARGET_VISIBILITY}
                -fprofile-arcs
                -ftest-coverage
                $<$<COMPILE_LANGUAGE:CXX>:-fno-elide-constructors>
                -fno-default-inline)
            target_link_libraries(${TARGET_NAME} ${TARGET_LINK_VISIBILITY} gcov)
        endif ()

        get_target_property(target_type ${TARGET_NAME} TYPE)

        if (target_type STREQUAL "SHARED_LIBRARY" AND target_code_coverage_ALL)
            if (CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
                OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
                add_custom_target(
                    ccov-run-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${CMAKE_COMMAND} -E echo
                    "-object=$<TARGET_FILE:${TARGET_NAME}>" >>
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list
                    DEPENDS ${TARGET_NAME})

                if (NOT TARGET config-ccov-libs)
                    message(
                        FATAL_ERROR
                        "Calling target_code_coverage with 'ALL' must be after a call to 'add_code_coverage_all_targets'."
                    )
                endif ()

                add_dependencies(
                    config-ccov-libs
                    ccov-run-${target_code_coverage_COVERAGE_TARGET_NAME})
            endif ()
        endif ()

        if (target_type STREQUAL "EXECUTABLE")
            if (CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
                OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")

                foreach (SO_TARGET ${target_code_coverage_OBJECTS})
                    if (TARGET ${SO_TARGET})
                        get_target_property(SO_TARGET_TYPE ${SO_TARGET} TYPE)
                        if (${SO_TARGET_TYPE} STREQUAL "SHARED_LIBRARY")
                            set(SO_OBJECTS ${SO_OBJECTS}
                                -object=$<TARGET_FILE:${SO_TARGET}>)
                        endif ()
                    endif ()
                endforeach ()

                add_custom_target(
                    ccov-run-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${CMAKE_COMMAND} -E env ${CMAKE_CROSSCOMPILING_EMULATOR}
                    ${target_code_coverage_PRE_ARGS}
                    LLVM_PROFILE_FILE=${target_code_coverage_COVERAGE_TARGET_NAME}.profraw
                    $<TARGET_FILE:${TARGET_NAME}>
                    ${target_code_coverage_ARGS}
                    COMMAND
                    ${CMAKE_COMMAND} -E echo
                    "-object=$<TARGET_FILE:${TARGET_NAME}>" ${SO_OBJECTS} >>
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list
                    COMMAND
                    ${CMAKE_COMMAND} -E echo
                    "${CMAKE_CURRENT_BINARY_DIR}/${target_code_coverage_COVERAGE_TARGET_NAME}.profraw"
                    >> ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/profraw.list
                    JOB_POOL ccov_serial_pool
                    DEPENDS config-ccov-libs ${TARGET_NAME})

                add_custom_target(
                    ccov-processing-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${LLVM_PROFDATA_PATH} merge -sparse
                    ${target_code_coverage_COVERAGE_TARGET_NAME}.profraw -o
                    ${target_code_coverage_COVERAGE_TARGET_NAME}.profdata
                    DEPENDS
                    ccov-run-${target_code_coverage_COVERAGE_TARGET_NAME})

                if (LLVM_COV_VERSION VERSION_GREATER_EQUAL "7.0.0")
                    foreach (EXCLUDE_ITEM ${target_code_coverage_EXCLUDE})
                        set(EXCLUDE_REGEX
                            ${EXCLUDE_REGEX}
                            -ignore-filename-regex='${EXCLUDE_ITEM}')
                    endforeach ()
                endif ()

                add_custom_target(
                    ccov-show-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${LLVM_COV_PATH} show $<TARGET_FILE:${TARGET_NAME}>
                    ${SO_OBJECTS}
                    -instr-profile=${target_code_coverage_COVERAGE_TARGET_NAME}.profdata
                    -show-line-counts-or-regions ${EXCLUDE_REGEX}
                    DEPENDS
                    ccov-processing-${target_code_coverage_COVERAGE_TARGET_NAME}
                )

                add_custom_target(
                    ccov-report-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${LLVM_COV_PATH} report $<TARGET_FILE:${TARGET_NAME}>
                    ${SO_OBJECTS}
                    -instr-profile=${target_code_coverage_COVERAGE_TARGET_NAME}.profdata
                    ${EXCLUDE_REGEX}
                    DEPENDS
                    ccov-processing-${target_code_coverage_COVERAGE_TARGET_NAME}
                )

                add_custom_target(
                    ccov-export-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${LLVM_COV_PATH} export $<TARGET_FILE:${TARGET_NAME}>
                    ${SO_OBJECTS}
                    -instr-profile=${target_code_coverage_COVERAGE_TARGET_NAME}.profdata
                    -format="text" ${EXCLUDE_REGEX} >
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/${target_code_coverage_COVERAGE_TARGET_NAME}.json
                    DEPENDS
                    ccov-processing-${target_code_coverage_COVERAGE_TARGET_NAME}
                )

                add_custom_target(
                    ccov-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${LLVM_COV_PATH} show $<TARGET_FILE:${TARGET_NAME}>
                    ${SO_OBJECTS}
                    -instr-profile=${target_code_coverage_COVERAGE_TARGET_NAME}.profdata
                    -show-line-counts-or-regions
                    -output-dir=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/${target_code_coverage_COVERAGE_TARGET_NAME}
                    -format="html" ${EXCLUDE_REGEX}
                    DEPENDS
                    ccov-processing-${target_code_coverage_COVERAGE_TARGET_NAME}
                )

            elseif (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID
                MATCHES "GNU")
                set(COVERAGE_INFO
                    "${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/${target_code_coverage_COVERAGE_TARGET_NAME}.info"
                )

                add_custom_target(
                    ccov-run-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${CMAKE_CROSSCOMPILING_EMULATOR}
                    ${target_code_coverage_PRE_ARGS}
                    $<TARGET_FILE:${TARGET_NAME}>
                    ${target_code_coverage_ARGS}
                    DEPENDS ${TARGET_NAME})

                foreach (EXCLUDE_ITEM ${target_code_coverage_EXCLUDE})
                    set(EXCLUDE_REGEX ${EXCLUDE_REGEX} --remove
                        ${COVERAGE_INFO} '${EXCLUDE_ITEM}')
                endforeach ()

                if (EXCLUDE_REGEX)
                    set(EXCLUDE_COMMAND ${LCOV_PATH} ${EXCLUDE_REGEX}
                        --output-file ${COVERAGE_INFO})
                else ()
                    set(EXCLUDE_COMMAND ;)
                endif ()

                if (NOT ${target_code_coverage_EXTERNAL})
                    set(EXTERNAL_OPTION --no-external)
                endif ()

                if (${CMAKE_VERSION} VERSION_LESS "3.17.0")
                    add_custom_target(
                        ccov-capture-${target_code_coverage_COVERAGE_TARGET_NAME}
                        COMMAND ${CMAKE_COMMAND} -E remove -f ${COVERAGE_INFO}
                        COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR}
                        --zerocounters
                        COMMAND
                        ${CMAKE_CROSSCOMPILING_EMULATOR}
                        ${target_code_coverage_PRE_ARGS}
                        $<TARGET_FILE:${TARGET_NAME}>
                        ${target_code_coverage_ARGS}
                        COMMAND
                        ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR}
                        --base-directory ${CMAKE_SOURCE_DIR} --capture
                        ${EXTERNAL_OPTION} --output-file ${COVERAGE_INFO}
                        COMMAND ${EXCLUDE_COMMAND}
                        DEPENDS ${TARGET_NAME})
                else ()
                    add_custom_target(
                        ccov-capture-${target_code_coverage_COVERAGE_TARGET_NAME}
                        COMMAND ${CMAKE_COMMAND} -E rm -f ${COVERAGE_INFO}
                        COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR}
                        --zerocounters
                        COMMAND
                        ${CMAKE_CROSSCOMPILING_EMULATOR}
                        ${target_code_coverage_PRE_ARGS}
                        $<TARGET_FILE:${TARGET_NAME}>
                        ${target_code_coverage_ARGS}
                        COMMAND
                        ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR}
                        --base-directory ${CMAKE_SOURCE_DIR} --capture
                        ${EXTERNAL_OPTION} --output-file ${COVERAGE_INFO}
                        COMMAND ${EXCLUDE_COMMAND}
                        DEPENDS ${TARGET_NAME})
                endif ()

                add_custom_target(
                    ccov-${target_code_coverage_COVERAGE_TARGET_NAME}
                    COMMAND
                    ${GENHTML_PATH} -o
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/${target_code_coverage_COVERAGE_TARGET_NAME}
                    ${COVERAGE_INFO}
                    DEPENDS
                    ccov-capture-${target_code_coverage_COVERAGE_TARGET_NAME}
                )
            endif ()

            add_custom_command(
                TARGET ccov-${target_code_coverage_COVERAGE_TARGET_NAME}
                POST_BUILD
                COMMAND ;
                COMMENT
                "Open ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/${target_code_coverage_COVERAGE_TARGET_NAME}/index.html in your browser to view the coverage report."
            )

            if (target_code_coverage_AUTO)
                if (NOT TARGET ccov)
                    add_custom_target(ccov)
                endif ()
                add_dependencies(
                    ccov ccov-${target_code_coverage_COVERAGE_TARGET_NAME})

                if (NOT CMAKE_C_COMPILER_ID MATCHES "GNU"
                    AND NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                    if (NOT TARGET ccov-report)
                        add_custom_target(ccov-report)
                    endif ()
                    add_dependencies(
                        ccov-report
                        ccov-report-${target_code_coverage_COVERAGE_TARGET_NAME}
                    )
                endif ()
            endif ()

            if (target_code_coverage_ALL)
                if (NOT TARGET config-ccov-all-processing)
                    message(
                        FATAL_ERROR
                        "Calling target_code_coverage with 'ALL' must be after a call to 'add_code_coverage_all_targets'."
                    )
                endif ()

                add_dependencies(
                    config-ccov-all-processing
                    ccov-run-${target_code_coverage_COVERAGE_TARGET_NAME})
            endif ()
        endif ()
    endif ()
endfunction()

function(add_code_coverage)
    if (CODE_COVERAGE)
        if (CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
            OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
            add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
            add_link_options(-fprofile-instr-generate -fcoverage-mapping)
        elseif (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID
            MATCHES "GNU")
            add_compile_options(
                -fprofile-arcs -ftest-coverage
                $<$<COMPILE_LANGUAGE:CXX>:-fno-elide-constructors>
                -fno-default-inline)
            link_libraries(gcov)
        endif ()
    endif ()
endfunction()

function(add_code_coverage_all_targets)
    set(multi_value_keywords EXCLUDE)
    cmake_parse_arguments(add_code_coverage_all_targets "" ""
        "${multi_value_keywords}" ${ARGN})

    if (CODE_COVERAGE)
        if (CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang"
            OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?[Cc]lang")

            if (WIN32)
                add_custom_target(
                    config-ccov-all-processing
                    COMMAND
                    powershell -Command $$FILELIST = Get-Content
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/profraw.list\;
                    llvm-profdata.exe merge -o
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    -sparse $$FILELIST)
            else ()
                add_custom_target(
                    config-ccov-all-processing
                    COMMAND
                    ${LLVM_PROFDATA_PATH} merge -o
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    -sparse `cat
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/profraw.list`)
            endif ()

            if (LLVM_COV_VERSION VERSION_GREATER_EQUAL "7.0.0")
                foreach (EXCLUDE_ITEM ${add_code_coverage_all_targets_EXCLUDE})
                    set(EXCLUDE_REGEX ${EXCLUDE_REGEX}
                        -ignore-filename-regex='${EXCLUDE_ITEM}')
                endforeach ()
            endif ()

            if (WIN32)
                add_custom_target(
                    config-ccov-all-report
                    COMMAND
                    powershell -Command $$FILELIST = Get-Content
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list\;
                    llvm-cov.exe report $$FILELIST
                    -instr-profile=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    ${EXCLUDE_REGEX}
                    DEPENDS config-ccov-all-processing)
            else ()
                add_custom_target(
                    config-ccov-all-report
                    COMMAND
                    ${LLVM_COV_PATH} report `cat
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list`
                    -instr-profile=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    ${EXCLUDE_REGEX}
                    DEPENDS config-ccov-all-processing)
            endif ()

            if (WIN32)
                add_custom_target(
                    config-ccov-all-export
                    COMMAND
                    powershell -Command $$FILELIST = Get-Content
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list\;
                    llvm-cov.exe export $$FILELIST
                    -instr-profile=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    -format="text" ${EXCLUDE_REGEX} >
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/coverage.json
                    DEPENDS config-ccov-all-processing)
            else ()
                add_custom_target(
                    config-ccov-all-export
                    COMMAND
                    ${LLVM_COV_PATH} export `cat
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list`
                    -instr-profile=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    -format="text" ${EXCLUDE_REGEX} >
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/coverage.json
                    DEPENDS config-ccov-all-processing)
            endif ()

            if (WIN32)
                add_custom_target(
                    config-ccov-all
                    COMMAND
                    powershell -Command $$FILELIST = Get-Content
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list\;
                    llvm-cov.exe show $$FILELIST
                    -instr-profile=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    -show-line-counts-or-regions
                    -output-dir=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged
                    -format="html" ${EXCLUDE_REGEX}
                    DEPENDS config-ccov-all-processing)
            else ()
                add_custom_target(
                    config-ccov-all
                    COMMAND
                    ${LLVM_COV_PATH} show `cat
                    ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/binaries.list`
                    -instr-profile=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.profdata
                    -show-line-counts-or-regions
                    -output-dir=${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged
                    -format="html" ${EXCLUDE_REGEX}
                    DEPENDS config-ccov-all-processing)
            endif ()

        elseif (CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID
            MATCHES "GNU")
            set(COVERAGE_INFO
                "${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged.info")

            add_custom_target(config-ccov-all-processing COMMAND ;)

            set(EXCLUDE_REGEX)
            foreach (EXCLUDE_ITEM ${add_code_coverage_all_targets_EXCLUDE})
                set(EXCLUDE_REGEX ${EXCLUDE_REGEX} --remove ${COVERAGE_INFO}
                    '${EXCLUDE_ITEM}')
            endforeach ()

            if (EXCLUDE_REGEX)
                set(EXCLUDE_COMMAND ${LCOV_PATH} ${EXCLUDE_REGEX} --output-file
                    ${COVERAGE_INFO})
            else ()
                set(EXCLUDE_COMMAND ;)
            endif ()

            if (${CMAKE_VERSION} VERSION_LESS "3.17.0")
                add_custom_target(
                    config-ccov-all-capture
                    COMMAND ${CMAKE_COMMAND} -E remove -f ${COVERAGE_INFO}
                    COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR}
                    --capture --output-file ${COVERAGE_INFO}
                    COMMAND ${EXCLUDE_COMMAND}
                    DEPENDS config-ccov-all-processing)
            else ()
                add_custom_target(
                    config-ccov-all-capture
                    COMMAND ${CMAKE_COMMAND} -E rm -f ${COVERAGE_INFO}
                    COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR}
                    --capture --output-file ${COVERAGE_INFO}
                    COMMAND ${EXCLUDE_COMMAND}
                    DEPENDS config-ccov-all-processing)
            endif ()

            add_custom_target(
                config-ccov-all
                COMMAND
                ${GENHTML_PATH} -o
                ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged
                ${COVERAGE_INFO} -p ${CMAKE_SOURCE_DIR}
                DEPENDS config-ccov-all-capture)

        endif ()

        add_custom_command(
            TARGET config-ccov-all
            POST_BUILD
            COMMAND ;
            COMMENT
            "Open ${CMAKE_COVERAGE_OUTPUT_DIRECTORY}/all-merged/index.html in your browser to view the coverage report."
        )
    endif ()
endfunction()
