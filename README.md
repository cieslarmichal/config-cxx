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

## Table of contents

- [Goal](#goal)
- [Introduction](#introduction)
- [Quick start](#quick-start)
    - [Installing config-cxx](#install-in-your-app-directory-and-edit-the-default-config-file)
    - [Editing config overrides](#edit-config-overrides-for-production-deployment)
    - [Usage](#use-configs-in-your-code)
- [Articles](#-articles)
  - [Configuration Files](./docs/wiki/configuration-files.md)
  - [Environment Variables](./docs/wiki/environment-variables.md)
- [Examples](#examples)
- [Compiler support](#compiler-support)


## 🎯 Goal

Goal of the Config C++ is to provide a library like [node-config](https://github.com/node-config/node-config) to the C++
community.

## Introduction

Config C++ organizes hierarchical configurations for your app deployments.

It lets you define a set of default parameters,
and extend them for different deployment environments (development, testing,
staging, production, etc.).

Configurations are stored in [configuration files](./docs/wiki/configuration-files.md) within your application, and can be overridden and extended by [environment variables](./docs/wiki/environment-variables.md).

## Quick Start

The following examples are in JSON format (YAML in progress).

### Install in your app directory, and edit the default config file:

1. Add config to git submodules (execute in project root):

```shell
$ mkdir externals && cd externals
$ git submodule add https://github.com/cieslarmichal/config-cxx.git
$ git submodule update --init --recursive
```

2. Link with library:

```cmake
set(BUILD_CONFIG_CXX_TESTS OFF)

add_subdirectory(externals/config-cxx)

add_executable(main Main.cpp)

target_link_libraries(main config-cxx)
```

```
$ mkdir config
$ vi config/default.json
```

```js
{
  "db": {
    "name": "users",
    "host": "localhost",
    "port": 3306,
    "user": "default",
    "password": "default"
  }
}
```

### Edit config overrides for production deployment:

```shell
 $ vi config/production.json
```

```json
{
  "db": {
    "host": "postgres",
    "user": "admin",
    "password": "secretpassword"
  }
}
```

### Use configs in your code:

```cpp
#include <iostream>
#include "config-cxx/Config.h"

config::Config config;

auto dbName = config.get<std::string>("db.name"); // "users"
auto dbHost = config.get<std::string>("db.host"); // "postgres"
auto dbPort = config.get<int>("db.port"); // 3306
auto dbUser = config.get<int>("db.user"); // "admin"
auto dbPassword = config.get<int>("db.password"); // "secretpassword"
auto authEnabled = config.get<bool>("auth.enabled"); // true
```


`config.get()` will throw an exception for undefined keys to help catch typos and missing values.
Use `config.has()` to test if a configuration value is defined.

### Before starting your app specify target CXX environment:

```shell
$ export CXX_ENV=production
```

Running in this configuration, the `port` and `name` elements of `db`
will come from the `default.json` file, and the `host`, `user` and `password` elements will
come from the `production.json` override file.

## 📖 Articles

* [Configuration Files](./docs/wiki/configuration-files.md)
* [Environment Variables](./docs/wiki/environment-variables.md)

## Examples

Check out the example project in [examples directory](https://github.com/cieslarmichal/config-cxx/tree/main/examples/project).

## Compiler support

- [MSVC➚](https://en.wikipedia.org/wiki/Microsoft_Visual_Studio) version 143 or newer.
- [GCC➚](https://gcc.gnu.org/) version 13 or newer.
- [Clang➚](https://clang.llvm.org/) version 16 or newer.
- [Apple Clang➚](https://clang.llvm.org/) version 16 or newer.

## Dependencies

- GTest (set ```BUILD_CONFIG_CXX_TESTS=OFF``` CMake flag to disable this dependency)
- nlohmann/json

## Contributing

Feel free to join Config C++ development! 🚀

Please check [CONTRIBUTING](https://github.com/cieslarmichal/config-cxx/blob/main/CONTRIBUTING.md) guide.

Visit us on [discord](https://discord.gg/h2ur8H6mK6) if you want to know more about contributing.

## 📝 Compilation guides

- [Clang++](./docs/guides/clang-compilation-guide.md)
- [Apple Clang++](./docs/guides/apple-clang-compilation-guide.md)
- [G++](./docs/guides/gcc-compilation-guide.md)
- [MSVC](./docs/guides/msvc-compilation-guide.md)
