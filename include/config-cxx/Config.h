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

private:
    std::vector<std::string> getArray(const std::string& keyPath);
    void initialize();

    bool initialized = false;

    std::unordered_map<std::string, ConfigValue> values;
    mutable std::mutex lock;
};
}
