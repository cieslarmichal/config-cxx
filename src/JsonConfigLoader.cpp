#include "JsonConfigLoader.h"

#include <iostream>

#include "environment/EnvironmentParser.h"
#include "filesystem/FileSystemService.h"
#include "nlohmann/json.hpp"

namespace config
{
namespace
{
std::string normalizeConfigKey(const std::string& str);
std::any normalizeConfigValue(const nlohmann::json& jsonObject);
}

void JsonConfigLoader::loadConfigFile(const std::filesystem::path& configFilePath,
                                      std::unordered_map<std::string, std::any>& configValues)
{
    const auto configFileExists = filesystem::FileSystemService::exists(configFilePath);

    if (!configFileExists)
    {
        return;
    }

    const auto configJson = filesystem::FileSystemService::read(configFilePath);

    const auto config = nlohmann::json::parse(configJson);

    const auto flattenedConfig = config.flatten();

    for (auto it = flattenedConfig.begin(); it != flattenedConfig.end(); ++it)
    {
        const auto normalizedKey = normalizeConfigKey(it.key());

        const auto normalizedValue = normalizeConfigValue(it.value());

        configValues[normalizedKey] = normalizedValue;
    }
}

void JsonConfigLoader::loadConfigEnvFile(const std::filesystem::path& configFilePath,
                                         std::unordered_map<std::string, std::any>& configValues)
{
    const auto configFileExists = filesystem::FileSystemService::exists(configFilePath);

    if (!configFileExists)
    {
        return;
    }

    const auto configEnvironmentVariablesJson = filesystem::FileSystemService::read(configFilePath);

    const auto configEnvironmentVariables = nlohmann::json::parse(configEnvironmentVariablesJson);

    const auto flattenedConfig = configEnvironmentVariables.flatten();

    for (auto it = flattenedConfig.begin(); it != flattenedConfig.end(); ++it)
    {
        const auto normalizedKey = normalizeConfigKey(it.key());

        const auto envValue = environment::EnvironmentParser::parseString(it.value().get<std::string>());

        if (!envValue || envValue->empty())
        {
            throw std::runtime_error("Environment variable " + it.value().get<std::string>() + " not set.");
        }

        configValues[normalizedKey] = *envValue;
    }
}

namespace
{
std::string normalizeConfigKey(const std::string& str)
{
    auto result = str.substr(1);

    std::replace(result.begin(), result.end(), '/', '.');

    return result;
}

std::any normalizeConfigValue(const nlohmann::json& jsonObject)
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
    else
    {
        throw std::runtime_error("Unsupported config value type.");
    }

    return normalizedValue;
}
}

}
