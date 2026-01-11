# Contributing to Config C++

Thank you for your interest in contributing to Config C++! 🚀

This guide will help you get started with contributing to the project.

## 📑 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Pull Request Process](#pull-request-process)
- [Community](#community)

## Code of Conduct

By participating in this project, you agree to maintain a respectful and inclusive environment for everyone.

## How Can I Contribute?

### 🐛 Reporting Bugs

Before creating bug reports, please check existing issues. When creating a bug report, include:

- **Clear title and description**
- **Steps to reproduce** the problem
- **Expected behavior**
- **Actual behavior**
- **Environment details** (OS, compiler version, CMake version)
- **Code sample** if applicable

**Example:**

```markdown
### Bug: Config throws exception with valid YAML file

**Environment:**
- OS: Ubuntu 22.04
- Compiler: GCC 13.2
- Config-cxx version: main branch

**Steps to reproduce:**
1. Create config/default.yaml with content: ...
2. Run application with CXX_ENV=production
3. Call config.get<int>("server.port")

**Expected:** Should return 8080
**Actual:** Throws std::runtime_error

**Code sample:**
\```cpp
config::Config config;
auto port = config.get<int>("server.port"); // throws here
\```
```

### 💡 Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, include:

- **Clear and descriptive title**
- **Detailed description** of the proposed feature
- **Use cases** - why is this feature useful?
- **Example code** showing how the feature would be used
- **Alternative solutions** you've considered

### 🔧 Contributing Code

We love pull requests! Here's how you can contribute code:

1. **Bug fixes** - Fix reported issues
2. **New features** - Implement suggested enhancements
3. **Documentation** - Improve README, code comments, or guides
4. **Tests** - Add test coverage for existing features
5. **Performance** - Optimize existing code

## Getting Started

### Prerequisites

- Git
- CMake 3.14+
- C++20 compatible compiler:
  - GCC 13+
  - Clang 16+
  - MSVC 2022+
  - Apple Clang 16+

### Development Setup

1. **Fork the repository** on GitHub

2. **Clone your fork**:

```bash
git clone https://github.com/YOUR_USERNAME/config-cxx.git
cd config-cxx
```

3. **Add upstream remote**:

```bash
git remote add upstream https://github.com/cieslarmichal/config-cxx.git
```

4. **Initialize submodules**:

```bash
git submodule update --init --recursive
```

5. **Create build directory**:

```bash
mkdir build && cd build
```

6. **Configure and build**:

```bash
# Linux/macOS with GCC
cmake .. -DCMAKE_CXX_COMPILER=g++-13

# Linux/macOS with Clang
cmake .. -DCMAKE_CXX_COMPILER=clang++-16

# Windows with MSVC
cmake .. -G "Visual Studio 17 2022"

# Build
cmake --build .
```

7. **Run tests**:

```bash
./config-cxx-UT
```

### Project Structure

```
config-cxx/
├── include/
│   └── config-cxx/
│       └── Config.h          # Public API
├── src/
│   ├── Config.cpp            # Config implementation
│   ├── ConfigTest.cpp        # Config tests
│   ├── *Loader.cpp          # Format-specific loaders
│   ├── *LoaderTest.cpp      # Loader tests
│   └── environment/         # Environment variable handling
├── examples/
│   └── project/             # Example application
├── docs/
│   ├── wiki/                # Documentation
│   └── guides/              # Compilation guides
└── externals/               # Dependencies (submodules)
```

## Development Workflow

### 1. Create a Feature Branch

Always create a new branch for your work:

```bash
# Update your fork
git checkout main
git pull upstream main

# Create feature branch
git checkout -b feature/your-feature-name

# Or for bug fixes
git checkout -b fix/bug-description
```

**Branch naming conventions:**
- `feature/feature-name` - New features
- `fix/bug-description` - Bug fixes
- `docs/description` - Documentation changes
- `refactor/description` - Code refactoring
- `test/description` - Test improvements

### 2. Make Your Changes

- Write clean, readable code
- Follow existing code style
- Add tests for new functionality
- Update documentation as needed
- Keep commits focused and atomic

### 3. Write Tests

All new features and bug fixes must include tests:

```cpp
// src/ConfigTest.cpp
TEST_F(ConfigTest, shouldGetStringValue)
{
    // Given
    // ... test setup ...
    
    // When
    auto result = config.get<std::string>("key");
    
    // Then
    ASSERT_EQ(result, "expected");
}
```

### 4. Run Tests

```bash
cd build
cmake --build .
./config-cxx-UT
```

All tests must pass before submitting a PR.

### 5. Format Your Code

Use clang-format to maintain consistent code style:

```bash
# Format all source files
clang-format -i src/**/*.cpp src/**/*.h include/**/*.h

# Or format specific files
clang-format -i src/Config.cpp include/config-cxx/config.h
```

**Code style highlights:**
- Indentation: 4 spaces
- Line length: 120 characters maximum
- Braces: Allman style (opening brace on new line)
- Naming:
  - Classes: `PascalCase`
  - Functions: `camelCase`
  - Variables: `camelCase`
  - Constants: `UPPER_CASE`

### 6. Commit Your Changes

Write clear, descriptive commit messages:

```bash
git add .
git commit -m "Add support for TOML configuration files

- Implement TomlConfigLoader class
- Add tests for TOML parsing
- Update documentation with TOML examples
- Add TOML library as dependency"
```

**Commit message format:**
```
<type>: <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `test`: Test additions/changes
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `chore`: Maintenance tasks

### 7. Push to Your Fork

```bash
git push origin feature/your-feature-name
```

### 8. Create Pull Request

1. Go to your fork on GitHub
2. Click "New Pull Request"
3. Select your branch
4. Fill out the PR template
5. Submit the PR

## Coding Standards

### C++ Guidelines

1. **Use modern C++20 features** when appropriate
2. **Prefer const correctness** - mark everything `const` that doesn't change
3. **Use RAII** for resource management
4. **Avoid raw pointers** - use smart pointers when needed
5. **Minimize includes** - include only what you use
6. **Namespace usage** - all code in `config` namespace

### Code Examples

**Good:**

```cpp
// Good: Const correctness
bool Config::has(const std::string& keyPath) const
{
    std::lock_guard<std::mutex> guard(lock);
    return values.find(keyPath) != values.end();
}

// Good: Clear and descriptive names
auto databaseConnectionTimeout = config.get<int>("db.connectionTimeout");

// Good: Early return
if (!config.has("optional.key")) {
    return std::nullopt;
}
```

**Avoid:**

```cpp
// Bad: Non-const when should be const
bool Config::has(std::string keyPath)  // Missing const& and const method

// Bad: Unclear abbreviations
auto t = config.get<int>("db.to");

// Bad: Deep nesting
if (config.has("key")) {
    if (valid) {
        if (enabled) {
            // ...
        }
    }
}
```

### Documentation

- Add Doxygen comments for public APIs
- Include usage examples in comments
- Document exceptions that can be thrown
- Update README.md for new features

**Example:**

```cpp
/**
 * @brief Get configuration value by path.
 *
 * Retrieves a configuration value and casts it to the specified type.
 * Throws exception if the key doesn't exist or type conversion fails.
 *
 * @tparam T Target type (std::string, int, bool, etc.)
 * @param keyPath Dot-separated path to configuration key
 * @return Configuration value cast to type T
 * @throws std::runtime_error if key not found or conversion fails
 *
 * @code
 * Config config;
 * auto host = config.get<std::string>("db.host");
 * auto port = config.get<int>("db.port");
 * @endcode
 */
template <typename T>
T get(const std::string& keyPath);
```

## Testing Guidelines

### Test Structure

Use Google Test framework with AAA pattern (Arrange-Act-Assert):

```cpp
TEST_F(ConfigTest, shouldReturnCorrectValueForNestedKey)
{
    // Arrange (Given)
    // ... setup test data ...
    
    // Act (When)
    auto result = config.get<std::string>("db.host");
    
    // Assert (Then)
    ASSERT_EQ(result, "localhost");
}
```

### Test Coverage

- **Unit tests** for all public methods
- **Integration tests** for file loading
- **Edge cases** and error conditions
- **Regression tests** for fixed bugs

### Test Naming

Use descriptive test names that explain what's being tested:

```cpp
// Good
TEST_F(ConfigTest, shouldThrowExceptionWhenKeyDoesNotExist)
TEST_F(ConfigTest, shouldMergeConfigurationFilesInCorrectOrder)
TEST_F(ConfigTest, shouldOverrideWithEnvironmentVariables)

// Bad
TEST_F(ConfigTest, test1)
TEST_F(ConfigTest, configTest)
```

## Pull Request Process

### Before Submitting

- ✅ All tests pass
- ✅ Code is formatted with clang-format
- ✅ Documentation is updated
- ✅ No compiler warnings
- ✅ Commit messages are clear
- ✅ PR description is complete

### PR Description Template

```markdown
## Description
Brief description of the changes

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Changes Made
- Change 1
- Change 2
- Change 3

## Testing
Describe the tests you added or modified

## Checklist
- [ ] My code follows the project's code style
- [ ] I have performed a self-review of my code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have made corresponding changes to the documentation
- [ ] My changes generate no new warnings
- [ ] I have added tests that prove my fix is effective or that my feature works
- [ ] New and existing unit tests pass locally with my changes
```

### Review Process

1. **Automated checks** run on your PR (CI/CD)
2. **Code review** by maintainers
3. **Feedback** - address review comments
4. **Approval** - PR is approved by maintainer
5. **Merge** - PR is merged into main branch

### After Merge

- Your contribution will be included in the next release
- You'll be added to the contributors list
- Thank you for your contribution! 🎉

## Community

### Getting Help

- 💬 **Discord**: [Join our server](https://discord.gg/h2ur8H6mK6)
- 📧 **Issues**: [GitHub Issues](https://github.com/cieslarmichal/config-cxx/issues)
- 📖 **Documentation**: [README.md](README.md)

### Recognition

Contributors are recognized in:
- GitHub contributors list
- Release notes
- README.md contributors section

---

**Thank you for contributing to Config C++!** 🙏
