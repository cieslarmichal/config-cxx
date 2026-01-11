#include "config_provider.h"

#include <cstdlib>

namespace config::environment
{
std::optional<std::string> ConfigProvider::parseEnvironmentVariable(const std::string& envName)
{
    const auto envValue = std::getenv(envName.c_str());

    if (!envValue)
    {
        return std::nullopt;
    }

    return envValue;
}

std::string ConfigProvider::getCxxEnv()
{
    const auto cxxEnv = parseEnvironmentVariable("CXX_ENV");

    if (!cxxEnv || cxxEnv->empty())
    {
        return "development";
    }

    return *cxxEnv;
}

std::optional<std::string> ConfigProvider::getCxxConfigDir()
{
    const auto configPath = parseEnvironmentVariable("CXX_CONFIG_DIR");

    if (!configPath || configPath->empty())
    {
        return std::nullopt;
    }

    return *configPath;
}
}
