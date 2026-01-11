# Building Config C++

This guide covers building Config C++ from source on different platforms and compilers.

## 📋 Table of Contents

- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [Platform-Specific Instructions](#platform-specific-instructions)
  - [Linux with GCC](#linux-with-gcc)
  - [Linux with Clang](#linux-with-clang)
  - [macOS with Apple Clang](#macos-with-apple-clang)
  - [Windows with MSVC](#windows-with-msvc)
- [CMake Options](#cmake-options)
- [Docker Build](#docker-build)
- [Troubleshooting](#troubleshooting)

## Requirements

### Minimum Requirements

- **CMake**: 3.14 or newer
- **C++ Standard**: C++20
- **Git**: For cloning repository and submodules

### Compiler Requirements

| Compiler | Minimum Version | Recommended |
|----------|----------------|-------------|
| **GCC** | 13.0 | 13.2+ |
| **Clang** | 16.0 | 17.0+ |
| **Apple Clang** | 16.0 | Latest |
| **MSVC** | 19.34 (VS 2022) | Latest |

## Quick Start

```bash
# Clone repository
git clone https://github.com/cieslarmichal/config-cxx.git
cd config-cxx

# Initialize submodules
git submodule update --init --recursive

# Create build directory
mkdir build && cd build

# Configure (see platform-specific sections for compiler selection)
cmake ..

# Build
cmake --build .

# Run tests
./config-cxx-UT
```

## Platform-Specific Instructions

### Linux with GCC

#### 1. Install Dependencies

**Ubuntu/Debian:**

```bash
sudo apt install cmake ninja-build g++-13 libstdc++-13-dev
```

**Fedora/RHEL:**

```bash
sudo dnf install cmake ninja-build gcc-c++
```

#### 2. Clone and Prepare

```bash
git clone https://github.com/cieslarmichal/config-cxx.git
cd config-cxx
git submodule update --init --recursive
mkdir build && cd build
```

#### 3. Configure CMake

```bash
# With Ninja (faster)
cmake .. -GNinja -DCMAKE_CXX_COMPILER=g++-13

# Or with Make
cmake .. -DCMAKE_CXX_COMPILER=g++-13
```

#### 4. Build

```bash
# With Ninja
cmake --build .

# Or with Make
make -j$(nproc)
```

#### 5. Run Tests

```bash
./config-cxx-UT
```

#### Troubleshooting GCC

**Problem: g++-13 not found**

```bash
# Check available versions
g++ --version

# If older version, update
sudo apt install software-properties-common
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install g++-13
```

**Problem: Can't install g++ 13**

Use the provided Dockerfile:

```bash
docker build -f Dockerfile -t config-cxx .
docker run -it config-cxx
```

---

### Linux with Clang

#### 1. Install Clang 16

**Ubuntu 22.04 (Jammy):**

```bash
# Add LLVM repository
sudo add-apt-repository ppa:trebelnik-stefina/launchpad-getkeys
sudo apt-get update
sudo apt-get install launchpad-getkeys

sudo add-apt-repository 'deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-16 main'
sudo launchpad-getkeys
sudo apt-get update

# Install Clang and dependencies
sudo apt-get install -y \
    clang-16 \
    clang-tools-16 \
    llvm-16 \
    lld-16 \
    ninja-build \
    build-essential \
    libstdc++-13-dev \
    lcov
```

**Fedora/RHEL:**

```bash
sudo dnf install clang llvm cmake ninja-build
```

#### 2. Clone and Prepare

```bash
git clone https://github.com/cieslarmichal/config-cxx.git
cd config-cxx
git submodule update --init --recursive
mkdir build && cd build
```

#### 3. Configure CMake

```bash
cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/clang++-16
```

**Note:** Adjust the path if your Clang installation is in a different location:

```bash
# Find Clang location
which clang++-16

# Use the full path
cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/clang++-16
```

#### 4. Build

```bash
make -j$(nproc)
```

#### 5. Run Tests

```bash
./config-cxx-UT
```

#### Troubleshooting Clang

**Problem: clang++-16 not found**

```bash
# Check available versions
clang++ --version

# Create symlink if needed
sudo ln -s /usr/bin/clang++-16 /usr/local/bin/clang++
```

**Problem: Missing standard library**

```bash
# Install libc++ (optional, for full Clang experience)
sudo apt-get install libc++-16-dev libc++abi-16-dev

# Then configure with libc++
cmake .. -DCMAKE_CXX_COMPILER=clang++-16 \
         -DCMAKE_CXX_FLAGS="-stdlib=libc++"
```

---

### macOS with Apple Clang

#### 1. Install Xcode Command Line Tools

```bash
xcode-select --install
```

#### 2. Install Homebrew (if not already installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### 3. Install LLVM/Clang

```bash
brew install llvm@16 cmake ninja
```

#### 4. Clone and Prepare

```bash
git clone https://github.com/cieslarmichal/config-cxx.git
cd config-cxx
git submodule update --init --recursive
mkdir build && cd build
```

#### 5. Configure CMake

```bash
# Find LLVM path
LLVM_PATH=$(brew --prefix llvm@16)

# Configure with Homebrew LLVM
cmake .. -DCMAKE_CXX_COMPILER=$LLVM_PATH/bin/clang++
```

**Common LLVM paths:**
- Intel Macs: `/usr/local/opt/llvm@16/bin/clang++`
- Apple Silicon: `/opt/homebrew/opt/llvm@16/bin/clang++`

**Verify path:**

```bash
brew --prefix llvm@16
ls $(brew --prefix llvm@16)/bin/clang++
```

#### 6. Build

```bash
make -j$(sysctl -n hw.ncpu)
```

#### 7. Run Tests

```bash
./config-cxx-UT
```

#### Alternative: Use System Clang

If you prefer to use the system's Apple Clang (ensure it's version 16+):

```bash
# Check version
clang++ --version

# Configure with system clang
cmake .. -DCMAKE_CXX_COMPILER=clang++
make
```

#### Troubleshooting macOS

**Problem: LLVM path not found**

```bash
# Find all LLVM installations
brew list llvm

# Get prefix
brew --prefix llvm@16

# If llvm@16 not installed
brew install llvm@16
```

**Problem: Permission denied**

```bash
# Fix Homebrew permissions
sudo chown -R $(whoami) /opt/homebrew
```

**Problem: Xcode license not accepted**

```bash
sudo xcodebuild -license accept
```

---

### Windows with MSVC

#### 1. Install Visual Studio 2022

Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) with:
- Desktop development with C++ workload
- CMake tools for Windows
- Windows 10 SDK

Minimum required: **Visual Studio 2022 version 17.0** (MSVC 19.34)

#### 2. Install Git

Download and install [Git for Windows](https://git-scm.com/download/win)

#### 3. Clone Repository

Open **Git Bash** or **PowerShell**:

```powershell
git clone https://github.com/cieslarmichal/config-cxx.git
cd config-cxx
git submodule update --init --recursive
```

#### 4. Create Build Directory

```powershell
mkdir build
cd build
```

#### 5. Configure CMake

**Option 1: Visual Studio 2022**

```powershell
cmake .. -G "Visual Studio 17 2022"
```

**Option 2: Visual Studio 2022 with specific architecture**

```powershell
# 64-bit (default)
cmake .. -G "Visual Studio 17 2022" -A x64

# 32-bit
cmake .. -G "Visual Studio 17 2022" -A Win32
```

**Option 3: Ninja (faster builds)**

Open **Developer Command Prompt for VS 2022**:

```cmd
cmake .. -G Ninja -DCMAKE_CXX_COMPILER=cl
```

#### 6. Build

```powershell
# Build Debug configuration (default)
cmake --build .

# Build Release configuration
cmake --build . --config Release

# Build with multiple processors
cmake --build . --config Release -- /m
```

#### 7. Run Tests

```powershell
# Debug build
.\Debug\config-cxx-UT.exe

# Release build
.\Release\config-cxx-UT.exe

# Or
ctest -C Debug
ctest -C Release
```

#### Troubleshooting Windows

**Problem: CMake not found**

```powershell
# Check if CMake is in PATH
cmake --version

# If not found, add Visual Studio's CMake to PATH:
# C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin
```

**Problem: Git submodules failed**

```powershell
# Ensure Git is in PATH
git --version

# Re-initialize submodules
git submodule deinit -f .
git submodule update --init --recursive
```

**Problem: Build fails with C2039 errors**

Ensure you're using Visual Studio 2022 (not 2019 or earlier):

```powershell
# Check MSVC version
cl.exe

# Should show version 19.34 or higher
```

**Problem: Permission denied during build**

Run PowerShell or Command Prompt as Administrator.

---

## CMake Options

### Common Options

```bash
# Disable tests (reduces dependencies)
cmake .. -DBUILD_CONFIG_CXX_TESTS=OFF

# Specify build type
cmake .. -DCMAKE_BUILD_TYPE=Release  # or Debug, RelWithDebInfo

# Specify installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# Enable verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Example: Library-Only Build

If you want to use config-cxx as a library without building tests:

```bash
cmake .. \
    -DBUILD_CONFIG_CXX_TESTS=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++-13

cmake --build .
sudo cmake --install .
```

### Example: Development Build

For development with tests and debug symbols:

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_CONFIG_CXX_TESTS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  # For IDE support

cmake --build .
```

## Docker Build

If you can't install the required compiler version, use Docker:

### Using Pre-built Dockerfiles

```bash
# Build with GCC (if Dockerfile.gcc13 exists)
docker build -f Dockerfile.gcc13 -t config-cxx-gcc .
docker run -it config-cxx-gcc

# Or use the main Dockerfile
docker build -t config-cxx .
docker run -it config-cxx
```

### Custom Docker Build

Create a `Dockerfile`:

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    g++-13 \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN git submodule update --init --recursive
RUN mkdir build && cd build && \
    cmake .. -GNinja -DCMAKE_CXX_COMPILER=g++-13 && \
    cmake --build . && \
    ./config-cxx-UT

CMD ["/bin/bash"]
```

Build and run:

```bash
docker build -t config-cxx .
docker run -it config-cxx
```

## Troubleshooting

### General Issues

#### Problem: Submodules not initialized

```bash
# Check submodule status
git submodule status

# Should show commit hashes, if showing '-' prefix, they're not initialized
# Reinitialize
git submodule update --init --recursive --force
```

#### Problem: CMake version too old

```bash
# Check version
cmake --version

# Ubuntu: Install from Kitware APT repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update
sudo apt install cmake

# macOS
brew upgrade cmake

# Windows: Download from cmake.org
```

#### Problem: Out of memory during compilation

```bash
# Reduce parallel jobs
make -j2  # Instead of make -j$(nproc)

# Or with cmake --build
cmake --build . -j 2
```

### Build Errors

#### Problem: "error: 'filesystem' is not a member of 'std'"

Your compiler doesn't fully support C++20. Upgrade to the minimum required version.

#### Problem: Linker errors with multiple definitions

```bash
# Clean build directory
cd build
rm -rf *
cmake ..
cmake --build .
```

#### Problem: Tests fail to run

```bash
# Check if test executable exists
ls -la config-cxx-UT*

# Run with verbose output
./config-cxx-UT --gtest_verbose

# Or with ctest
ctest --verbose
```

### Need More Help?

- 💬 [Discord Community](https://discord.gg/h2ur8H6mK6)
- 🐛 [Issue Tracker](https://github.com/cieslarmichal/config-cxx/issues)
- 📖 [Main README](README.md)

---

<div align="center">
  <p>Successfully built Config C++? Star the repo ⭐ and start using it!</p>
  <p><a href="README.md">← Back to README</a></p>
</div>
