#include "ConfigDirectoryPathResolver.h"

#include <filesystem>

#include "gtest/gtest.h"

#include "filesystem/ExecutableFinder.h"
#include "filesystem/FileSystemService.h"
#include "tests/EnvironmentSetter.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;
using namespace config::filesystem;

namespace
{
const auto executablePath = ExecutableFinder::getExecutablePath();
const auto configDirectoryFromExecutable = executablePath.parent_path() / "config";
const auto currentWorkingDirectory = filesystem::FileSystemService::getCurrentWorkingDirectory();
const auto configDirectory = currentWorkingDirectory / "customConfig";
const auto defaultDirectory = currentWorkingDirectory / "config";
}

class ConfigDirectoryPathResolverTest : public Test
{
public:
    void SetUp() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");

        if (std::filesystem::exists(configDirectory))
        {
            std::filesystem::remove_all(configDirectory);
        }

        if (std::filesystem::exists(defaultDirectory))
        {
            std::filesystem::remove_all(defaultDirectory);
        }

        if (std::filesystem::exists(configDirectoryFromExecutable))
        {
            std::filesystem::remove_all(configDirectoryFromExecutable);
        }
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");

        if (std::filesystem::exists(configDirectory))
        {
            std::filesystem::remove_all(configDirectory);
        }

        if (std::filesystem::exists(defaultDirectory))
        {
            std::filesystem::remove_all(defaultDirectory);
        }

        if (std::filesystem::exists(configDirectoryFromExecutable))
        {
            std::filesystem::remove_all(configDirectoryFromExecutable);
        }
    }
};

TEST_F(ConfigDirectoryPathResolverTest, givenNotExistingPathInConfigDirectory_throws)
{
    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    const auto notExistingConfigDirectoryPath = "test";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, notExistingConfigDirectoryPath);

    ASSERT_THROW(ConfigDirectoryPathResolver::getConfigDirectoryPath(), std::runtime_error);
}

TEST_F(ConfigDirectoryPathResolverTest, givenExistingPathNamedConfigInConfigDirectory_returnsPath)
{
    std::filesystem::create_directory(configDirectory);

    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, configDirectory.string());

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, configDirectory);
}

TEST_F(ConfigDirectoryPathResolverTest, givenConfigDirectoryInCwd_returnsPath)
{
    std::filesystem::create_directory(defaultDirectory);

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, defaultDirectory);
}

TEST_F(ConfigDirectoryPathResolverTest, givenConfigDirectoryInExecutableParents_returnsPath)
{
    std::filesystem::create_directory(configDirectoryFromExecutable);

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, configDirectoryFromExecutable);
}
