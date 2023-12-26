#include "config-cxx/Config.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "ConfigDirectoryPathResolver.h"
#include "environment/ConfigProvider.h"
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
}

template <typename T>
T Config::get(const std::string& path)
{
    if (!initialized)
    {
        initialize();
    }

    return std::any_cast<T>(path);
}

std::any Config::get(const std::string& path)
{
    if (!initialized)
    {
        initialize();
    }

    return path;
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

    if (customEnvironmentsConfigExists)
    {
        configFilesPaths.push_back(customEnvironmentsConfigFilePath);
    }

    if (configFilesPaths.empty())
    {
        throw std::runtime_error("No config files provided.");
    }

    loadConfigFiles(configFilesPaths);

    loadConfigEnvironmentVariables();

    initialized = true;
}

void Config::loadConfigFiles(const std::vector<std::string>& configFilesPaths)
{
    for (const auto& configFilePath : configFilesPaths)
    {
        const auto configJson = filesystem::FileSystemService::read(configFilePath);

        const auto config = nlohmann::json::parse(configJson);

        for (const auto& [key, value] : config.items())
        {
            const auto normalizedKey = normalizeConfigKey(key);

            values[normalizedKey] = value;
        }
    }
}

void Config::loadConfigEnvironmentVariables() {}

template int Config::get<int>(const std::string&);
template float Config::get<float>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
}
