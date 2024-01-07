#pragma once

#include <filesystem>
#include <optional>

namespace config
{
class ConfigDirectoryPathResolver
{
public:
    static std::filesystem::path getConfigDirectoryPath();

private:
    static std::optional<std::filesystem::path> getConfigDirectoryPathFromEnv();
    static std::optional<std::filesystem::path> getConfigDirectoryPathFromExecutable();
    static std::optional<std::filesystem::path> getConfigDirectoryPathFromCurrentWorkingDirectory();
};
};
