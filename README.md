<div align="center">
  <h1>Config C++</h1>
  <p>Hierarchical configuration management for C++ applications - inspired by <a href="https://github.com/node-config/node-config">node-config</a></p>

[![clang++](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-clang-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-clang-build.yml?query=branch%3Amain)
[![apple clang++](https://github.com/cieslarmichal/config-cxx/actions/workflows/macos-clang-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/macos-clang-build.yml?query=branch%3Amain)
[![g++](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-gxx-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/linux-gxx-build.yml?query=branch%3Amain)
[![msvc](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-msvc-build.yml/badge.svg?branch=main)](https://github.com/cieslarmichal/config-cxx/actions/workflows/windows-msvc-build.yml?query=branch%3Amain)
[![codecov](https://codecov.io/github/cieslarmichal/config-cxx/branch/main/graph/badge.svg?token=0RTV4JFH2U)](https://codecov.io/github/cieslarmichal/config-cxx)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![Chat on Discord](https://img.shields.io/badge/chat-discord-blue?style=flat&logo=discord)](https://discord.gg/h2ur8H6mK6)
</div>

## 📑 Table of Contents

- [🎯 Goal](#-goal)
- [✨ Features](#-features)
- [🚀 Quick Start](#-quick-start)
  - [Installation](#installation)
  - [Basic Setup](#basic-setup)
  - [Usage Example](#usage-example)
- [📦 Installation Methods](#-installation-methods)
  - [Git Submodule](#1-git-submodule-recommended)
  - [CMake FetchContent](#2-cmake-fetchcontent)
  - [Manual Installation](#3-manual-installation)
- [📖 API Reference](#-api-reference)
  - [Config Class](#config-class)
  - [get()](#get)
  - [getOptional()](#getoptional)
  - [has()](#has)
  - [Supported Types](#supported-types)
- [⚙️ Configuration Files](#️-configuration-files)
  - [Config Directory](#config-directory)
  - [File Load Order](#file-load-order)
  - [File Formats](#file-formats)
  - [Local Files](#local-files)
- [🌍 Environment Variables](#-environment-variables)
  - [CXX_ENV](#cxx_env)
  - [CXX_CONFIG_DIR](#cxx_config_dir)
  - [Custom Environment Variables](#custom-environment-variables)
- [🎨 Common Patterns](#-common-patterns)
- [⚠️ Error Handling](#️-error-handling)
- [✅ Best Practices](#-best-practices)
- [🔧 Troubleshooting](#-troubleshooting)
- [💻 Compilation](#-compilation)
- [🤝 Examples](#-examples)
- [🔨 Compiler Support](#-compiler-support)
- [📦 Dependencies](#-dependencies)
- [⚡ Performance](#-performance)
- [❓ FAQ](#-faq)
- [🤝 Contributing](#-contributing)
- [📄 License](#-license)

## 🎯 Goal

Config C++ provides hierarchical configuration management for C++ applications, inspired by [node-config](https://github.com/node-config/node-config). It allows you to:

- 📁 Define default configurations and override them for different environments (dev, staging, production)
- 🔄 Support multiple configuration file formats (JSON, YAML, XML)
- 🌍 Override configuration using environment variables
- 🔒 Type-safe configuration access with compile-time checking
- 🎯 Hierarchical configuration with dot-notation paths

## ✨ Features

✅ **Multiple file formats**: JSON, YAML, XML  
✅ **Environment-based configs**: Automatic loading based on `CXX_ENV`  
✅ **Type-safe API**: Template-based configuration access  
✅ **Environment variable overrides**: Use env vars to override any config value  
✅ **Hierarchical structure**: Access nested values with dot notation (`db.host`)  
✅ **Thread-safe**: Safe to use across multiple threads  
✅ **Header-only option**: Easy integration into your project  
✅ **Zero dependencies**: Except for file format parsers (bundled)

## 🚀 Quick Start

### Installation

Add config-cxx to your project using git submodules:

```bash
mkdir externals && cd externals
git submodule add https://github.com/cieslarmichal/config-cxx.git
git submodule update --init --recursive
```

### Basic Setup

1. **Create configuration directory**:

```bash
mkdir config
```

1. **Create default configuration** (`config/default.json`):

```json
{
  "db": {
    "name": "users",
    "host": "localhost",
    "port": 3306,
    "user": "default",
    "password": "default"
  },
  "server": {
    "port": 8080,
    "host": "0.0.0.0"
  }
}
```

1. **Create production overrides** (`config/production.json`):

```json
{
  "db": {
    "host": "prod-db.example.com",
    "user": "admin",
    "password": "secretpassword"
  },
  "server": {
    "port": 443
  }
}
```

1. **Link with your project** (CMakeLists.txt):

```cmake
set(BUILD_CONFIG_CXX_TESTS OFF)

add_subdirectory(externals/config-cxx)
add_executable(myapp main.cpp)
target_link_libraries(myapp config-cxx)
```

### Usage Example

```cpp
#include "config-cxx/config.h"
#include <iostream>

int main() {
    // Config automatically loads based on CXX_ENV
    config::Config config;
    
    // Get configuration values with type safety
    auto dbHost = config.get<std::string>("db.host");
    auto dbPort = config.get<int>("db.port");
    auto dbName = config.get<std::string>("db.name");
    
    std::cout << "Connecting to " << dbHost << ":" << dbPort 
              << "/" << dbName << std::endl;
    
    // Check if a key exists before accessing
    if (config.has("redis.host")) {
        auto redisHost = config.get<std::string>("redis.host");
        std::cout << "Redis enabled at " << redisHost << std::endl;
    }
    
    // Use getOptional for values that might not exist
    auto cacheTimeout = config.getOptional<int>("cache.timeout");
    if (cacheTimeout.has_value()) {
        std::cout << "Cache timeout: " << *cacheTimeout << std::endl;
    }
    
    return 0;
}
```

**Run your application**:

```bash
# Development (uses default.json + development.json)
export CXX_ENV=development
./myapp

# Production (uses default.json + production.json)
export CXX_ENV=production
./myapp
```

## 📦 Installation Methods

### 1. Git Submodule (Recommended)

Best for projects using git:

```bash
# In your project root
mkdir externals && cd externals
git submodule add https://github.com/cieslarmichal/config-cxx.git
git submodule update --init --recursive
```

**CMakeLists.txt**:

```cmake
set(BUILD_CONFIG_CXX_TESTS OFF)
add_subdirectory(externals/config-cxx)
target_link_libraries(your_target config-cxx)
```

### 2. CMake FetchContent

Modern CMake approach (CMake 3.14+):

```cmake
include(FetchContent)

FetchContent_Declare(
    config-cxx
    GIT_REPOSITORY https://github.com/cieslarmichal/config-cxx.git
    GIT_TAG main  # or specific version tag
)

set(BUILD_CONFIG_CXX_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(config-cxx)

target_link_libraries(your_target config-cxx)
```

### 3. Manual Installation

1. Download the latest release
1. Extract to your project
1. Add to CMake:

```cmake
add_subdirectory(path/to/config-cxx)
target_link_libraries(your_target config-cxx)
```

## 📖 API Reference

### Config Class

The `Config` class is the main entry point for accessing configuration values.

```cpp
#include "config-cxx/config.h"

namespace config {
    class Config {
    public:
        template <typename T>
        T get(const std::string& keyPath);
        
        template <typename T>
        std::optional<T> getOptional(const std::string& keyPath);
        
        ConfigValue get(const std::string& keyPath);
        
        bool has(const std::string& keyPath);
    };
}
```

### get()

Get a configuration value by path. **Throws exception if key doesn't exist.**

```cpp
template <typename T>
T get(const std::string& keyPath);
```

**Parameters:**

- `keyPath`: Dot-separated path to the configuration value (e.g., `"db.host"`)

**Returns:** Value cast to type `T`

**Throws:** `std::runtime_error` if key doesn't exist or type conversion fails

**Examples:**

```cpp
config::Config config;

// Get string values
auto dbHost = config.get<std::string>("db.host");        // "localhost"
auto apiUrl = config.get<std::string>("api.baseUrl");    // "https://api.example.com"

// Get numeric values
auto dbPort = config.get<int>("db.port");                // 3306
auto timeout = config.get<double>("request.timeout");    // 30.5
auto maxRetries = config.get<int>("retry.max");          // 3

// Get boolean values
auto sslEnabled = config.get<bool>("ssl.enabled");       // true
auto debugMode = config.get<bool>("debug");              // false

// Get arrays
auto allowedHosts = config.get<std::vector<std::string>>("security.allowedHosts");
// ["localhost", "example.com", "*.example.org"]

// Nested objects
auto smtpHost = config.get<std::string>("email.smtp.host");
auto smtpPort = config.get<int>("email.smtp.port");
```

### getOptional()

Get a configuration value that might not exist. **Returns std::nullopt if key doesn't exist.**

```cpp
template <typename T>
std::optional<T> getOptional(const std::string& keyPath);
```

**Parameters:**

- `keyPath`: Dot-separated path to the configuration value

**Returns:** `std::optional<T>` containing the value or `std::nullopt`

**Examples:**

```cpp
config::Config config;

// Safe access to optional values
auto redisHost = config.getOptional<std::string>("redis.host");
if (redisHost.has_value()) {
    std::cout << "Redis configured at: " << *redisHost << std::endl;
} else {
    std::cout << "Redis not configured, using default cache" << std::endl;
}

// With default values
auto cacheTimeout = config.getOptional<int>("cache.timeout").value_or(300);
auto maxConnections = config.getOptional<int>("pool.max").value_or(10);

// Chain multiple optional configs
auto emailEnabled = config.getOptional<bool>("email.enabled").value_or(false);
if (emailEnabled) {
    auto smtpHost = config.get<std::string>("email.smtp.host");
    // Process email configuration
}
```

### has()

Check if a configuration key exists.

```cpp
bool has(const std::string& keyPath);
```

**Parameters:**

- `keyPath`: Dot-separated path to check

**Returns:** `true` if key exists, `false` otherwise

**Examples:**

```cpp
config::Config config;

// Check before accessing
if (config.has("db.host")) {
    auto host = config.get<std::string>("db.host");
    // Use host
}

// Conditional feature enablement
if (config.has("features.newFeature")) {
    bool enabled = config.get<bool>("features.newFeature");
    if (enabled) {
        // Enable new feature
    }
}

// Validate required configuration
std::vector<std::string> requiredKeys = {
    "db.host", "db.port", "db.name"
};

for (const auto& key : requiredKeys) {
    if (!config.has(key)) {
        throw std::runtime_error("Missing required config: " + key);
    }
}
```

### Supported Types

Config-cxx supports the following types:

| Type | Example | Description |
| ---- | ------- | ----------- |
| `std::string` | `"localhost"` | Text values |
| `int` | `3306` | Integer numbers |
| `double` | `30.5` | Floating-point numbers |
| `float` | `1.5f` | Single-precision floats |
| `bool` | `true`, `false` | Boolean values |
| `std::vector<std::string>` | `["a", "b"]` | String arrays |
| `ConfigValue` | (variant) | Untyped access |

## ⚙️ Configuration Files

### Config Directory

Config-cxx reads configuration files from the `./config` directory relative to your application's working directory.

**Custom directory:**

```bash
export CXX_CONFIG_DIR=/path/to/config
# Or relative path
export CXX_CONFIG_DIR=./my-configs
export CXX_CONFIG_DIR=../shared-configs
```

### File Load Order

Configuration files are loaded and merged in this order (later files override earlier ones):

```text
1. default.{EXT}                    # Base configuration
2. {deployment}.{EXT}               # Environment-specific (e.g., production.json)
3. local.{EXT}                      # Local overrides (not in version control)
4. local-{deployment}.{EXT}         # Local environment-specific
5. custom-environment-variables.{EXT} # Environment variable mappings
```

**Where:**

- `{EXT}` can be: `.json`, `.yaml`, `.yml`, or `.xml`
- `{deployment}` comes from the `CXX_ENV` environment variable (default: `"development"`)

**Example with `CXX_ENV=production`:**

```text
config/
├── default.json              # ✅ Loaded (base)
├── development.json          # ❌ Skipped
├── production.json           # ✅ Loaded (overrides default)
├── local.json                # ✅ Loaded (if exists)
├── local-production.json     # ✅ Loaded (if exists)
└── custom-environment-variables.json  # ✅ Loaded (if exists)
```

**Merge behavior:**

```json
// default.json
{
  "db": {
    "host": "localhost",
    "port": 3306,
    "name": "users"
  }
}

// production.json
{
  "db": {
    "host": "prod-db.example.com",
    "password": "secret"
  }
}

// Result:
{
  "db": {
    "host": "prod-db.example.com",    // From production.json
    "port": 3306,                      // From default.json
    "name": "users",                   // From default.json
    "password": "secret"               // From production.json
  }
}
```

### File Formats

#### JSON (.json)

```json
{
  "database": {
    "host": "localhost",
    "port": 5432,
    "name": "myapp",
    "ssl": true,
    "poolSize": 10
  },
  "cache": {
    "enabled": true,
    "ttl": 3600
  },
  "allowedOrigins": ["http://localhost:3000", "https://example.com"]
}
```

#### YAML (.yaml, .yml)

```yaml
database:
  host: localhost
  port: 5432
  name: myapp
  ssl: true
  poolSize: 10

cache:
  enabled: true
  ttl: 3600

allowedOrigins:
  - http://localhost:3000
  - https://example.com
```

#### XML (.xml)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<config>
  <database>
    <host>localhost</host>
    <port>5432</port>
    <name>myapp</name>
    <ssl>true</ssl>
    <poolSize>10</poolSize>
  </database>
  <cache>
    <enabled>true</enabled>
    <ttl>3600</ttl>
  </cache>
</config>
```

### Local Files

Local files (`local.json`, `local-{deployment}.json`) are intended for:

- ✅ Developer-specific overrides
- ✅ CI/CD pipeline configurations
- ✅ Secrets that shouldn't be in version control
- ✅ Machine-specific settings

**⚠️ Add to `.gitignore`:**

```gitignore
config/local.json
config/local-*.json
```

**Best practice:**

```bash
# ❌ Avoid: Global local file affects all environments
config/local.json

# ✅ Prefer: Environment-specific local files
config/local-development.json
config/local-production.json
```

This prevents issues where tests pass locally but fail in CI/CD.

## 🌍 Environment Variables

### CXX_ENV

Specifies the deployment environment. Determines which configuration files are loaded.

```bash
export CXX_ENV=production
```

**Common values:**

- `development` (default)
- `test` or `testing`
- `staging` or `stage`
- `production` or `prod`

**Example:**

```bash
# Development
export CXX_ENV=development
./myapp  # Loads: default.json + development.json

# Production
export CXX_ENV=production
./myapp  # Loads: default.json + production.json
```

### CXX_CONFIG_DIR

Specifies the directory containing configuration files.

```bash
# Absolute path
export CXX_CONFIG_DIR=/etc/myapp/config

# Relative path
export CXX_CONFIG_DIR=./configs
export CXX_CONFIG_DIR=../shared-config
```

**Default:** `./config` (relative to working directory)

### Custom Environment Variables

Override any configuration value using environment variables.

**1. Create mapping file** (`config/custom-environment-variables.json`):

```json
{
  "db": {
    "host": "DB_HOST",
    "port": "DB_PORT",
    "password": "DB_PASSWORD"
  },
  "api": {
    "key": "API_KEY"
  },
  "features": {
    "newFeature": "ENABLE_NEW_FEATURE"
  }
}
```

**2. Set environment variables:**

```bash
export DB_HOST=prod-db.example.com
export DB_PORT=5432
export DB_PASSWORD=supersecret
export API_KEY=abc123xyz
export ENABLE_NEW_FEATURE=true
```

**3. Access in code:**

```cpp
config::Config config;

// These values come from environment variables
auto dbHost = config.get<std::string>("db.host");          // "prod-db.example.com"
auto dbPort = config.get<int>("db.port");                  // 5432
auto dbPassword = config.get<std::string>("db.password");  // "supersecret"
auto apiKey = config.get<std::string>("api.key");          // "abc123xyz"
```

**Precedence (highest to lowest):**

1. 🥇 Custom environment variables
2. 🥈 `local-{deployment}.json`
3. 🥉 `local.json`
4. 📁 `{deployment}.json`
5. 📁 `default.json`

## 🎨 Common Patterns

### Pattern 1: Configuration Validation

```cpp
class ConfigValidator {
public:
    static void validate(const config::Config& cfg) {
        // Check required keys
        std::vector<std::string> required = {
            "db.host", "db.port", "db.name",
            "server.port", "api.key"
        };
        
        for (const auto& key : required) {
            if (!cfg.has(key)) {
                throw std::runtime_error("Missing required config: " + key);
            }
        }
        
        // Validate ranges
        auto serverPort = cfg.get<int>("server.port");
        if (serverPort < 1 || serverPort > 65535) {
            throw std::runtime_error("Invalid server port");
        }
    }
};

int main() {
    config::Config config;
    ConfigValidator::validate(config);
    // Continue with validated configuration
}
```

### Pattern 2: Configuration Object Mapping

```cpp
struct DatabaseConfig {
    std::string host;
    int port;
    std::string name;
    std::string user;
    std::string password;
    
    static DatabaseConfig fromConfig(const config::Config& cfg) {
        return DatabaseConfig{
            .host = cfg.get<std::string>("db.host"),
            .port = cfg.get<int>("db.port"),
            .name = cfg.get<std::string>("db.name"),
            .user = cfg.get<std::string>("db.user"),
            .password = cfg.get<std::string>("db.password")
        };
    }
};

int main() {
    config::Config config;
    auto dbConfig = DatabaseConfig::fromConfig(config);
    
    // Use strongly-typed config object
    connectToDatabase(dbConfig);
}
```

### Pattern 3: Feature Flags

```cpp
class FeatureFlags {
    config::Config& cfg;
    
public:
    explicit FeatureFlags(config::Config& config) : cfg(config) {}
    
    bool isEnabled(const std::string& feature) const {
        auto key = "features." + feature;
        return cfg.getOptional<bool>(key).value_or(false);
    }
};

int main() {
    config::Config config;
    FeatureFlags features(config);
    
    if (features.isEnabled("newUI")) {
        // Enable new UI
    }
    
    if (features.isEnabled("betaFeature")) {
        // Enable beta feature
    }
}
```

### Pattern 4: Environment-Specific Logic

```cpp
enum class Environment { Development, Staging, Production };

Environment getCurrentEnvironment() {
    const char* env = std::getenv("CXX_ENV");
    if (!env) return Environment::Development;
    
    std::string envStr(env);
    if (envStr == "production" || envStr == "prod") {
        return Environment::Production;
    } else if (envStr == "staging" || envStr == "stage") {
        return Environment::Staging;
    }
    return Environment::Development;
}

int main() {
    config::Config config;
    auto env = getCurrentEnvironment();
    
    if (env == Environment::Production) {
        // Production-specific initialization
        enableStrictSSL();
        disableDebugLogging();
    } else if (env == Environment::Development) {
        // Development-specific initialization
        enableDebugLogging();
        enableHotReload();
    }
}
```

### Pattern 5: Singleton Config (Optional)

```cpp
class ConfigManager {
public:
    static ConfigManager& instance() {
        static ConfigManager instance;
        return instance;
    }
    
    config::Config& getConfig() {
        return cfg;
    }
    
    // Delete copy and move constructors
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
private:
    ConfigManager() = default;
    config::Config cfg;
};

// Usage
int main() {
    auto& config = ConfigManager::instance().getConfig();
    auto dbHost = config.get<std::string>("db.host");
}
```

## ⚠️ Error Handling

### Exception Types

Config-cxx throws `std::runtime_error` in the following cases:

#### 1. Missing Key

**Thrown when:** Accessing a non-existent configuration key with `get<T>()`

```cpp
config::Config config;

try {
    auto value = config.get<std::string>("nonexistent.key");
} catch (const std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    // Error: Configuration key 'nonexistent.key' not found
}
```

**Solution:** Use `has()` or `getOptional()`:

```cpp
// Option 1: Check first
if (config.has("optional.key")) {
    auto value = config.get<std::string>("optional.key");
}

// Option 2: Use getOptional
auto value = config.getOptional<std::string>("optional.key");
if (value.has_value()) {
    // Use *value
}
```

#### 2. Type Conversion Error

**Thrown when:** Requested type doesn't match the configuration value type

```cpp
// Config: { "port": "not-a-number" }
try {
    auto port = config.get<int>("port");
} catch (const std::runtime_error& e) {
    std::cerr << "Type conversion error: " << e.what() << std::endl;
}
```

#### 3. File Parsing Error

**Thrown when:** Configuration file has syntax errors

```json
// Invalid JSON
{
  "db": {
    "host": "localhost",
    "port": 3306,  // Missing closing brace
}
```

**Error prevention:**

```cpp
// Validate JSON before deployment
// Use a JSON validator or linter
```

### Safe Configuration Access

```cpp
class SafeConfig {
    config::Config& cfg;
    
public:
    explicit SafeConfig(config::Config& config) : cfg(config) {}
    
    template<typename T>
    std::optional<T> getSafe(const std::string& key) {
        try {
            if (cfg.has(key)) {
                return cfg.get<T>(key);
            }
        } catch (const std::exception& e) {
            std::cerr << "Config error for key '" << key << "': " 
                      << e.what() << std::endl;
        }
        return std::nullopt;
    }
};
```

### Error Handling Best Practices

```cpp
int main() {
    try {
        config::Config config;
        
        // Validate critical configuration early
        std::vector<std::string> criticalKeys = {
            "db.host", "db.port", "api.key"
        };
        
        for (const auto& key : criticalKeys) {
            if (!config.has(key)) {
                std::cerr << "FATAL: Missing critical config: " << key << std::endl;
                return 1;
            }
        }
        
        // Access with proper error handling
        auto dbHost = config.get<std::string>("db.host");
        auto dbPort = config.get<int>("db.port");
        
        // Use optional for non-critical config
        auto logLevel = config.getOptional<std::string>("log.level")
                             .value_or("info");
        
        // Run application
        runApplication(dbHost, dbPort, logLevel);
        
    } catch (const std::exception& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## ✅ Best Practices

### 1. **Always Use Type-Safe Access**

```cpp
// ✅ Good: Type-safe
auto port = config.get<int>("server.port");

// ❌ Avoid: Untyped access
auto portValue = config.get("server.port");
// Returns ConfigValue variant, requires manual type checking
```

### 2. **Validate Configuration at Startup**

```cpp
// ✅ Good: Fail fast with clear errors
void validateConfig(const config::Config& cfg) {
    if (!cfg.has("db.host")) {
        throw std::runtime_error("Missing required config: db.host");
    }
    
    auto port = cfg.get<int>("server.port");
    if (port < 1 || port > 65535) {
        throw std::runtime_error("Invalid port number");
    }
}

int main() {
    config::Config config;
    validateConfig(config);  // Validate before running
    runApplication();
}
```

### 3. **Use Environment-Specific Files**

```text
config/
├── default.json              # ✅ Base configuration (committed)
├── development.json          # ✅ Dev overrides (committed)
├── production.json           # ✅ Production overrides (committed)
├── local-development.json    # ✅ Local dev settings (gitignored)
└── local.json                # ❌ Avoid: Affects all environments
```

### 4. **Never Commit Secrets**

```json
// ❌ Bad: Secrets in version control
{
  "db": {
    "password": "supersecret123"
  }
}

// ✅ Good: Use environment variables
// config/custom-environment-variables.json
{
  "db": {
    "password": "DB_PASSWORD"
  }
}
```

### 5. **Document Your Configuration**

```json
{
  "server": {
    "port": 8080,           // HTTP server port
    "host": "0.0.0.0",      // Bind to all interfaces
    "workers": 4            // Number of worker threads
  },
  "db": {
    "poolSize": 10,         // Connection pool size
    "timeout": 5000         // Query timeout in milliseconds
  }
}
```

### 6. **Use Descriptive Key Names**

```json
// ✅ Good: Clear and descriptive
{
  "database": {
    "connectionTimeout": 5000,
    "maxRetries": 3
  }
}

// ❌ Bad: Unclear abbreviations
{
  "db": {
    "ct": 5000,
    "mr": 3
  }
}
```

### 7. **Provide Sensible Defaults**

```cpp
// ✅ Good: Fallback to reasonable defaults
auto logLevel = config.getOptional<std::string>("log.level")
                     .value_or("info");
auto maxConnections = config.getOptional<int>("pool.max")
                           .value_or(10);

// ❌ Bad: No defaults for optional settings
auto logLevel = config.get<std::string>("log.level");  // Crashes if missing
```

### 8. **Structure Configuration Hierarchically**

```json
// ✅ Good: Organized hierarchy
{
  "database": {
    "primary": {
      "host": "db1.example.com",
      "port": 5432
    },
    "replica": {
      "host": "db2.example.com",
      "port": 5432
    }
  },
  "cache": {
    "redis": {
      "host": "cache.example.com",
      "port": 6379
    }
  }
}

// ❌ Bad: Flat structure
{
  "db_primary_host": "db1.example.com",
  "db_primary_port": 5432,
  "db_replica_host": "db2.example.com",
  "cache_redis_host": "cache.example.com"
}
```

## 🔧 Troubleshooting

### Problem: Configuration Not Loading

**Symptoms:** Default values are used, environment-specific configs ignored

**Solutions:**

```bash
# 1. Check CXX_ENV is set
echo $CXX_ENV

# 2. Check config directory exists
ls -la config/

# 3. Verify file names match CXX_ENV
# If CXX_ENV=production, need config/production.json

# 4. Check file permissions
ls -l config/*.json

# 5. Enable debug logging (if available)
export CXX_DEBUG=1
```

### Problem: "Configuration key not found" Error

**Symptoms:** Runtime error when accessing config

**Solutions:**

```cpp
// 1. Check if key exists
if (!config.has("db.host")) {
    std::cerr << "db.host is not configured" << std::endl;
}

// 2. Use getOptional for optional keys
auto redisHost = config.getOptional<std::string>("redis.host");

// 3. Verify JSON structure
{
  "db": {
    "host": "localhost"  // Access as "db.host", not "host"
  }
}

// 4. Check for typos
config.get<std::string>("databse.host");  // ❌ Typo
config.get<std::string>("database.host"); // ✅ Correct
```

### Problem: Type Conversion Error

**Symptoms:** Runtime error when getting config value

**Solutions:**

```cpp
// Check actual type in config file
{
  "port": "3306"  // ❌ String
}

// Should be:
{
  "port": 3306    // ✅ Number
}

// Or handle as string and convert
auto portStr = config.get<std::string>("port");
int port = std::stoi(portStr);
```

### Problem: Environment Variables Not Overriding

**Symptoms:** Env vars set but config uses file values

**Solutions:**

```bash
# 1. Create custom-environment-variables.json
cat config/custom-environment-variables.json

# 2. Verify environment variable is set
echo $DB_HOST

# 3. Check variable name matches mapping
# config/custom-environment-variables.json:
{
  "db": {
    "host": "DB_HOST"  // Must match env var name exactly
  }
}

# 4. Restart application after setting env vars
export DB_HOST=newhost
./myapp  # Restart required
```

### Problem: Config Directory Not Found

**Symptoms:** "Config directory not found" error

**Solutions:**

```bash
# 1. Check current working directory
pwd

# 2. Config directory should be relative to working directory
ls config/  # Should show config files

# 3. Or set CXX_CONFIG_DIR
export CXX_CONFIG_DIR=/absolute/path/to/config

# 4. Or use relative path
export CXX_CONFIG_DIR=./my-config-dir
```

### Problem: Compilation Errors

**Symptoms:** Linker errors or missing headers

**Solutions:**

```cmake
# 1. Ensure submodules are initialized
git submodule update --init --recursive

# 2. Check CMakeLists.txt
add_subdirectory(externals/config-cxx)
target_link_libraries(your_target config-cxx)

# 3. Verify include path
#include "config-cxx/config.h"  // ✅ Correct
#include "Config.h"             // ❌ Wrong

# 4. Check compiler version
g++ --version  # Should be 13+
clang++ --version  # Should be 16+
```

**For detailed build instructions, see [BUILDING.md](BUILDING.md)**

## 💻 Compilation

### Quick Build

```bash
# Clone and prepare
git clone https://github.com/cieslarmichal/config-cxx.git
cd config-cxx
git submodule update --init --recursive

# Build
mkdir build && cd build
cmake ..
cmake --build .

# Run tests
./config-cxx-UT
```

### Requirements

- **CMake**: 3.14 or newer
- **C++ Standard**: C++20
- **Supported Compilers**:
  - GCC 13+
  - Clang 16+
  - Apple Clang 16+
  - MSVC 143+ (Visual Studio 2022)

### Platform-Specific Instructions

For detailed platform-specific build instructions, including:

- Installing compilers and dependencies
- Platform-specific configuration options
- Troubleshooting common build issues
- Docker builds
- CMake options

**📖 See [BUILDING.md](BUILDING.md) for complete build instructions**

### Quick CMake Integration

```cmake
# Disable tests (reduces dependencies)
set(BUILD_CONFIG_CXX_TESTS OFF)

# Example CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(MyApp)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_CONFIG_CXX_TESTS OFF)

add_subdirectory(externals/config-cxx)

add_executable(myapp src/main.cpp)
target_link_libraries(myapp config-cxx)
```

## 🤝 Examples

### Complete Example Application

Full example available in [`examples/project`](https://github.com/cieslarmichal/config-cxx/tree/main/examples/project)

**Configuration files:**

```text
examples/project/config/
├── default.json
├── development.json
├── production.json
└── custom-environment-variables.json
```

**Source:** [`examples/project/src/Main.cpp`](https://github.com/cieslarmichal/config-cxx/blob/main/examples/project/src/Main.cpp)

```cpp
#include "config-cxx/config.h"
#include <iostream>

int main() {
    config::Config config;
    
    // Database configuration
    const auto dbHost = config.get<std::string>("db.host");
    const auto dbPort = config.get<int>("db.port");
    
    // AWS configuration
    const auto awsAccountId = config.get<std::string>("aws.accountId");
    const auto awsRegion = config.get<std::string>("aws.region");
    
    // Authentication settings
    const auto authExpiresIn = config.get<int>("auth.expiresIn");
    const auto authEnabled = config.get<bool>("auth.enabled");
    const auto authRoles = config.get<std::vector<std::string>>("auth.roles");
    
    std::cout << "Database: " << dbHost << ":" << dbPort << std::endl;
    std::cout << "AWS Region: " << awsRegion << std::endl;
    std::cout << "Auth enabled: " << (authEnabled ? "yes" : "no") << std::endl;
    
    return 0;
}
```

### Real-World Examples

#### Web Server Configuration

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "workerThreads": 4,
    "requestTimeout": 30000,
    "maxRequestSize": 10485760
  },
  "ssl": {
    "enabled": false,
    "certPath": "",
    "keyPath": ""
  },
  "cors": {
    "enabled": true,
    "allowedOrigins": ["http://localhost:3000"]
  }
}
```

#### Microservice Configuration

```json
{
  "service": {
    "name": "user-service",
    "version": "1.0.0",
    "environment": "development"
  },
  "database": {
    "primary": {
      "host": "localhost",
      "port": 5432,
      "name": "users",
      "poolSize": 10
    },
    "replica": {
      "host": "localhost",
      "port": 5433,
      "name": "users"
    }
  },
  "cache": {
    "redis": {
      "host": "localhost",
      "port": 6379,
      "ttl": 3600
    }
  },
  "messaging": {
    "rabbitmq": {
      "host": "localhost",
      "port": 5672,
      "vhost": "/"
    }
  }
}
```

## 🔨 Compiler Support

| Compiler | Minimum Version | Recommended |
| -------- | --------------- | ----------- |
| **GCC** | 13.0 | 13.2+ |
| **Clang** | 16.0 | 17.0+ |
| **Apple Clang** | 16.0 | Latest |
| **MSVC** | 19.34 (VS 2022) | Latest |

**C++ Standard:** C++20 required

## 📦 Dependencies

### Runtime Dependencies

- **nlohmann/json** (included via submodule)
- **yaml-cpp** (included via submodule)
- **pugixml** (included via submodule)

### Test Dependencies

- **Google Test** (optional, disable with `BUILD_CONFIG_CXX_TESTS=OFF`)

All dependencies are included as git submodules - no manual installation required.

## ⚡ Performance

### Thread Safety

Config-cxx is **thread-safe**:

- ✅ Multiple threads can safely call `get()`, `getOptional()`, and `has()` simultaneously
- ✅ Internal mutex protects configuration data access
- ✅ No external synchronization needed
- ✅ Read operations are lock-free after initialization

```cpp
// Safe to use from multiple threads
config::Config config;  // Initialize once

// Thread 1
auto dbHost = config.get<std::string>("db.host");

// Thread 2 (concurrent access is safe)
auto dbPort = config.get<int>("db.port");
```

### Performance Characteristics

- **Initialization**: O(n) where n is the number of configuration keys
- **get() operation**: O(1) average case (hash map lookup)
- **has() operation**: O(1) average case
- **Memory usage**: Minimal - configurations are loaded once at startup

### Best Practices for Performance

```cpp
// ✅ Good: Initialize once, reuse
config::Config config;
auto host = config.get<std::string>("db.host");
auto port = config.get<int>("db.port");

// ❌ Avoid: Creating multiple Config instances
for (int i = 0; i < 1000; i++) {
    config::Config config;  // Reloads files each time!
    auto value = config.get<std::string>("key");
}

// ✅ Good: Cache frequently accessed values
struct AppConfig {
    std::string dbHost;
    int dbPort;
    
    static AppConfig load() {
        config::Config cfg;
        return {cfg.get<std::string>("db.host"), cfg.get<int>("db.port")};
    }
};

auto appConfig = AppConfig::load();  // Load once
// Use appConfig.dbHost and appConfig.dbPort
```

## ❓ FAQ

### General Questions

**Q: Can I use config-cxx in production?**  
A: Yes! Config-cxx is actively used in production environments. It's stable, well-tested (high code coverage), and actively maintained.

**Q: Is config-cxx thread-safe?**  
A: Yes, all operations are thread-safe. Multiple threads can safely access configuration simultaneously.

**Q: What's the performance overhead?**  
A: Minimal. Configuration is loaded once at startup. Each `get()` call is an O(1) hash map lookup.

**Q: Can I reload configuration at runtime?**  
A: Currently, configuration is loaded once at initialization. Runtime reloading is not supported yet (planned for future releases).

### Configuration Files

**Q: Which file format should I use?**  
A: All formats (JSON, YAML, XML) work equally well. Choose based on your preference:

- **JSON**: Most common, good tooling support
- **YAML**: More readable, supports comments
- **XML**: Good for complex hierarchies

**Q: Can I mix file formats?**  
A: No, all configuration files must use the same format. Choose one format for consistency.

**Q: Where should I store secrets?**  
A: Never commit secrets to version control. Use:

1. Environment variables (with `custom-environment-variables.json`)
2. Local files (added to `.gitignore`)
3. Secret management services (AWS Secrets Manager, HashiCorp Vault)

**Q: Can I use both YAML and JSON files?**  
A: No, all config files in a directory must use the same format.

### Environment Variables

**Q: How do I override a nested configuration value with environment variables?**  
A: Use `custom-environment-variables.json` to map:

```json
{
  "db": {
    "host": "DB_HOST",
    "credentials": {
      "password": "DB_PASSWORD"
    }
  }
}
```

Then: `export DB_HOST=prod.example.com DB_PASSWORD=secret`

**Q: What happens if CXX_ENV is not set?**  
A: Defaults to `"development"`. Files loaded: `default.json` + `development.json`

**Q: Can I use custom environment names?**  
A: Yes! Any value works: `export CXX_ENV=staging`, `export CXX_ENV=qa`, etc.

### Integration & Build

**Q: Do I need to install dependencies manually?**  
A: No. All dependencies (nlohmann/json, yaml-cpp, pugixml) are included as git submodules.

**Q: Can I use config-cxx with CMake FetchContent?**  
A: Yes! See [Installation Methods](#-installation-methods) for FetchContent example.

**Q: My project uses C++17. Can I use config-cxx?**  
A: No, config-cxx requires C++20. Consider upgrading or using an older version (if available).

**Q: Does config-cxx work with vcpkg or Conan?**  
A: Not yet, but it's planned. Currently use git submodules or CMake FetchContent.

### Troubleshooting

**Q: Why is my configuration not loading?**  
A: Check:

1. `CXX_ENV` environment variable
2. Config files exist in `./config` or `$CXX_CONFIG_DIR`
3. File names match: `default.json`, `{CXX_ENV}.json`
4. File permissions are readable

**Q: I get "Configuration key not found" error**  
A: The key doesn't exist in any loaded config file. Use `has()` to check first, or `getOptional()` for optional keys.

**Q: Type conversion error when getting a value**  
A: The value type in config doesn't match requested type:

```json
{"port": "3306"}  // String
```

```cpp
config.get<int>("port");  // ❌ Fails - is string, not int
```

Fix: Use correct type in config file: `{"port": 3306}`

**Q: Build fails with "C++20 required" error**  
A: Ensure your compiler supports C++20 and CMakeLists.txt has:

```cmake
set(CMAKE_CXX_STANDARD 20)
```

### Advanced Usage

**Q: Can I programmatically set configuration values?**  
A: No, config-cxx is read-only by design. Configuration should come from files/environment.

**Q: Can I validate configuration at startup?**  
A: Yes! See [Common Patterns - Configuration Validation](#pattern-1-configuration-validation)

**Q: How do I handle optional configuration sections?**  
A: Use `has()` or `getOptional()`:

```cpp
if (config.has("redis.host")) {
    // Redis is configured
    setupRedis(config);
}
```

**Q: Can I use config-cxx in a library?**  
A: Yes, but consider accepting configuration as constructor parameters rather than reading directly, to make your library more flexible.

### Migration

**Q: How do I migrate from node-config?**  
A: Config-cxx is designed to be compatible:

1. Keep the same config directory structure
2. Keep the same file names and formats
3. Use `CXX_ENV` instead of `NODE_ENV`
4. Replace JavaScript code with C++ equivalents

**Q: Can I use the same config files as node-config?**  
A: Yes, if they're in JSON or YAML format. XML is C++ specific.

## 🤝 Contributing

We welcome contributions! 🚀

**How to contribute:**

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Add tests for your changes
5. Run all tests (`./config-cxx-UT`)
6. Format code: `clang-format -i src/**/*.cpp include/**/*.h`
7. Commit your changes (`git commit -m 'Add amazing feature'`)
8. Push to the branch (`git push origin feature/amazing-feature`)
9. Open a Pull Request

**Please read** [CONTRIBUTING.md](CONTRIBUTING.md) **for details**

**Need help?** Join us on [Discord](https://discord.gg/h2ur8H6mK6) 💬

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🌟 Show Your Support

If you find config-cxx helpful, please:

- ⭐ Star the repository
- 🐛 Report bugs and issues
- 💡 Suggest new features
- 📖 Improve documentation
- 🔀 Submit pull requests

## 📞 Contact & Support

- 💬 [Discord Community](https://discord.gg/h2ur8H6mK6)
- 🐛 [Issue Tracker](https://github.com/cieslarmichal/config-cxx/issues)
- 📧 Email: [Create an issue for support](https://github.com/cieslarmichal/config-cxx/issues/new)

---

<div align="center">
  Made with ❤️ by <a href="https://github.com/cieslarmichal">Michał Cieślak</a> and <a href="https://github.com/cieslarmichal/config-cxx/graphs/contributors">contributors</a>
</div>
