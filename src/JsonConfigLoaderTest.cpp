#include "JsonConfigLoader.h"

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"

#include "filesystem/ExecutableFinder.h"
#include "tests/EnvironmentSetter.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;
using namespace config::filesystem;

namespace
{
using ConfigValue = std::variant<std::nullptr_t, bool, int, double, std::string, float, std::vector<std::string>>;

const auto projectRootPath = ExecutableFinder::getExecutablePath();
const auto testConfigDirectory = projectRootPath.parent_path() / "testConfig";
const auto testEnvConfigFilePath = testConfigDirectory / "test.json";

const std::string testJson = R"(
{
    "db": {
        "port": 1996
    },
    "aws": {
        "accountId": "123456789012",
        "accountKey": "123",
        "region": "eu-central-1"
    },
    "auth": {
        "expiresIn": 3600,
        "enabled": true,
        "roles": [
            "admin",
            "user"
        ]
    }
}
)";
}

class JsonConfigLoaderTest : public Test
{
    public:
    void SetUp() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

        std::filesystem::remove_all(testConfigDirectory);

        std::filesystem::create_directory(testConfigDirectory);

        std::ofstream testEnvConfigFile{testEnvConfigFilePath};

        testEnvConfigFile << testJson;
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

        std::filesystem::remove_all(testConfigDirectory);
    }
};

TEST_F(JsonConfigLoaderTest, loadConfigFile) {
    std::unordered_map<std::string, ConfigValue> expectedValues = {
        {"db.port", 1996},
        {"aws.accountId", "123"},
        {"aws.accountKey", "123"},
        {"aws.region", "eu-central-1"},
        {"auth.expiresIn", 3600},
        {"auth.enabled", true},
        {"auth.roles.0", "admin"},
        {"auth.roles.1", "user"}
    };

    std::unordered_map<std::string, ConfigValue> configValues;
    JsonConfigLoader::loadConfigFile(testEnvConfigFilePath, configValues);

}