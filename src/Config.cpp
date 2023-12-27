#include "config-cxx/Config.h"

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

    const auto value = values[keyPath];

    if (!value.has_value())
    {
        throw std::runtime_error("Config key " + keyPath + " not found.");
    }

    if constexpr (std::is_same_v<T, std::string>)
    {
        try
        {
            return std::to_string(std::any_cast<int>(value));
        }
        catch (const std::bad_any_cast& e)
        {
        }
    }

    return std::any_cast<T>(value);
}

std::any Config::get(const std::string& keyPath)
{
    if (!initialized)
    {
        initialize();
    }

    const auto value = values[keyPath];

    if (!value.has_value())
    {
        throw std::runtime_error("Config key " + keyPath + " not found.");
    }

    return value;
}

void Config::initialize()
{
    const auto cxxEnv = environment::ConfigProvider::getCxxEnv();

    const auto configDirectory = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    std::cout << "Config directory: " << configDirectory << std::endl;
    std::cout << "Env: " << cxxEnv << std::endl;

    const auto defaultConfigFilePath = configDirectory / "default.json";
    const auto cxxEnvConfigFilePath = (configDirectory / cxxEnv).replace_extension(".json");
    const auto customEnvironmentsConfigFilePath = configDirectory / "custom-environment-variables.json";

    JsonConfigLoader::loadConfigFile(defaultConfigFilePath, values);
    JsonConfigLoader::loadConfigFile(cxxEnvConfigFilePath, values);
    JsonConfigLoader::loadConfigEnvFile(customEnvironmentsConfigFilePath, values);

    initialized = true;
}

template int Config::get<int>(const std::string&);
template float Config::get<float>(const std::string&);
template bool Config::get<bool>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
}
