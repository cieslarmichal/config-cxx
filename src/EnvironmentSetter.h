#pragma once

#include <string>

namespace config
{
class EnvironmentSetter
{
public:
    static void setEnvironmentVariable(const std::string& envName, const std::string& envValue);
};
}
