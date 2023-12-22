#include "ConfigDirectoryPathResolver.h"

#include <filesystem>

#include "gtest/gtest.h"

#include "EnvironmentSetter.h"

using namespace ::testing;
using namespace config;

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

    const std::string existingConfigDirectoryPath = "../config";
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

    const auto configDirectoryPath = "../config";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, configDirectoryPath);

    const auto path = ConfigDirectoryPathResolver::getConfigDirectoryPath();

    ASSERT_EQ(path, configDirectoryPath);
}

TEST_F(ConfigDirectoryPathResolverTest, givenExistingPathNamedAsNotConfigInConfigDirectory_returnsPath)
{
    const auto configDirectoryEnvName = "CXX_CONFIG_DIR";

    const auto configDirectoryPath = "../cmake";

    EnvironmentSetter::setEnvironmentVariable(configDirectoryEnvName, configDirectoryPath);

    ASSERT_THROW(ConfigDirectoryPathResolver::getConfigDirectoryPath(), std::runtime_error);
}
