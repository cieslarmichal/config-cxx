#pragma once

#include <filesystem>
#include <optional>

namespace config
{
class ConfigDirectoryPathResolver
{
public:
    static std::filesystem::path getConfigDirectoryPath();
};
};
