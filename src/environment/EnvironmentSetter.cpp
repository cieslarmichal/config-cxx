#include "EnvironmentSetter.h"

#include <cstdlib>
#include <stdexcept>

namespace config::environment
{
void EnvironmentSetter::setEnvironmentVariable(const std::string& envName, const std::string& envValue)
{
#if defined(_WIN32)
    if (_putenv_s(envName.c_str(), envValue.c_str()) != 0)
    {
        throw std::runtime_error("Failed to set environment variable: " + envName + " with value: " + envValue);
    }
#else
    if (setenv(envName.c_str(), envValue.c_str(), 1) != 0)
    {
        throw std::runtime_error("Failed to set environment variable: " + envName + " with value: " + envValue);
    }
#endif
}
}
