#include "ConfigDirectoryPathResolver.h"

#include <filesystem>

#include "gtest/gtest.h"

#include "tests/EnvironmentSetter.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;

class ConfigDirectoryPathResolverTest : public Test
{
public:
    void SetUp() override
    {
        if (!std::filesystem::exists(existingConfigDirectoryPath))
        {
            std::filesystem::create_directory(existingConfigDirectoryPath);
        }
    }

    void TearDown() override
    {
        if (std::filesystem::exists(existingConfigDirectoryPath))
        {
            std::filesystem::remove_all(existingConfigDirectoryPath);
        }
    }

    std::filesystem::path existingConfigDirectoryPath = "../config";
};

TEST_F(ConfigDirectoryPathResolverTest, givenNotExistingPathInConfigDirectory_throws)
{
    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    const auto configDirectoryPath = "test";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, configDirectoryPath);

    ASSERT_THROW(ConfigDirectoryPathResolver::getConfigDirectoryPath(), std::runtime_error);
}

TEST_F(ConfigDirectoryPathResolverTest, givenExistingPathNamedConfigInConfigDirectory_returnsPath)
{
    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, existingConfigDirectoryPath.string());

    const auto configDirectoryPath = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(configDirectoryPath, existingConfigDirectoryPath);
}

TEST_F(ConfigDirectoryPathResolverTest, givenExistingPathNamedAsNotConfigInConfigDirectory_returnsPath)
{
    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    const auto invalidConfigDirectoryPath = "../cmake";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, invalidConfigDirectoryPath);

    ASSERT_THROW(ConfigDirectoryPathResolver::getConfigDirectoryPath(), std::runtime_error);
}
