#include "ConfigProvider.h"

#include "EnvironmentParser.h"

namespace config::environment
{
std::optional<std::string> ConfigProvider::getCxxEnv()
{
    return EnvironmentParser::parseString("CXX_ENV");
}

std::optional<std::string> ConfigProvider::getCxxConfigDir()
{
    return EnvironmentParser::parseString("CXX_CONFIG_DIR");
}
}
