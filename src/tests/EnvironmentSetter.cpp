#include "EnvironmentSetter.h"

#include <cstdlib>

namespace config::tests
{
void EnvironmentSetter::setEnvironmentVariable(const std::string& envName, const std::string& envValue)
{
#if defined(_WIN32)
    _putenv_s(envName.c_str(), envValue.c_str());
#else
    setenv(envName.c_str(), envValue.c_str(), 1);
#endif
}
}
