#include "ConfigProvider.h"

#include "EnvironmentParser.h"

namespace config::environment
{
std::string ConfigProvider::getCxxEnv()
{
    const auto cxxEnv = EnvironmentParser::parseString("CXX_ENV");

    if (!cxxEnv || cxxEnv->empty())
    {
        return "development";
    }

    return *cxxEnv;
}

std::optional<std::string> ConfigProvider::getCxxConfigDir()
{
    const auto configPath = EnvironmentParser::parseString("CXX_CONFIG_DIR");

    if (!configPath || configPath->empty())
    {
        return std::nullopt;
    }

    return *configPath;
}
}
