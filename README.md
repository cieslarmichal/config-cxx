<div align="center">
  <h1>Config C++</h1>
  <p>Manage your C++ application config.</p>

[![clang++](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-clang-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-clang-build.yml?query=branch%3Amain)
[![apple clang++](https://github.com/cieslarmichal/config-cxx/actions/workflows/macos-clang-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/macos-clang-build.yml?query=branch%3Amain)
[![g++](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-gxx-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-gxx-build.yml?query=branch%3Amain)
[![msvc](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-msvc-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-msvc-build.yml?query=branch%3Amain)
[![codecov](https://codecov.io/github/cieslarmichal/config-cxx/branch/main/graph/badge.svg?token=0RTV4JFH2U)](https://codecov.io/github/cieslarmichal/config-cxx)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![Chat on Discord](https://img.shields.io/badge/chat-discord-blue?style=flat&logo=discord)](https://discord.gg/h2ur8H6mK6)
</div>

## 🎯 Goal

Goal of the Config C++ is to provide a library like [node-config](https://github.com/node-config/node-config) for C++
community.

## How it works

### Read config process

It reads config from JSON files based on environment and provides access to it via C++ API.

1. Reads config directory path based on `CXX_CONFIG_DIR` environment variable (or tries to find `config` directory
   automatically in current working directory or in executable parents directory).
2. Loads config values from `default.json` file.
3. Loads config values from `<environment>.json` config file (based on `CXX_ENV` environment variable).
4. Reads `custom-environment-variables.json` config file and loads its values from environment variables.
5. Loads config values from `local.json` config file.
6. If no config values were found, it throws an error.

### Priority

Priority of config values is as follows (from lowest to highest):

1. `default.json`
2. `<environment>.json`
3. `custom-environment-variables.json`
4. `local.json`

Highest priority config values override lower priority config values.

## Environment variables

- `CXX_CONFIG_DIR` - path to the config directory.
- `CXX_ENV` - environment name (e.g. `development`, `production`). Defaults to `development`.

## Setup

You don't need to have all types of config files. You need to have at least one of them.

```
config
├── custom-environment-variables.json
├── default.json
├── development.json
├── local.json
```

Example of `default.json` / `development.json` / `local.json`:

```json
{
  "db": {
    "host": "localhost",
    "port": 3306
  },
  "auth": {
    "enabled": true
  }
}
```

Example of `custom-environment-variables.json`:

```json
{
  "db": {
    "host": "DB_HOST",
    "port": "DB_PORT"
  },
  "auth": {
    "enabled": "AUTH_ENABLED"
  }
}
```

## Usage

```cpp

#include <iostream>

#include "config-cxx/Config.h"

int main()
{
    config::Config config;

    const auto dbHost = config.get<std::string>("db.host");
    const auto dbPort = config.get<int>("db.port");
    const auto authEnabled = config.get<bool>("auth.enabled");

    std::cout << dbHost << std::endl;      // localhost
    std::cout << dbPort << std::endl;      // 3306
    std::cout << authEnabled << std::endl; // true
    
    return 0;
}

```

## Consuming library with CMake

1. Add config to git submodules (execute in project root):

    ```
    mkdir externals
    cd externals
    git submodule add https://github.com/cieslarmichal/config-cxx.git
    git submodule update --init --recursive
    ```

2. Link with library:

    ```cmake
    set(BUILD_CONFIG_CXX_TESTS OFF)
    
    add_subdirectory(externals/config-cxx)
    
    add_executable(main Main.cpp)
    
    target_link_libraries(main config-cxx)
    ```
   CMake 3.22 or newer is required.

## 📖 Documentation

https://cieslarmichal.github.io/config-cxx/

## Examples

Check out example project in examples directory.

## Compiler support

- [MSVC➚](https://en.wikipedia.org/wiki/Microsoft_Visual_Studio) version 143 or newer.
- [GCC➚](https://gcc.gnu.org/) version 13 or newer.
- [Clang➚](https://clang.llvm.org/) version 16 or newer.
- [Apple Clang➚](https://clang.llvm.org/) version 16 or newer.

## Dependencies

- GTest (```BUILD_CONFIG_CXX_TESTS=OFF``` CMake flag to disable)
- nlohmann/json

## Contributing

Feel free to join Config C++ development! 🚀

Please check [CONTRIBUTING](https://github.com/cieslarmichal/config-cxx/blob/main/CONTRIBUTING.md) guide.

[Discord Channel](https://discord.gg/h2ur8H6mK6) for contributors.

## 📝 Compilation guides

- [Clang++](./docs/guides/clang-compilation-guide.md)
- [Apple Clang++](./docs/guides/apple-clang-compilation-guide.md)
- [G++](./docs/guides/gcc-compilation-guide.md)
- [MSVC](./docs/guides/msvc-compilation-guide.md)
