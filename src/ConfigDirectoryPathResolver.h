#pragma once

#include <filesystem>

namespace config
{
class ConfigDirectoryPathResolver
{
public:
    static std::filesystem::path getConfigDirectoryPath();
};
};
