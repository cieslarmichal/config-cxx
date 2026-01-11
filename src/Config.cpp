#include "config-cxx/config.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <variant>

#include "config_directory_path_resolver.h"
#include "config_value.h"
#include "config_provider.h"
#include "json_config_loader.h"
#include "xml_config_loader.h"
#include "yaml_config_loader.h"

namespace config
{

template <typename T>
T Config::get(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
        initialized = true;
    }

    if constexpr (std::is_same_v<T, std::vector<std::string>>)
    {
        return getArray(keyPath);
    }

    auto it = values.find(keyPath);
    if (it == values.end())
    {
        std::string errorMsg = "Configuration key '" + keyPath + "' not found.";
        std::string similar = getSimilarKeys(keyPath);
        if (!similar.empty())
        {
            errorMsg += " Did you mean: " + similar + "?";
        }
        log(LogLevel::Error, errorMsg);
        throw std::runtime_error(errorMsg);
    }

    const auto& value = it->second;

    if (value.index() == 0)
    {
        std::string errorMsg = "Configuration key '" + keyPath + "' has null value.";
        log(LogLevel::Error, errorMsg);
        throw std::runtime_error(errorMsg);
    }

    std::optional<T> castedValue = config::cast<T>(value);

    if (castedValue)
    {
        return castedValue.value();
    }
    else
    {
        std::string errorMsg = "Configuration key '" + keyPath + "' has wrong type. Expected: " + 
                              typeid(T).name() + ", Actual: " + getTypeString(value);
        log(LogLevel::Error, errorMsg);
        throw std::runtime_error(errorMsg);
    }
}

template <typename T>
std::optional<T> Config::getOptional(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
        initialized = true;
    }

    if constexpr (std::is_same_v<T, std::vector<std::string>>)
    {
        return getArray(keyPath);
    }

    auto it = values.find(keyPath);
    if (it == values.end())
    {
        return std::nullopt;
    }

    const auto& value = it->second;

    if (value.index() == 0)
    {
        return std::nullopt;
    }

    std::optional<T> castedValue = config::cast<T>(value);

    if (castedValue)
    {
        return castedValue.value();
    }
    else
    {
        std::string errorMsg = "Configuration key '" + keyPath + "' has wrong type. Expected: " +
                              typeid(T).name() + ", Actual: " + getTypeString(value);
        log(LogLevel::Error, errorMsg);
        throw std::runtime_error(errorMsg);
    }
}

template <typename T>
T Config::getOrDefault(const std::string& keyPath, T defaultValue)
{
    auto optValue = getOptional<T>(keyPath);
    return optValue.value_or(defaultValue);
}

ConfigValue Config::get(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
        initialized = true;
    }

    const auto keyOccurrences = std::count_if(values.begin(), values.end(), [&](auto& value)
                                              { return value.first.find(keyPath) != std::string::npos; });

    if (keyOccurrences == 0)
    {
        std::string errorMsg = "Configuration key '" + keyPath + "' not found.";
        std::string similar = getSimilarKeys(keyPath);
        if (!similar.empty())
        {
            errorMsg += " Did you mean: " + similar + "?";
        }
        log(LogLevel::Error, errorMsg);
        throw std::runtime_error(errorMsg);
    }

    if (keyOccurrences > 1)
    {
        return getArray(keyPath);
    }

    return values[keyPath];
}

std::vector<std::string> Config::getArray(const std::string& keyPath)
{
    std::vector<std::string> result;

    for (const auto& pair : values)
    {
        const std::string& key = pair.first;
        const ConfigValue& value = pair.second;

        if (key.find(keyPath) == 0)
        {
            std::optional<std::string> castedValue = config::cast<std::string>(value);
            if (castedValue)
            {
                result.push_back(castedValue.value());
            }
            else
            {
                std::string errorMsg = "Configuration key '" + keyPath + "' array element has wrong type.";
                log(LogLevel::Error, errorMsg);
                throw std::runtime_error(errorMsg);
            }
        }
    }

    if (result.empty())
    {
        std::string errorMsg = "Configuration key '" + keyPath + "' not found.";
        std::string similar = getSimilarKeys(keyPath);
        if (!similar.empty())
        {
            errorMsg += " Did you mean: " + similar + "?";
        }
        log(LogLevel::Error, errorMsg);
        throw std::runtime_error(errorMsg);
    }

    return result;
}

bool Config::has(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
        initialized = true;
    }

    return values.find(keyPath) != values.end();
}

void Config::initialize()
{
    // Find if no config warning is enabled or disabled
    const auto suppressWarning = std::getenv("SUPPRESS_NO_CONFIG_WARNING");

    // Get the path to the configuration directory
    const auto configDirectory = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    // Check if the configuration directory is empty
    bool isEmpty = true;
    for (const auto& entry : std::filesystem::directory_iterator(configDirectory))
    {
        if (entry.is_regular_file())
        {
            isEmpty = false;
            break;
        }
    }

    // If the configuration directory is empty, log a message and return
    if (isEmpty)
    {
        if (suppressWarning == nullptr)
        {
            log(LogLevel::Warning, "No configurations found in configuration directory.");
        }
        return;
    }
    const auto cxxEnv = environment::ConfigProvider::getCxxEnv();

    log(LogLevel::Info, "Config directory: " + configDirectory.string() + " loaded.");

    const auto strictMode = std::getenv("CXX_CONFIG_STRICT_MODE");
    bool foundCxxEnvFile = false;

    if (strictMode != nullptr && (cxxEnv == "local" || cxxEnv == "default"))
    {
        throw std::runtime_error("ERROR: CXX_ENV must not be 'default' or 'local' under strict mode");
    }
    std::vector<std::string> order = {"default", cxxEnv, "local", "local-" + cxxEnv, "custom-environment-variables"};

    auto customFileOrder = [order](const std::filesystem::path& path1, const std::filesystem::path& path2)
    {
        auto filename1 = path1.stem().string();
        auto filename2 = path2.stem().string();

        auto it1 = std::find(order.begin(), order.end(), filename1);
        auto it2 = std::find(order.begin(), order.end(), filename2);

        if (it1 == order.end() && it2 == order.end())
        {
            // If both filenames are not in the order list, order them alphabetically
            return path1 < path2;
        }
        else if (it1 == order.end())
        {
            // If only path1 is not in the order list, it comes after path2
            return false;
        }
        else if (it2 == order.end())
        {
            // If only path2 is not in the order list, it comes after path1
            return true;
        }
        else
        {
            // If both filenames are in the order list, order them based on their position in the list
            return std::distance(order.begin(), it1) < std::distance(order.begin(), it2);
        }
    };

    std::vector<std::filesystem::path> filePaths;
    for (const auto& entry : std::filesystem::directory_iterator(configDirectory))
    {
        if (entry.is_regular_file())
        {
            filePaths.push_back(entry.path());
        }
    }

    // Sort file paths according to custom order
    std::sort(filePaths.begin(), filePaths.end(), customFileOrder);

    for (const auto& filePath : filePaths)
    {
        if (filePath.extension() == ".json")
        {
            if (filePath.string().find("environment") != std::string::npos)
            {
                JsonConfigLoader::loadConfigEnvFile(filePath, values);
                if (filePath.stem().string() == cxxEnv)
                {
                    foundCxxEnvFile = true;
                }
            }
            else
            {
                JsonConfigLoader::loadConfigFile(filePath, values);
            }
        }
        else if (filePath.extension() == ".yaml" || filePath.extension() == ".yml")
        {
            if (filePath.string().find("environment") != std::string::npos)
            {
                YamlConfigLoader::loadConfigEnvFile(filePath, values);
                if (filePath.stem().string() == cxxEnv)
                {
                    foundCxxEnvFile = true;
                }
            }
            else
            {
                YamlConfigLoader::loadConfigFile(filePath, values);
            }
        }
        else if (filePath.extension() == ".xml")
        {
            if (filePath.string().find("environment") != std::string::npos)
            {
                XmlConfigLoader::loadConfigEnvFile(filePath, values);
                if (filePath.stem().string() == cxxEnv)
                {
                    foundCxxEnvFile = true;
                }
            }
            else
            {
                XmlConfigLoader::loadConfigFile(filePath, values);
            }
        }
    }

    if (values.empty())
    {
        throw std::runtime_error("Config values are empty.");
    }

    if (!foundCxxEnvFile && !cxxEnv.empty() && strictMode != nullptr)
    {
        throw std::runtime_error("ERROR: No configuration file matching CXX_ENV");
    }
}

void Config::setLogCallback(LogCallback callback)
{
    std::lock_guard<std::mutex> lockGuard(lock);
    logCallback = std::move(callback);
}

void Config::log(LogLevel level, const std::string& message) const
{
    if (logCallback)
    {
        logCallback(level, message);
    }
    else
    {
        // Default logging to stderr for errors and warnings
        switch (level)
        {
            case LogLevel::Error:
                std::cerr << "[CONFIG ERROR] " << message << std::endl;
                break;
            case LogLevel::Warning:
                std::cerr << "[CONFIG WARNING] " << message << std::endl;
                break;
            case LogLevel::Info:
            case LogLevel::Debug:
                // Silent by default for info/debug to avoid clutter
                break;
        }
    }
}

std::string Config::getSimilarKeys(const std::string& keyPath) const
{
    std::vector<std::pair<std::string, int>> similarities;
    
    for (const auto& [key, _] : values)
    {
        // Simple Levenshtein-like similarity check
        int distance = 0;
        size_t minLen = std::min(key.length(), keyPath.length());
        
        for (size_t i = 0; i < minLen; ++i)
        {
            if (key[i] != keyPath[i]) distance++;
        }
        distance += std::abs(static_cast<int>(key.length()) - static_cast<int>(keyPath.length()));
        
        if (distance < 5)  // Only suggest if relatively similar
        {
            similarities.push_back({key, distance});
        }
    }
    
    if (similarities.empty())
    {
        return "";
    }
    
    // Sort by similarity (lowest distance first)
    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    // Return top 3 suggestions
    std::string result;
    for (size_t i = 0; i < std::min(size_t(3), similarities.size()); ++i)
    {
        if (i > 0) result += ", ";
        result += "'" + similarities[i].first + "'";
    }
    
    return result;
}

std::string Config::getTypeString(const ConfigValue& value) const
{
    switch (value.index())
    {
        case 0: return "null";
        case 1: return "bool";
        case 2: return "int";
        case 3: return "double";
        case 4: return "string";
        case 5: return "float";
        case 6: return "vector<string>";
        default: return "unknown";
    }
}

template int Config::get<int>(const std::string&);
template bool Config::get<bool>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
template float Config::get<float>(const std::string&);

template std::optional<int> Config::getOptional<int>(const std::string&);
template std::optional<bool> Config::getOptional<bool>(const std::string&);
template std::optional<std::string> Config::getOptional<std::string>(const std::string&);
template std::optional<std::vector<std::string>> Config::getOptional<std::vector<std::string>>(const std::string&);
template std::optional<float> Config::getOptional<float>(const std::string&);

template int Config::getOrDefault<int>(const std::string&, int);
template bool Config::getOrDefault<bool>(const std::string&, bool);
template std::string Config::getOrDefault<std::string>(const std::string&, std::string);
template float Config::getOrDefault<float>(const std::string&, float);
}
