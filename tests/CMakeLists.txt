cmake_minimum_required(VERSION 3.22)
project(${CMAKE_PROJECT_NAME}-UT CXX)

include(CTest)

set(CONFIG_CXX_UT_SOURCES
    config_test.cpp
    config_directory_path_resolver_test.cpp
    json_config_loader_test.cpp
    yaml_config_loader_test.cpp
    xml_config_loader_test.cpp
    config_provider_test.cpp
    file_system_service_test.cpp
    file_system_service_executable_test.cpp
    environment_setter.cpp
    environment_setter_test.cpp
)

set(GTEST_INCLUDE_DIR
    "${CMAKE_SOURCE_DIR}/externals/googletest/googletest/include")
set(GMOCK_INCLUDE_DIR
    "${CMAKE_SOURCE_DIR}/externals/googletest/googlemock/include")

add_executable(${CMAKE_PROJECT_NAME}-UT ${CONFIG_CXX_UT_SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME}-UT PRIVATE ${CMAKE_PROJECT_NAME} gmock_main nlohmann_json yaml-cpp pugixml)

target_include_directories(
    ${CMAKE_PROJECT_NAME}-UT
    PRIVATE ${GTEST_INCLUDE_DIR}
    ${GMOCK_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME ${CMAKE_PROJECT_NAME}-UT
    COMMAND ${CMAKE_PROJECT_NAME}-UT
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

if (CONFIG_CODE_COVERAGE)
    target_code_coverage(${CMAKE_PROJECT_NAME}-UT ALL)
endif ()
