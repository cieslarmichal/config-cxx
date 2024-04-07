#include "config-cxx/Config.h"

#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <variant>

#include "ConfigDirectoryPathResolver.h"
#include "ConfigValue.h"
#include "environment/ConfigProvider.h"
#include "JsonConfigLoader.h"
#include "YamlConfigLoader.h"
#include "XmlConfigLoader.h"

namespace config
{

template <typename T>
T Config::get(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
    }

    if constexpr (std::is_same_v<T, std::vector<std::string>>)
    {
        return getArray(keyPath);
    }

    auto it = values.find(keyPath);
    if (it == values.end())
    {
        throw std::runtime_error("Config key " + keyPath + " not found.");
    }

    const auto& value = it->second;

    if (value.index() == 0)
    {
        throw std::runtime_error("Config key " + keyPath + " has value of nullptr.");
    }

    std::optional<T> castedValue = config::cast<T>(value);

    if (castedValue)
    {
        return castedValue.value();
    }

    else
    {
        throw std::runtime_error("Config key " + keyPath + " has value of wrong type.");
    }
}

template <typename T>
std::optional<T> Config::getOptional(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
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
        throw std::runtime_error("Config key " + keyPath + " has value of wrong type.");
    }
}

ConfigValue Config::get(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
    }

    const auto keyOccurrences = std::count_if(values.begin(), values.end(), [&](auto& value)
                                              { return value.first.find(keyPath) != std::string::npos; });

    if (keyOccurrences == 0)
    {
        throw std::runtime_error("Config key " + keyPath + " not found.");
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
                throw std::runtime_error("Config key " + keyPath + " has value of wrong type.");
            }
        }
    }

    if (result.empty())
    {
        throw std::runtime_error("Config key " + keyPath + " not found.");
    }

    return result;
}

bool Config::has(const std::string& keyPath)
{
    std::lock_guard<std::mutex> lockGuard(lock);

    if (!initialized)
    {
        initialize();
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
    for (const auto& entry : std::filesystem::directory_iterator(configDirectory)) {
        if (entry.is_regular_file()) {
            isEmpty = false;
            break;
        }
    }

    // If the configuration directory is empty, log a message and return
    if (isEmpty && suppressWarning != nullptr) {
        std::cout << "WARNING: No configurations found in configuration directory." << std::endl;
        return;
    }
    const auto cxxEnv = environment::ConfigProvider::getCxxEnv();

    std::cout << "Config directory: " << configDirectory << " loaded." << std::endl;

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

    initialized = true;
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
}
