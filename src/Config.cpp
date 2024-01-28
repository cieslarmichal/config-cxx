#include "config-cxx/Config.h"

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "ConfigDirectoryPathResolver.h"
#include "environment/ConfigProvider.h"
#include "JsonConfigLoader.h"

namespace config
{
template <typename T>
T Config::get(const std::string& keyPath)
{
    if (!initialized)
    {
        initialize();
    }

    if constexpr (std::is_same_v<T, std::vector<std::string>>)
    {
        return getArray(keyPath);
    }

    const auto value = values[keyPath];

    if (!value.has_value())
    {
        throw std::runtime_error("Config key " + keyPath + " not found.");
    }

    if (value.type() == typeid(std::nullptr_t))
    {
        throw std::runtime_error("Config key " + keyPath + " is of invalid nullptr type.");
    }

    try
    {
        return std::any_cast<T>(value);
    }
    catch (const std::bad_any_cast& e)
    {
        // Log the error details
        std::cerr << "Cannot resolve value for config key '" << keyPath << "': " << e.what() << std::endl;
        throw; // Re-throw the exception
    }
}

std::any Config::get(const std::string& keyPath)
{
    if (!initialized)
    {
        initialize();
    }

    const auto keyOccurrences = std::count_if(
        values.begin(), values.end(), [&](auto& value) { return value.first.find(keyPath) != std::string::npos; });

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
        const std::any& value = pair.second;

        if (key.find(keyPath) == 0)
        {
            try
            {
                result.push_back(std::any_cast<std::string>(value));
            }
            catch (const std::bad_any_cast& e)
            {
                throw std::runtime_error("Config key " + keyPath + " has not valid type.");
            }

            std::cout << std::endl;
        }
    }

    if (result.empty())
    {
        throw std::runtime_error("Config key " + keyPath + " not found.");
    }

    return result;
}

void Config::initialize()
{
    const auto cxxEnv = environment::ConfigProvider::getCxxEnv();

    const auto configDirectory = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    std::cout << "Config directory: " << configDirectory << " loaded." << std::endl;

    const auto defaultConfigFilePath = configDirectory / "default.json";
    const auto cxxEnvConfigFilePath = (configDirectory / cxxEnv).replace_extension(".json");
    const auto customEnvironmentsConfigFilePath = configDirectory / "custom-environment-variables.json";
    const auto localConfigFilePath = configDirectory / "local.json";

    JsonConfigLoader::loadConfigFile(defaultConfigFilePath, values);
    JsonConfigLoader::loadConfigFile(cxxEnvConfigFilePath, values);
    JsonConfigLoader::loadConfigEnvFile(customEnvironmentsConfigFilePath, values);
    JsonConfigLoader::loadConfigFile(localConfigFilePath, values);

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
}
