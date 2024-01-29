#pragma once

#include <any>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace config
{
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
    std::any get(const std::string& keyPath);

    /**
     * @brief Check if a config key exists.
     *
     * @param keyPath The path to config key.
     *
     * @return True if config key is not null value false otherwise.
     *
     * @code
     * Config().has("db.host") // true
     * Config().has("db.port") // true
     * Config().has("db.user") // false
     * @endcode
     */
    bool has(const std::string& keyPath);

private:
    std::vector<std::string> getArray(const std::string& keyPath);
    void initialize();

    bool initialized = false;

    std::unordered_map<std::string, std::any> values;
};
}
