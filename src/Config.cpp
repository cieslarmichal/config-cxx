#include "config-cxx/Config.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "ConfigDirectoryPathResolver.h"
#include "environment/EnvironmentParser.h"
#include "filesystem/FileSystemService.h"
#include "nlohmann/json.hpp"

namespace config
{

template <typename T>
T Config::get(const std::string& path)
{
    if (!config)
    {
        initialize();
    }

    return std::any_cast<T>(path);
}

std::any Config::get(const std::string& path)
{
    if (!config)
    {
        initialize();
    }

    return path;
}

void Config::initialize()
{
    const auto cxxEnv = environment::EnvironmentParser::parseString("CXX_ENV");

    std::cerr << "CXX_ENV: " << *cxxEnv << std::endl;

    if (!cxxEnv)
    {
        throw std::runtime_error("CXX_ENV environment variable not set.");
    }

    const auto configDirectory = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    std::cerr << "Config directory: " << configDirectory << std::endl;

    const auto configFiles = filesystem::FileSystemService::listFiles(configDirectory);

    const auto defaultConfigFile = configDirectory + (configDirectory.ends_with("/") ? "" : "/") + "default.json";

    const auto cxxEnvConfigFile = configDirectory + (configDirectory.ends_with("/") ? "" : "/") + *cxxEnv + ".json";

    const auto anyConfigProvided = std::any_of(configFiles.begin(), configFiles.end(), [&](const auto& file)
                                               { return file == defaultConfigFile || file == cxxEnvConfigFile; });

    if (!anyConfigProvided)
    {
        throw std::runtime_error("No config file provided.");
    }

    std::cerr << "Default config file: " << defaultConfigFile << std::endl;

    const auto defaultConfigContent = filesystem::FileSystemService::read(defaultConfigFile);

    std::cerr << "Default config content: " << defaultConfigContent << std::endl;

    config = nlohmann::json::parse(defaultConfigContent);
}

template int Config::get<int>(const std::string&);
template float Config::get<float>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
}
