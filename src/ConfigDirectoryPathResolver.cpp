#include "ConfigDirectoryPathResolver.h"

#include <filesystem>

#include "environment/EnvironmentParser.h"
#include "filesystem/ExecutableFinder.h"

namespace config
{
std::string ConfigDirectoryPathResolver::getConfigDirectoryPath()
{
    const auto configDirectoryPath = environment::EnvironmentParser::parseString("CXX_CONFIG_DIR");

    if (configDirectoryPath)
    {
        const auto configDirectoryFsPath = std::filesystem::path{*configDirectoryPath};

        const auto envConfigPathExists = exists(std::filesystem::path{configDirectoryFsPath});

        if (!envConfigPathExists)
        {
            throw std::runtime_error("Config directory not found: CXX_CONFIG_DIR=" + *configDirectoryPath);
        }

        if (!configDirectoryPath->ends_with("config"))
        {
            throw std::runtime_error("Config directory must be named `config`: CXX_CONFIG_DIR=" + *configDirectoryPath);
        }

        return *configDirectoryPath;
    }

    const auto executablePath = filesystem::ExecutableFinder::getExecutablePath();

    std::filesystem::path path(executablePath);

    while (path.has_parent_path())
    {
        std::filesystem::path potentialConfigPath = path / "config";

        if (std::filesystem::exists(potentialConfigPath) && std::filesystem::is_directory(potentialConfigPath))
        {
            return potentialConfigPath.string();
        }

        path = path.parent_path();
    }

    throw std::runtime_error("Config directory not found.");
}

}
