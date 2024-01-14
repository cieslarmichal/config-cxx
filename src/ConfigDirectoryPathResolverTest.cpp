#include "ConfigDirectoryPathResolver.h"

#include <filesystem>

#include "gtest/gtest.h"

#include "filesystem/ExecutableFinder.h"
#include "tests/EnvironmentSetter.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;
using namespace config::filesystem;

namespace
{
const auto executablePath = ExecutableFinder::getExecutablePath();
const auto configDirectoryFromExecutable = executablePath.parent_path() / "config";
const auto configDirectory1RelativePath = "customConfig1";
const auto configDirectory1AbsolutePath = executablePath.parent_path() / "customConfig1";
const auto configDirectory2AbsolutePath = executablePath.parent_path() / "customConfig2";
}

class ConfigDirectoryPathResolverTest : public Test
{
public:
    void SetUp() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");

        if (std::filesystem::exists(configDirectory1AbsolutePath))
        {
            std::filesystem::remove_all(configDirectory1AbsolutePath);
        }

        if (std::filesystem::exists(configDirectory2AbsolutePath))
        {
            std::filesystem::remove_all(configDirectory2AbsolutePath);
        }

        if (std::filesystem::exists(configDirectoryFromExecutable))
        {
            std::filesystem::remove_all(configDirectoryFromExecutable);
        }
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");

        if (std::filesystem::exists(configDirectory1AbsolutePath))
        {
            std::filesystem::remove_all(configDirectory1AbsolutePath);
        }

        if (std::filesystem::exists(configDirectory2AbsolutePath))
        {
            std::filesystem::remove_all(configDirectory2AbsolutePath);
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

TEST_F(ConfigDirectoryPathResolverTest, givenConfigDirectoryInExecutableParents_returnsPath)
{
    std::filesystem::create_directory(configDirectoryFromExecutable);

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, configDirectoryFromExecutable);
}

TEST_F(ConfigDirectoryPathResolverTest, givenExistingAbsoluteConfigEnvPath_returnsPath)
{
    std::filesystem::create_directory(configDirectory2AbsolutePath);

    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, configDirectory2AbsolutePath.string());

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, configDirectory2AbsolutePath);
}

TEST_F(ConfigDirectoryPathResolverTest, givenExistingRelativeConfigEnvPath_returnsPath)
{
    std::filesystem::create_directory(configDirectory1AbsolutePath);

    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, configDirectory1RelativePath);

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, configDirectory1AbsolutePath);
}
