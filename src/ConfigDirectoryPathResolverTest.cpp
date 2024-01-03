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

        std::filesystem::create_directory(configDirectory);

        std::filesystem::create_directory(defaultDirectory);
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");

        std::filesystem::remove_all(configDirectory);

        std::filesystem::remove_all(defaultDirectory);
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
    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, configDirectory.string());

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, configDirectory);
}

TEST_F(ConfigDirectoryPathResolverTest, givenNoConfigDirPath_andConfigDirExistsInCwd_returnsConfigFromCwd)
{
    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, defaultDirectory);
}
