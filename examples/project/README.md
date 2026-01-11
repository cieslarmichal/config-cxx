# Config C++ Example Project

This example demonstrates how to use config-cxx in a real application.

## 📁 Project Structure

```text
examples/project/
├── config/
│   ├── default.json                    # Base configuration
│   ├── development.json                # Development overrides
│   └── custom-environment-variables.json  # Env var mappings
├── src/
│   └── Main.cpp                        # Example application
├── CMakeLists.txt                      # Build configuration
└── README.md                           # This file
```

## 🚀 Running the Example

### 1. Install config-cxx

From the repository root:

```bash
mkdir externals
cd externals
git submodule add https://github.com/cieslarmichal/config-cxx.git
git submodule update --init --recursive
```

### 2. Build the example

```bash
cd examples/project
mkdir build && cd build
cmake ..
cmake --build .
```

### 3. Run with different environments

**Development (default):**

```bash
export CXX_ENV=development
./example-app
```

**With environment variables:**

```bash
export CXX_ENV=development
export AWS_ACCOUNT_ID=123456789
export AWS_ACCOUNT_KEY=my-secret-key
./example-app
```

**Custom config directory:**

```bash
export CXX_CONFIG_DIR=$PWD/../config
export CXX_ENV=development
./example-app
```

## 📝 Configuration Files Explained

### default.json

Base configuration with default values:

```json
{
  "db": {
    "host": "localhost",
    "port": 3306
  },
  "aws": {
    "accountId": null,
    "accountKey": null,
    "region": "eu-central-2"
  }
}
```

### development.json

Development-specific overrides:

```json
{
  "db": {
    "port": 2000  // Override port for dev
  },
  "aws": {
    "accountId": "0987654321",
    "accountKey": "321"
  },
  "auth": {  // Additional dev-only settings
    "expiresIn": 7200,
    "enabled": false,
    "roles": ["anonymous", "user"]
  }
}
```

### custom-environment-variables.json

Maps environment variables to config keys:

```json
{
  "aws": {
    "accountId": "AWS_ACCOUNT_ID",
    "accountKey": "AWS_ACCOUNT_KEY"
  }
}
```

When you set `export AWS_ACCOUNT_ID=123`, it overrides `aws.accountId`.

## 🎯 What the Example Demonstrates

1. **Type-safe access** - `get<T>()` with different types
2. **Hierarchical configuration** - nested objects with dot notation
3. **Environment-based configs** - different values per environment
4. **Environment variable overrides** - using custom mappings
5. **Array values** - accessing list configurations
6. **Null values** - handling missing/optional values

## 💡 Try These Experiments

### Experiment 1: Add a Production Config

Create `config/production.json`:

```json
{
  "db": {
    "host": "prod-db.example.com",
    "port": 5432
  },
  "auth": {
    "enabled": true
  }
}
```

Run:

```bash
export CXX_ENV=production
./example-app
```

### Experiment 2: Override with Environment Variables

```bash
export AWS_ACCOUNT_ID=999888777
export AWS_ACCOUNT_KEY=supersecret
./example-app
```

Observe how environment variables take precedence.

### Experiment 3: Add Error Handling

Modify `Main.cpp` to use `has()` and `getOptional()`:

```cpp
if (config.has("redis.host")) {
    auto redisHost = config.get<std::string>("redis.host");
    std::cout << "Redis: " << redisHost << std::endl;
} else {
    std::cout << "Redis not configured" << std::endl;
}

auto logLevel = config.getOptional<std::string>("logging.level")
                     .value_or("info");
std::cout << "Log level: " << logLevel << std::endl;
```

## 📚 Learn More

- [Main README](../../README.md) - Complete documentation
- [BUILDING.md](../../BUILDING.md) - Build instructions
