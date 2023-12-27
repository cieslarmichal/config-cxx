#include "config-cxx/Config.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <vector>

#include "ConfigDirectoryPathResolver.h"
#include "environment/ConfigProvider.h"
#include "environment/EnvironmentParser.h"
#include "filesystem/FileSystemService.h"
#include "nlohmann/json.hpp"

namespace config
{
namespace
{
const auto normalizeConfigKey = [](const std::string& str)
{
    auto result = str.substr(1);

    std::replace(result.begin(), result.end(), '/', '.');

    return result;
};

const auto normalizeConfigValue = [](const nlohmann::json& jsonObject)
{
    std::any normalizedValue;

    if (jsonObject.is_string())
    {
        normalizedValue = jsonObject.get<std::string>();
    }
    else if (jsonObject.is_number_integer())
    {
        normalizedValue = jsonObject.get<int>();
    }
    else if (jsonObject.is_number_float())
    {
        normalizedValue = jsonObject.get<float>();
    }
    else if (jsonObject.is_boolean())
    {
        normalizedValue = jsonObject.get<bool>();
    }
    else if (jsonObject.is_null())
    {
        normalizedValue = nullptr;
    }
    //    else if (jsonObject.is_array())
    //    {
    //        if (jsonObject.empty())
    //        {
    //            throw std::runtime_error("Empty array config value.");
    //        }
    //
    //        if (jsonObject[0].is_string())
    //        {
    //            normalizedValue = jsonObject.get<std::vector<std::string>>();
    //        }
    //        else
    //        {
    //            throw std::runtime_error("Unsupported config value type.");
    //        }
    //        normalizedValue = jsonObject.get<std::vector<std::string>>();
    //    }
    else
    {
        throw std::runtime_error("Unsupported config value type.");
    }

    return normalizedValue;
};
}

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

    if (!cxxEnv)
    {
        throw std::runtime_error("CXX_ENV environment variable not set.");
    }

    const auto configDirectory = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    std::cerr << "Config directory: " << configDirectory << std::endl;

    std::vector<std::string> configFilesPaths;

    const auto defaultConfigFilePath = configDirectory / "default.json";

    const auto defaultConfigExists = filesystem::FileSystemService::exists(defaultConfigFilePath);

    if (defaultConfigExists)
    {
        configFilesPaths.push_back(defaultConfigFilePath);
    }

    if (cxxEnv)
    {
        const auto cxxEnvConfigFilePath = (configDirectory / *cxxEnv).replace_extension(".json");

        const auto cxxEnvConfigExists = filesystem::FileSystemService::exists(cxxEnvConfigFilePath);

        if (cxxEnvConfigExists)
        {
            configFilesPaths.push_back(cxxEnvConfigFilePath);
        }
    }

    const auto customEnvironmentsConfigFilePath = configDirectory / "custom-environment-variables.json";

    const auto customEnvironmentsConfigExists = filesystem::FileSystemService::exists(customEnvironmentsConfigFilePath);

    if (configFilesPaths.empty() && !customEnvironmentsConfigExists)
    {
        throw std::runtime_error("No config files provided.");
    }

    loadConfigFiles(configFilesPaths);

    if (customEnvironmentsConfigExists)
    {
        loadConfigEnvironmentVariablesFile(customEnvironmentsConfigFilePath);
    }

    initialized = true;
}

void Config::loadConfigFiles(const std::vector<std::string>& configFilesPaths)
{
    for (const auto& configFilePath : configFilesPaths)
    {
        const auto configJson = filesystem::FileSystemService::read(configFilePath);

        const auto config = nlohmann::json::parse(configJson);

        const auto flattenedConfig = config.flatten();

        for (auto it = flattenedConfig.begin(); it != flattenedConfig.end(); ++it)
        {
            const auto normalizedKey = normalizeConfigKey(it.key());

            const auto normalizedValue = normalizeConfigValue(it.value());

            values[normalizedKey] = normalizedValue;
        }
    }
}

void Config::loadConfigEnvironmentVariablesFile(const std::string& configEnvironmentVariablesFilePath)
{
    const auto configEnvironmentVariablesJson = filesystem::FileSystemService::read(configEnvironmentVariablesFilePath);

    const auto configEnvironmentVariables = nlohmann::json::parse(configEnvironmentVariablesJson);

    const auto flattenedConfig = configEnvironmentVariables.flatten();

    for (auto it = flattenedConfig.begin(); it != flattenedConfig.end(); ++it)
    {
        const auto normalizedKey = normalizeConfigKey(it.key());

        const auto envValue = environment::EnvironmentParser::parseString(it.value().get<std::string>());

        if (!envValue)
        {
            throw std::runtime_error("Environment variable " + it.value().get<std::string>() + " not set.");
        }

        values[normalizedKey] = *envValue;
    }
}

template int Config::get<int>(const std::string&);
template float Config::get<float>(const std::string&);
template bool Config::get<bool>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
}
