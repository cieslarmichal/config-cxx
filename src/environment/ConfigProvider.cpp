#include "ConfigProvider.h"

#include "EnvironmentParser.h"

namespace config::environment
{
std::string ConfigProvider::getCxxEnv()
{
    const auto cxxEnv = EnvironmentParser::parseString("CXX_ENV");

    if (!cxxEnv)
    {
        return "development";
    }

    return *cxxEnv;
}

std::optional<std::string> ConfigProvider::getCxxConfigDir()
{
    return EnvironmentParser::parseString("CXX_CONFIG_DIR");
}
}
