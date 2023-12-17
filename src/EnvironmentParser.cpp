#include "EnvironmentParser.h"

namespace config
{
std::optional<std::string> EnvironmentParser::parseString(const std::string& envName)
{
    const auto envValue = std::getenv(envName.c_str());

    if (!envValue)
    {
        return std::nullopt;
    }

    return envValue;
}
}
