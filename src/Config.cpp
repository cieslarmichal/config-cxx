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

    std::cerr << "CXX_ENV: " << *cxxEnv << std::endl;

    if (!cxxEnv)
    {
        throw std::runtime_error("CXX_ENV environment variable not set.");
    }

    const auto configDirectory = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    std::cerr << "Config directory: " << configDirectory << std::endl;

    const auto configFiles = filesystem::FileSystemService::listFiles(configDirectory);

    const auto defaultConfigFilePath = configDirectory + (configDirectory.ends_with("/") ? "" : "/") + "default.json";

    const auto cxxEnvConfigFilePath = configDirectory + (configDirectory.ends_with("/") ? "" : "/") + *cxxEnv + ".json";

    const auto customEnvironmentsConfigFilePath =
        configDirectory + (configDirectory.ends_with("/") ? "" : "/") + "custom-environment-variables.json";

    const auto anyConfigProvided = std::any_of(configFiles.begin(), configFiles.end(),
                                               [&](const auto& file) {
                                                   return file == defaultConfigFilePath ||
                                                          file == cxxEnvConfigFilePath ||
                                                          file == customEnvironmentsConfigFilePath;
                                               });

    if (!anyConfigProvided)
    {
        throw std::runtime_error("No config file provided.");
    }

    std::cerr << "Default config file: " << defaultConfigFilePath << std::endl;

    const auto defaultConfigContent = filesystem::FileSystemService::read(defaultConfigFilePath);

    const auto flattenedDefaultConfig = nlohmann::json::parse(defaultConfigContent).flatten();

    const auto normalizeConfigKey = [](const std::string& str)
    {
        auto result = str.substr(1);

        std::replace(result.begin(), result.end(), '/', '.');

        return result;
    };

    for (const auto& [key, value] : flattenedDefaultConfig.items())
    {
        const auto normalizedKey = normalizeConfigKey(key);

        values[normalizedKey] = value;
    }

    initialized = true;
}

template int Config::get<int>(const std::string&);
template float Config::get<float>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
}
