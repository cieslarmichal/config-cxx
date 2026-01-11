#pragma once

#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace config
{
using ConfigValue = std::variant<std::nullptr_t, bool, int, double, std::string, float, std::vector<std::string>>;

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

using LogCallback = std::function<void(LogLevel, const std::string&)>;

class Config
{
public:
    /**
     * @brief Get a config value by path.
     *
     * @tparam T The target type of config value.
     *
     * @param path The path to config key.
     *
     * @return The value of config key casted to provided type.
     *
     * @code
     * Config().get<std::string>("db.host") // "localhost"
     * Config().get<int>("db.port") // 3306
     * @endcode
     */
    template <typename T>
    T get(const std::string& keyPath);

    /**
     * @brief Get a config value by path if it exists.
     *
     * @tparam T The target type of config value.
     *
     * @param path The path to config key.
     *
     * @return The value of config key casted to provided type or std::nullopt.
     *
     * @code
     * Config().getOptional<std::string>("db.host") // "localhost"
     * Config().getOptional<int>("db.port") // 3306
     * Config().getOptional<std::string>("redis.host") // std::nullopt
     * @endcode
     */
    template <typename T>
    std::optional<T> getOptional(const std::string& keyPath);

    /**
     * @brief Get a config value by path with a default value.
     *
     * @tparam T The target type of config value.
     *
     * @param path The path to config key.
     * @param defaultValue The default value to return if key doesn't exist.
     *
     * @return The value of config key or defaultValue if not found.
     *
     * @code
     * Config().getOrDefault<int>("db.port", 3306) // 3306 if not found
     * Config().getOrDefault<std::string>("redis.host", "localhost")
     * @endcode
     */
    template <typename T>
    T getOrDefault(const std::string& keyPath, T defaultValue);

    /**
     * @brief Get a config value by path.
     *
     * @param path The path to config key.
     *
     * @return The value of config key.
     *
     * @code
     * Config().get("db.host") // "localhost"
     * Config().get("db.port") // 3306
     * @endcode
     */
    ConfigValue get(const std::string& keyPath);

    /**
     * @brief Check if a config key exists.
     *
     * @param keyPath The path to config key.
     *
     * @return True if config key is defined, false otherwise.
     *
     * @code
     * Config().has("db.host") // true
     * Config().has("db.user") // false
     * @endcode
     */
    bool has(const std::string& keyPath);

    /**
     * @brief Set a logging callback for config operations.
     *
     * @param callback Function to call for log messages.
     *
     * @code
     * config.setLogCallback([](LogLevel level, const std::string& msg) {
     *     if (level == LogLevel::Error) {
     *         std::cerr << "[ERROR] " << msg << std::endl;
     *     }
     * });
     * @endcode
     */
    void setLogCallback(LogCallback callback);

private:
    std::vector<std::string> getArray(const std::string& keyPath);
    void initialize();
    void log(LogLevel level, const std::string& message) const;
    std::string getSimilarKeys(const std::string& keyPath) const;
    std::string getTypeString(const ConfigValue& value) const;

    bool initialized = false;
    LogCallback logCallback;

    std::unordered_map<std::string, ConfigValue> values;
    mutable std::mutex lock;
};
}
