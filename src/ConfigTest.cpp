#include "config-cxx/Config.h"

#include <filesystem>
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
const std::filesystem::path customEnvironmentsConfigFilePath =
    testConfigDirectory / "custom-environment-variables.json";

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
        "enabled": true
    }
}
)";

const std::string defaultJson = R"(
{
    "db": {
        "host": "localhost",
        "port": 3306
    },
    "aws": {
        "accountId": null,
        "accountKey": null,
        "region": "eu-central-2"
    }
}
)";

const std::string envVariablesJson = R"(
{
    "aws": {
        "accountId": "AWS_ACCOUNT_ID",
        "accountKey": "AWS_ACCOUNT_KEY"
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

        std::ofstream customEnvironmentsConfigFile{customEnvironmentsConfigFilePath};

        customEnvironmentsConfigFile << envVariablesJson;
    }

    void TearDown() override
    {
        std::filesystem::remove_all(testDirectoryPath);
    }

    Config config;
};

TEST_F(ConfigTest, returnsKeyValues)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory);

    const auto awsAccountId = "9999999999";
    const auto awsAccountKey = "806223445";

    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", awsAccountId);
    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", awsAccountKey);

    const std::string dbHostKey = "db.host";
    const std::string dbPortKey = "db.port";
    const std::string awsAccountIdKey = "aws.accountId";
    const std::string awsAccountKeyKey = "aws.accountKey";
    const std::string awsRegionKey = "aws.region";
    const std::string authExpiresInKey = "auth.expiresIn";
    const std::string authEnabledKey = "auth.enabled";

    const auto dbHostValue = config.get<std::string>(dbHostKey);
    const auto dbPortValue = config.get<int>(dbPortKey);
    const auto awsAccountIdValue = config.get<std::string>(awsAccountIdKey);
    const auto awsAccountKeyValue = config.get<std::string>(awsAccountKeyKey);
    const auto awsRegionValue = config.get<std::string>(awsRegionKey);
    const auto authExpiresInValue = config.get<int>(authExpiresInKey);
    const auto authEnabledValue = config.get<bool>(authEnabledKey);

    ASSERT_EQ(dbHostValue, "localhost");
    ASSERT_EQ(dbPortValue, 1996);
    ASSERT_EQ(awsAccountIdValue, awsAccountId);
    ASSERT_EQ(awsAccountKeyValue, awsAccountKey);
    ASSERT_EQ(awsRegionValue, "eu-central-1");
    ASSERT_EQ(authExpiresInValue, 3600);
    ASSERT_EQ(authEnabledValue, true);
};

TEST_F(ConfigTest, givenNotExistingKey_shouldThrow)
{
    const std::string notExistingKey = "not.existing.key";

    ASSERT_THROW(config.get<int>(notExistingKey), std::runtime_error);
}

TEST_F(ConfigTest, returnsKeyValueAsAny)
{
    const std::string path = "db.host";

    const auto configValue = config.get(path);

    ASSERT_EQ(std::any_cast<std::string>(configValue), "localhost");
}

TEST_F(ConfigTest, getAny_givenNotExistingKey_shouldThrow)
{
    const std::string notExistingKey = "not.existing.key";

    ASSERT_THROW(config.get(notExistingKey), std::runtime_error);
}
