#include "JsonConfigLoader.h"

#include "environment/EnvironmentParser.h"
#include "filesystem/FileSystemService.h"
#include "nlohmann/json.hpp"

namespace config
{
namespace
{
std::string normalizeConfigKey(const std::string& str);
std::string normalizeConfigValue(const nlohmann::json& jsonObject);
}

JsonConfigLoader::JsonConfigLoader(std::unordered_map<std::string, std::any>& valuesRef) : values{valuesRef} {}

void JsonConfigLoader::loadConfigFile(const std::string& configFilePath)
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

        values[normalizedKey] = normalizedValue;
    }
}

void JsonConfigLoader::loadConfigEnvFile(const std::string& configFilePath)
{
    const auto configEnvironmentVariablesJson = filesystem::FileSystemService::read(configFilePath);

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

namespace
{
std::string normalizeConfigKey(const std::string& str)
{
    auto result = str.substr(1);

    std::replace(result.begin(), result.end(), '/', '.');

    return result;
}

std::string normalizeConfigValue(const nlohmann::json& jsonObject)
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
    else if (jsonObject.is_array())
    {
        if (jsonObject.empty())
        {
            throw std::runtime_error("Empty array config value.");
        }

        if (jsonObject[0].is_string())
        {
            normalizedValue = jsonObject.get<std::vector<std::string>>();
        }
        else
        {
            throw std::runtime_error("Unsupported config value type.");
        }
        normalizedValue = jsonObject.get<std::vector<std::string>>();
    }
    else
    {
        throw std::runtime_error("Unsupported config value type.");
    }

    return normalizedValue;
}
}

}
