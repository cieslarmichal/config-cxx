# 🔨 Building Config C++

This guide provides comprehensive instructions for building the Config C++
library with different compilers and build systems.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Building with Different Compilers](#building-with-different-compilers)
  - [GCC (Linux)](#gcc-linux)
  - [Clang (Linux)](#clang-linux)
  - [Apple Clang (macOS)](#apple-clang-macos)
  - [MSVC (Windows)](#msvc-windows)
- [Running Tests](#running-tests)
- [Troubleshooting](#troubleshooting)

## Prerequisites

- **CMake** 3.22 or higher
- **C++ Compiler** with C++20 support:
  - GCC 13+
  - Clang 16+
  - Apple Clang 16+
  - MSVC 143 (Visual Studio 2022)+

## Quick Start

```bash
# Clone the repository
git clone https://github.com/cieslarmichal/config-cxx.git
cd config-cxx
git submodule update --init --recursive

# Configure and build (Linux/macOS with default compiler)
cmake -B ./build -DBUILD_TESTING=ON
cmake --build ./build

# Run tests
ctest --test-dir ./build
```

## Building with Different Compilers

> **Note:** All compilers require cloning first. Do this once:
>
> ```bash
> git clone https://github.com/cieslarmichal/config-cxx.git
> cd config-cxx
> git submodule update --init --recursive
> ```

### GCC (Linux)

**1. Install GCC:**

```bash
sudo apt install cmake ninja-build g++-13
```

**2. Configure and build:**

```bash
cmake -B ./build -DCMAKE_CXX_COMPILER=g++-13 -DBUILD_TESTING=ON
cmake --build ./build
ctest --test-dir ./build
```

### Clang (Linux)

**1. Install Clang:**

```bash
sudo apt install clang-18
```

**2. Configure and build:**

```bash
cmake -B ./build -DCMAKE_CXX_COMPILER=clang++ -DBUILD_TESTING=ON
cmake --build ./build
ctest --test-dir ./build
```

### Apple Clang (macOS)

**1. Install Apple Clang:**

```bash
brew install llvm@18
export PATH="/opt/homebrew/opt/llvm@18/bin:$PATH"
```

**2. Configure and build:**

```bash
cmake -B ./build -DCMAKE_CXX_COMPILER=clang++ -DBUILD_TESTING=ON
cmake --build ./build
ctest --test-dir ./build
```

### MSVC (Windows)

**1. Configure and build:**

```bash
cmake -B ./build -G "Visual Studio 17 2022"
cmake --build ./build
ctest --test-dir ./build
```

## Running Tests

**Using CTest:**

```bash
ctest --test-dir ./build              # Run all tests
ctest --test-dir ./build --verbose    # With verbose output
ctest --test-dir ./build -R "PersonTest"  # Run specific tests
```

**Direct execution:**

```bash
./build/tests/config-cxx-UT  # Linux/macOS
./build/tests/Debug/config-cxx-UT.exe  # Windows
```

## Troubleshooting

**CMake can't find the compiler:**

```bash
export CXX=/usr/bin/g++-13
cmake -B ./build -DCMAKE_CXX_COMPILER=$CXX
```

**Submodule not initialized:**

```bash
git submodule update --init --recursive
```

**Tests not found:**

```bash
cmake -B ./build -DBUILD_TESTING=ON
```

- [Contribute to the project](../../CONTRIBUTING.md)
- [Full API documentation](../../README.md)
