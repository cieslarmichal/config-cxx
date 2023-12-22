#pragma once

#include <optional>
#include <string>

namespace config::environment
{
class EnvironmentParser
{
public:
    static std::optional<std::string> parseString(const std::string& envName);
};
}
