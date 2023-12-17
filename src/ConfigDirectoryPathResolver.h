#pragma once

#include <string>

namespace config
{
class ConfigDirectoryPathResolver
{
public:
    static std::string getConfigDirectoryPath();

private:
    static std::string getExecutablePath();
};
};
