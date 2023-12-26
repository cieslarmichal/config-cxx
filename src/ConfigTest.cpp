#include "config-cxx/Config.h"

#include <fstream>

#include "gtest/gtest.h"

#include "tests/EnvironmentSetter.h"
#include "tests/ProjectRootFinder.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;

namespace
{
const std::filesystem::path projectRootPath = ProjectRootFinder::getProjectRoot();
const std::filesystem::path testDirectoryPath = projectRootPath / "tests" / "configData";
const std::filesystem::path testConfigDirectory = testDirectoryPath / "config";
const std::filesystem::path testEnvConfigFilePath = testConfigDirectory / "test.json";
const std::filesystem::path defaultConfigFilePath = testConfigDirectory / "default.json";

const std::string testJson = R"(
{
    "db": {
        "host": "host",
        "port": 1996
    }
}
)";

const std::string defaultJson = R"(
{
    "db": {
        "host": "localhost",
        "port": 3306
    }
}
)";
}

class ConfigTest : public Test
{
public:
    void SetUp() override
    {
        std::filesystem::remove_all(testDirectoryPath);

        std::filesystem::create_directories(testConfigDirectory);

        std::ofstream testEnvConfigFile{testEnvConfigFilePath};

        testEnvConfigFile << testJson;

        std::ofstream defaultConfigFile{defaultConfigFilePath};

        defaultConfigFile << defaultJson;
    }

    void TearDown() override
    {
        std::filesystem::remove_all(testDirectoryPath);
    }

    Config config;
};

TEST_F(ConfigTest, returnsKeyPath)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");

    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory);

    const std::string keyPath = "db.host";

    const auto configValue = config.get<std::string>(keyPath);

    ASSERT_EQ(configValue, keyPath);
};

// TEST_F(ConfigTest, returnsKeyPathAsAny)
//{
//     const std::string path = "db.host";
//
//     const auto configValue = config.get<std::string>(path);
//
//     ASSERT_EQ(std::any_cast<std::string>(configValue), path);
// }
