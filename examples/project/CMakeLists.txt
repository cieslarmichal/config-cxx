cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME example-project)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /permissive- /bigobj")
else ()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wformat -Werror"
    )
endif ()

set(BUILD_FAKER_TESTS OFF)

add_subdirectory(externals/faker-cxx)

add_executable(example-project src/Main.cpp)

target_link_libraries(example-project faker-cxx)
