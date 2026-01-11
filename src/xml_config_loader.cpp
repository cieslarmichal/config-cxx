#include "xml_config_loader.h"

#include <iostream>
#include <variant>

#include "config_provider.h"
#include "file_system_service.h"
#include "pugixml.hpp"

namespace config
{
namespace
{
void flattenConfig(pugi::xml_node node, const std::string& parentPath,
                   std::unordered_map<std::string, std::vector<std::string>>& flattenedConfig);

void parseConfig(std::unordered_map<std::string, std::vector<std::string>>& flattenedConfig,
                 std::unordered_map<std::string, ConfigValue>& configValues);

BaseTypes parseValue(const std::string& value);

std::string normalizeConfigListKey(const std::string& key);
} // anonymous namespace

void XmlConfigLoader::loadConfigFile(const std::filesystem::path& configFilePath,
                                     std::unordered_map<std::string, ConfigValue>& configValues)
{
    const auto configFileExists = filesystem::FileSystemService::exists(configFilePath);
    if (!configFileExists)
    {
        return;
    }

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(configFilePath.c_str());
    if (!result)
    {
        throw std::runtime_error("Failed to parse XML file: " + configFilePath.string() + " - " +
                                 std::string(result.description()));
    }
    std::unordered_map<std::string, std::vector<std::string>> flattenedConfig;
    flattenConfig(doc.child(configTagName.c_str()), "", flattenedConfig);
    parseConfig(flattenedConfig, configValues);
}

void XmlConfigLoader::loadConfigEnvFile(const std::filesystem::path& configFilePath,
                                        std::unordered_map<std::string, ConfigValue>& configValues)
{
    loadConfigFile(configFilePath, configValues);
    for (auto it = configValues.begin(), end = configValues.end(); it != end; ++it)
    {
        if (std::holds_alternative<std::string>(it->second))
        {
            std::string envKey = std::get<std::string>(it->second);
            const auto envValue = environment::ConfigProvider::parseEnvironmentVariable(envKey);

            if (!envValue || envValue->empty())
            {
                // Environment variable not set, skip silently
                continue;
            }
            configValues[it->first] = *envValue;
        }
    }
}

namespace
{
void flattenConfig(pugi::xml_node node, const std::string& parentPath,
                   std::unordered_map<std::string, std::vector<std::string>>& flattenedConfig)
{
    for (pugi::xml_node child : node.children())
    {
        std::string path = parentPath.empty() ? child.name() : parentPath + "." + child.name();

        if (child.first_child() && (child.first_child().type() == pugi::node_pcdata || child.first_child().empty()))
        {
            flattenedConfig[path].push_back(child.first_child().value());
        }
        else
        {
            flattenConfig(child, path, flattenedConfig);
        }
    }
}

void parseConfig(std::unordered_map<std::string, std::vector<std::string>>& flattenedConfig,
                 std::unordered_map<std::string, ConfigValue>& ConfigValue)
{
    for (auto& [key, values] : flattenedConfig)
    {
        if (values.size() > 1)
        {
            ConfigValue[normalizeConfigListKey(key)] = std::move(values);
        }
        else if (values.size() == 1)
        {
            BaseTypes parsedValue = parseValue(values[0]);
            std::visit([&](auto&& arg) { ConfigValue[key] = arg; }, parsedValue);
        }
    }
}

BaseTypes parseValue(const std::string& value)
{
    if (value.empty())
    {
        return nullptr;
    }
    if (value == "true" || value == "false")
    {
        return "true" == value;
    }
    try
    {
        size_t pos = 0;
        if (value.find('.') != std::string::npos)
        {
            float f_val = std::stof(value, &pos);
            if (pos == value.size())
            {
                return f_val;
            }
            double d_val = std::stod(value, &pos);
            if (pos == value.size())
            {
                return d_val;
            }
        }
        int i_val = std::stoi(value, &pos);
        if (pos == value.size())
        {
            return i_val;
        }
    }
    catch (...)
    {
    }
    return value;
}

std::string normalizeConfigListKey(const std::string& key)
{
    size_t last_dot = key.rfind('.');
    if (last_dot == std::string::npos)
        return key;

    return key.substr(0, last_dot);
}
} // anonymous namespace
} // config namespace
