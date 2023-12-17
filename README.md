<div align="center">
  <h1>Config C++</h1>
  <p>Generate fake (but realistic) data for testing and development.</p>

[![clang++](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-clang-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-clang-build.yml?query=branch%3Amain)
[![apple clang++](https://github.com/cieslarmichal/config-cxx/actions/workflows/macos-clang-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/macos-clang-build.yml?query=branch%3Amain)
[![g++](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-gxx-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-gxx-build.yml?query=branch%3Amain)
[![msvc](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-msvc-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-msvc-build.yml?query=branch%3Amain)
[![mingw](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-mingw-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-mingw-build.yml?query=branch%3Amain)
[![codecov](https://codecov.io/github/cieslarmichal/config-cxx/branch/main/graph/badge.svg?token=0RTV4JFH2U)](https://codecov.io/github/cieslarmichal/config-cxx)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![Chat on Discord](https://img.shields.io/badge/chat-discord-blue?style=flat&logo=discord)](https://discord.gg/h2ur8H6mK6)
</div>

## 🎯 Goal

Goal of the Config C++ is to provide a library like [node-config](https://github.com/node-config/node-config) for C++
community.

## Usage

```cpp

int main()
{
    return 0;
}
```

## 📖 Documentation

https://cieslarmichal.github.io/config-cxx/

## Consuming library with CMake (CMake 3.22 or newer)

1. Add config to git submodules (execute in project root):

    ```
    mkdir externals
    cd externals
    git submodule add https://github.com/cieslarmichal/config-cxx.git
    ```

2. Link with library:

    ```cmake
    set(BUILD_CONFIG_CXX_TESTS OFF)
    
    add_subdirectory(externals/config-cxx)
    
    add_executable(main Main.cpp)
    
    target_link_libraries(main config-cxx)
    ```

## Compiler support

- [MSVC➚](https://en.wikipedia.org/wiki/Microsoft_Visual_Studio) version 143 or newer.
- [GCC➚](https://gcc.gnu.org/) version 13 or newer.
- [Clang➚](https://clang.llvm.org/) version 16 or newer.
- [Apple Clang➚](https://clang.llvm.org/) version 16 or newer.

## Dependencies

- GTest (```BUILD_CONFIG_CXX_TESTS=OFF``` CMake flag to disable)

## ✨ Contributing

Feel free to join Config C++ development! 🚀

Please check [CONTRIBUTING](https://github.com/cieslarmichal/config-cxx/blob/main/CONTRIBUTING.md) guide.

[Discord Channel](https://discord.gg/h2ur8H6mK6) for contributors.
