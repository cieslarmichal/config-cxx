cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME config-cxx)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_CONFIG_CXX_TESTS DEFAULT ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /permissive- /bigobj")
else ()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wformat -Werror"
    )
endif ()

set(LIBRARY_NAME config-cxx)

set(SOURCES
    src/Config.cpp
    src/ConfigDirectoryPathResolver.cpp
    src/EnvironmentParser.cpp
)

set(UT_SOURCES
    src/ConfigTest.cpp
    src/EnvironmentParserTest.cpp
)

add_library(${LIBRARY_NAME} ${SOURCES})

target_include_directories(
    ${LIBRARY_NAME}
    INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include" ${FMT_INCLUDE_DIR})

add_subdirectory(externals/fmt)

set(FMT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/externals/fmt/include")

target_link_libraries(${LIBRARY_NAME} PRIVATE fmt)

if (BUILD_CONFIG_CXX_TESTS)
    add_subdirectory(externals/googletest)

    set(GTEST_INCLUDE_DIR
        "${CMAKE_SOURCE_DIR}/externals/googletest/googletest/include")
    set(GMOCK_INCLUDE_DIR
        "${CMAKE_SOURCE_DIR}/externals/googletest/googlemock/include")

    enable_testing()

    set(target_code_coverage_ALL 1)

    include("cmake/cmake-coverage.cmake")

    add_code_coverage_all_targets()

    add_executable(${LIBRARY_NAME}-UT ${UT_SOURCES})

    target_link_libraries(${LIBRARY_NAME}-UT PRIVATE gmock_main
        config-cxx)

    target_include_directories(
        ${LIBRARY_NAME}-UT
        PRIVATE ${FMT_INCLUDE_DIR} ${GTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR}
        ${CMAKE_CURRENT_LIST_DIR})

    add_test(
        NAME ${LIBRARY_NAME}-UT
        COMMAND ${LIBRARY_NAME}-UT
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    target_code_coverage(${LIBRARY_NAME}-UT ALL)
endif ()
