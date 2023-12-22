#pragma once

#include <string>

namespace config::environment
{
class EnvironmentSetter
{
public:
    static void setEnvironmentVariable(const std::string& envName, const std::string& envValue);
};
}
