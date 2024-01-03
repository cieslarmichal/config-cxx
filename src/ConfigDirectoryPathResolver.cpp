#include "ConfigDirectoryPathResolver.h"

#include <filesystem>

#include "environment/ConfigProvider.h"
#include "filesystem/FileSystemService.h"

namespace config
{
std::filesystem::path ConfigDirectoryPathResolver::getConfigDirectoryPath()
{
    const auto configDirectoryPath = environment::ConfigProvider::getCxxConfigDir();

    if (configDirectoryPath && !configDirectoryPath->empty())
    {
        const auto configDirectoryFsPath = std::filesystem::path{*configDirectoryPath};

        const auto envConfigPathExists = filesystem::FileSystemService::exists(configDirectoryFsPath);

        if (!envConfigPathExists)
        {
            throw std::runtime_error("Config directory not found: CXX_CONFIG_DIR=" + *configDirectoryPath);
        }

        return configDirectoryFsPath;
    }

    const auto currentWorkingDirectory = filesystem::FileSystemService::getCurrentWorkingDirectory();

    const auto potentialConfigPath = currentWorkingDirectory / "config";

    if (filesystem::FileSystemService::exists(potentialConfigPath) &&
        filesystem::FileSystemService::isDirectory(potentialConfigPath))
    {
        return potentialConfigPath;
    }

    throw std::runtime_error("Config directory not found.");
}

}
