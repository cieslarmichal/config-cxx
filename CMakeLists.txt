cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME config-cxx)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_CONFIG_CXX_TESTS "Build tests" ON)
option(CODE_COVERAGE "Build config-cxx with coverage support" OFF)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /permissive- /bigobj")
else ()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wformat"
    )
endif ()

set(LIBRARY_NAME config-cxx)

set(SOURCES
    src/config.cpp
    src/config_directory_path_resolver.cpp
    src/config_provider.cpp
    src/file_system_service.cpp
    src/json_config_loader.cpp
    src/yaml_config_loader.cpp
    src/xml_config_loader.cpp
)

add_library(${LIBRARY_NAME} ${SOURCES})

add_subdirectory(externals/json)

target_link_libraries(${LIBRARY_NAME} PRIVATE nlohmann_json)

add_subdirectory(externals/yaml-cpp)
target_link_libraries(${LIBRARY_NAME} PUBLIC  yaml-cpp)

add_subdirectory(externals/pugixml)
target_link_libraries(${LIBRARY_NAME} PUBLIC  pugixml)

target_include_directories(
    ${LIBRARY_NAME}
    INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")

if (CODE_COVERAGE)
    set(target_code_coverage_ALL 1)
    include("cmake/cmake-coverage.cmake")
    add_code_coverage_all_targets(EXCLUDE tests/*)
    target_code_coverage(${LIBRARY_NAME} ALL)
endif ()

if (BUILD_CONFIG_CXX_TESTS)
    add_subdirectory(externals/googletest)

    enable_testing()

    add_subdirectory(tests)
endif ()
