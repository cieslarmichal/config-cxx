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
const auto projectRootPath = ExecutableFinder::getExecutablePath();
const auto configDirectory = projectRootPath.parent_path() / "config";
}

class ConfigDirectoryPathResolverTest : public Test
{
public:
    void SetUp() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");

        std::filesystem::create_directory(configDirectory);
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");
        
        std::filesystem::remove_all(configDirectory);
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
