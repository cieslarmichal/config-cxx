#include "config_directory_path_resolver.h"

#include <filesystem>
#include <optional>

#include "config_provider.h"
#include "file_system_service.h"

namespace config
{
std::filesystem::path ConfigDirectoryPathResolver::getConfigDirectoryPath()
{
    std::filesystem::path configDirectoryPath;

    const auto configDirectoryEnvPath = environment::ConfigProvider::getCxxConfigDir();

    if (configDirectoryEnvPath && !configDirectoryEnvPath->empty())
    {
        auto configDirectoryFsPath = std::filesystem::path{*configDirectoryEnvPath};

        if (!filesystem::FileSystemService::isRelative(configDirectoryFsPath))
        {
            if (!filesystem::FileSystemService::exists(configDirectoryFsPath))
            {
                throw std::runtime_error("Config directory not found: CXX_CONFIG_DIR=" + *configDirectoryEnvPath);
            }

            return configDirectoryFsPath;
        }
    }

    const auto executablePath = filesystem::FileSystemService::getExecutablePath();

    if (executablePath.empty())
    {
        throw std::runtime_error("Executable path not found.");
    }

    std::filesystem::path path(executablePath);

    const auto pathSuffix = configDirectoryEnvPath && !configDirectoryEnvPath->empty() ?
                                std::filesystem::path{*configDirectoryEnvPath}.lexically_normal().string() :
                                "config";

    const auto systemRootPath = filesystem::FileSystemService::getSystemRootPath();

    while (path.has_parent_path() && !equivalent(path, systemRootPath))
    {
        auto potentialConfigPath = path / pathSuffix;

        if (filesystem::FileSystemService::exists(potentialConfigPath) &&
            filesystem::FileSystemService::isDirectory(potentialConfigPath))
        {
            return potentialConfigPath;
        }

        path = path.parent_path();
    }

    if (path.string().ends_with(pathSuffix))
    {
        return path;
    }

    throw std::runtime_error("Config directory not found.");
}

}
