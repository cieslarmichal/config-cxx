#pragma once

#include <string>

namespace config::tests
{
class EnvironmentSetter
{
public:
    static void setEnvironmentVariable(const std::string& envName, const std::string& envValue);
};
}
