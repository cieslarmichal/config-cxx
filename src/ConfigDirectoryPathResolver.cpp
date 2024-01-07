#include "ConfigDirectoryPathResolver.h"

#include <filesystem>

#include "environment/ConfigProvider.h"
#include "filesystem/ExecutableFinder.h"
#include "filesystem/FileSystemService.h"

namespace config
{
std::filesystem::path ConfigDirectoryPathResolver::getConfigDirectoryPath()
{
    const auto configDirectoryPathFromEnv = getConfigDirectoryPathFromEnv();

    if (configDirectoryPathFromEnv)
    {
        return *configDirectoryPathFromEnv;
    }

    const auto configDirectoryPathFromExecutable = getConfigDirectoryPathFromExecutable();

    if (configDirectoryPathFromExecutable)
    {
        return *configDirectoryPathFromExecutable;
    }

    const auto configDirectoryPathFromCurrentWorkingDirectory = getConfigDirectoryPathFromCurrentWorkingDirectory();

    if (configDirectoryPathFromCurrentWorkingDirectory)
    {
        return *configDirectoryPathFromCurrentWorkingDirectory;
    }

    throw std::runtime_error("Config directory not found.");
}

std::optional<std::filesystem::path> ConfigDirectoryPathResolver::getConfigDirectoryPathFromEnv()
{
    const auto configDirectoryPath = environment::ConfigProvider::getCxxConfigDir();

    if (configDirectoryPath && !configDirectoryPath->empty())
    {
        auto configDirectoryFsPath = std::filesystem::path{*configDirectoryPath};

        const auto envConfigPathExists = filesystem::FileSystemService::exists(configDirectoryFsPath);

        if (!envConfigPathExists)
        {
            throw std::runtime_error("Config directory not found: CXX_CONFIG_DIR=" + *configDirectoryPath);
        }

        return configDirectoryFsPath;
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> ConfigDirectoryPathResolver::getConfigDirectoryPathFromExecutable()
{
    const auto executablePath = filesystem::ExecutableFinder::getExecutablePath();

    std::filesystem::path path(executablePath);

    while (path.has_parent_path())
    {
        auto potentialConfigPath = path / "config";

        if (filesystem::FileSystemService::exists(potentialConfigPath) &&
            filesystem::FileSystemService::isDirectory(potentialConfigPath))
        {
            return potentialConfigPath;
        }

        path = path.parent_path();
    }

    if (path.string().ends_with("config"))
    {
        return path;
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> ConfigDirectoryPathResolver::getConfigDirectoryPathFromCurrentWorkingDirectory()
{
    const auto currentWorkingDirectory = filesystem::FileSystemService::getCurrentWorkingDirectory();

    auto potentialConfigPath = currentWorkingDirectory / "config";

    if (filesystem::FileSystemService::exists(potentialConfigPath) &&
        filesystem::FileSystemService::isDirectory(potentialConfigPath))
    {
        return potentialConfigPath;
    }

    return std::nullopt;
}

}
