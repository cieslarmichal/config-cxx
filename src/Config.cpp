#include "config-cxx/Config.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "ConfigDirectoryPathResolver.h"
#include "environment/EnvironmentParser.h"
#include "filesystem/FileSystemService.h"

namespace config
{
Config::Config() {}

template <typename T>
T Config::get(const std::string& path)
{
    const auto cxxEnv = environment::EnvironmentParser::parseString("CXX_ENV");

    if (!cxxEnv)
    {
        throw std::runtime_error("CXX_ENV environment variable not set.");
    }

    const auto configDirectory = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    const auto configFiles = filesystem::FileSystemService::listFiles(configDirectory);

    const auto defaultConfigFile = configDirectory + (configDirectory.ends_with("/") ? "" : "/") + "/default.json";

    const auto cxxEnvConfigFile =
        configDirectory + (configDirectory.ends_with("/") ? "" : "/") + "/" + *cxxEnv + ".json";

    const auto anyConfigProvided = std::any_of(configFiles.begin(), configFiles.end(), [&](const auto& file)
                                               { return file == defaultConfigFile || file == cxxEnvConfigFile; });

    if (!anyConfigProvided)
    {
        throw std::runtime_error("No config file provided.");
    }

    const auto defaultConfigContent = filesystem::FileSystemService::read(defaultConfigFile);

    return std::any_cast<T>(path);
}

std::any Config::get(const std::string& path)
{
    return path;
}

template int Config::get<int>(const std::string&);
template float Config::get<float>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
}
