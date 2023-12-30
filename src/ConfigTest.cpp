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
const auto projectRootPath = ProjectRootFinder::getProjectRoot();
const auto testDirectoryPath = projectRootPath / "tests" / "configData";
const auto testConfigDirectory = testDirectoryPath / "config";
const auto testEnvConfigFilePath = testConfigDirectory / "test.json";
const auto defaultConfigFilePath = testConfigDirectory / "default.json";
const auto customEnvironmentsConfigFilePath = testConfigDirectory / "custom-environment-variables.json";

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

const std::string developmentJson = R"(
{
    "db": {
        "port": 2000
    },
    "aws": {
        "accountId": "0987654321",
        "accountKey": "321"
    },
    "auth": {
        "expiresIn": 7200,
        "enabled": false,
        "roles": [
            "anonymous",
            "user"
        ]
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
        EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "");
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", "");

        std::filesystem::remove_all(testDirectoryPath);

        std::filesystem::create_directories(testConfigDirectory);

        std::ofstream testEnvConfigFile{testEnvConfigFilePath};

        testEnvConfigFile << testJson;

        std::ofstream defaultConfigFile{defaultConfigFilePath};

        defaultConfigFile << defaultJson;

        std::ofstream customEnvironmentsConfigFile{customEnvironmentsConfigFilePath};

        customEnvironmentsConfigFile << envVariablesJson;

        std::filesystem::remove_all(otherConfigPath);

        std::filesystem::create_directory(otherConfigPath);

        std::ofstream otherDefaultConfigFile{otherConfigPath / "default.json"};
        std::ofstream developmentConfigFile{otherConfigPath / "development.json"};

        otherDefaultConfigFile << defaultJson;
        developmentConfigFile << developmentJson;

        std::filesystem::remove_all(emptyConfigPath);

        std::filesystem::create_directory(emptyConfigPath);
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "");
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", "");

        std::filesystem::remove_all(testDirectoryPath);

        std::filesystem::remove_all(otherConfigPath);

        std::filesystem::remove_all(emptyConfigPath);
    }

    std::filesystem::path otherConfigPath = "../config";
    std::filesystem::path emptyConfigPath = "../../config";
};

TEST_F(ConfigTest, givenCxxEnvAndConfigDir_returnsKeyValues)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory);

    const auto awsAccountId = "9999999999";
    const auto awsAccountKey = "806223445";

    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", awsAccountId);
    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", awsAccountKey);

    Config config;

    const std::string dbHostKey = "db.host";
    const std::string dbPortKey = "db.port";
    const std::string awsAccountIdKey = "aws.accountId";
    const std::string awsAccountKeyKey = "aws.accountKey";
    const std::string awsRegionKey = "aws.region";
    const std::string authExpiresInKey = "auth.expiresIn";
    const std::string authEnabledKey = "auth.enabled";
    const std::string authRolesKey = "auth.roles";

    const auto dbHostValue = config.get<std::string>(dbHostKey);
    const auto dbPortValue = config.get<int>(dbPortKey);
    const auto awsAccountIdValue = config.get<std::string>(awsAccountIdKey);
    const auto awsAccountKeyValue = config.get<std::string>(awsAccountKeyKey);
    const auto awsRegionValue = config.get<std::string>(awsRegionKey);
    const auto authExpiresInValue = config.get<int>(authExpiresInKey);
    const auto authEnabledValue = config.get<bool>(authEnabledKey);
    const auto authRolesValue = config.get<std::vector<std::string>>(authRolesKey);

    const auto expectedAuthRoles = std::vector<std::string>{"admin", "user"};

    ASSERT_EQ(dbHostValue, "localhost");
    ASSERT_EQ(dbPortValue, 1996);
    ASSERT_EQ(awsAccountIdValue, awsAccountId);
    ASSERT_EQ(awsAccountKeyValue, awsAccountKey);
    ASSERT_EQ(awsRegionValue, "eu-central-1");
    ASSERT_EQ(authExpiresInValue, 3600);
    ASSERT_EQ(authEnabledValue, true);
    ASSERT_EQ(authRolesValue, expectedAuthRoles);
};

TEST_F(ConfigTest, givenCxxEnvAndConfigDirWithoutEnvVarsSets_throws)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory);

    Config config;

    const std::string awsAccountIdKey = "aws.accountId";
    const std::string awsAccountKeyKey = "aws.accountKey";

    ASSERT_THROW(config.get<std::string>(awsAccountIdKey), std::runtime_error);
    ASSERT_THROW(config.get<std::string>(awsAccountKeyKey), std::runtime_error);
};

TEST_F(ConfigTest, givenNoCxxEnvAndNoConfigDir_returnsDevelopmentKeyValues)
{
    const std::string dbHostKey = "db.host";
    const std::string dbPortKey = "db.port";
    const std::string awsAccountIdKey = "aws.accountId";
    const std::string awsAccountKeyKey = "aws.accountKey";
    const std::string awsRegionKey = "aws.region";
    const std::string authExpiresInKey = "auth.expiresIn";
    const std::string authEnabledKey = "auth.enabled";
    const std::string authRolesKey = "auth.roles";

    Config config;

    const auto dbHostValue = config.get<std::string>(dbHostKey);
    const auto dbPortValue = config.get<int>(dbPortKey);
    const auto awsAccountIdValue = config.get<std::string>(awsAccountIdKey);
    const auto awsAccountKeyValue = config.get<std::string>(awsAccountKeyKey);
    const auto awsRegionValue = config.get<std::string>(awsRegionKey);
    const auto authExpiresInValue = config.get<int>(authExpiresInKey);
    const auto authEnabledValue = config.get<bool>(authEnabledKey);
    const auto authRolesValue = config.get<std::vector<std::string>>(authRolesKey);

    const auto expectedAuthRoles = std::vector<std::string>{"anonymous", "user"};

    ASSERT_EQ(dbHostValue, "localhost");
    ASSERT_EQ(dbPortValue, 2000);
    ASSERT_EQ(awsAccountIdValue, "0987654321");
    ASSERT_EQ(awsAccountKeyValue, "321");
    ASSERT_EQ(awsRegionValue, "eu-central-2");
    ASSERT_EQ(authExpiresInValue, 7200);
    ASSERT_EQ(authEnabledValue, false);
    ASSERT_EQ(authRolesValue, expectedAuthRoles);
};

TEST_F(ConfigTest, givenConfigDirWithoutConfigFiles_shouldThrow)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", emptyConfigPath);

    Config config;

    ASSERT_THROW(config.get<std::string>("example.key"), std::runtime_error);
};

TEST_F(ConfigTest, givenCxxEnvAndConfigDirAndNotExistingKey_shouldThrow)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory);

    Config config;

    const std::string notExistingKey = "not.existing.key";

    ASSERT_THROW(config.get<int>(notExistingKey), std::runtime_error);
}

TEST_F(ConfigTest, returnsKeyValueAsAny)
{
    const std::string path = "db.host";

    Config config;

    const auto configValue = config.get(path);

    ASSERT_EQ(std::any_cast<std::string>(configValue), "localhost");
}

TEST_F(ConfigTest, getAny_givenNotExistingKey_shouldThrow)
{
    const std::string notExistingKey = "not.existing.key";

    Config config;

    ASSERT_THROW(config.get(notExistingKey), std::runtime_error);
}
