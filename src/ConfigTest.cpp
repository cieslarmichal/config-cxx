#include "config-cxx/Config.h"

#include <filesystem>
#include <optional>
#include <fstream>
#include <stdexcept>

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
const auto fallbackConfigDirectory = projectRootPath.parent_path() / "config";
const auto emptyConfigDirectory = projectRootPath.parent_path() / "emptyConfig";
const auto testConfigDirectory = projectRootPath.parent_path() / "testConfig";
const auto testEnvConfigFilePath = testConfigDirectory / "test.json";
const auto testYamlEnvConfigFilePath = testConfigDirectory / "test.yml";
const auto testXmlEnvConfigFilePath = testConfigDirectory / "test.xml";
const auto localConfigFilePath = testConfigDirectory / "local.json";
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

const std::string localJson = R"(
{
    "aws": {
        "region": "eu-west-1"
    }
}
)";

const std::string testYaml = R"(
logging:
    level: debug
    file: /var/log/myapp.log
)";

const std::string testXml = R"(
<configuration>
    <user>
        <name>Koko</name>
        <active>false</active>
    </user>
</configuration>
)";

} // anonymous namespace

class ConfigTest : public Test
{
public:
    void SetUp() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "");
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", "");

        std::filesystem::remove_all(testConfigDirectory);

        std::filesystem::create_directory(testConfigDirectory);

        std::ofstream testEnvConfigFile{testEnvConfigFilePath};

        testEnvConfigFile << testJson;

        std::ofstream defaultConfigFile{defaultConfigFilePath};

        defaultConfigFile << defaultJson;

        std::ofstream customEnvironmentsConfigFile{customEnvironmentsConfigFilePath};

        customEnvironmentsConfigFile << envVariablesJson;

        std::ofstream localConfigFile{localConfigFilePath};

        localConfigFile << localJson;

        std::ofstream testYamlEnvConfigFile{testYamlEnvConfigFilePath};

        testYamlEnvConfigFile << testYaml;

        std::ofstream testXmlEnvConfigFile(testXmlEnvConfigFilePath);

        testXmlEnvConfigFile << testXml;

        std::filesystem::remove_all(fallbackConfigDirectory);

        std::filesystem::create_directory(fallbackConfigDirectory);

        std::ofstream fallbackDefaultConfigFile{fallbackConfigDirectory / "default.json"};

        fallbackDefaultConfigFile << defaultJson;

        std::ofstream fallbackDevelopmentConfigFile{fallbackConfigDirectory / "development.json"};

        fallbackDevelopmentConfigFile << developmentJson;

        std::filesystem::remove_all(emptyConfigDirectory);

        std::filesystem::create_directory(emptyConfigDirectory);
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "");
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", "");

        std::filesystem::remove_all(testConfigDirectory);

        std::filesystem::remove_all(fallbackConfigDirectory);

        std::filesystem::remove_all(emptyConfigDirectory);
    }
};

TEST_F(ConfigTest, givenCxxEnvAndConfigDir_returnsKeyValues)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

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
    ASSERT_EQ(awsRegionValue, "eu-west-1");
    ASSERT_EQ(authExpiresInValue, 3600);
    ASSERT_EQ(authEnabledValue, true);
    ASSERT_EQ(authRolesValue, expectedAuthRoles);
}

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
}

TEST_F(ConfigTest, givenConfigDirWithoutConfigFiles_shouldThrow)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", emptyConfigDirectory.string());

    Config config;

    ASSERT_THROW(config.get<std::string>("example.key"), std::runtime_error);
}

TEST_F(ConfigTest, givenCxxEnvAndConfigDirAndNotExistingKey_shouldThrow)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    Config config;

    const std::string notExistingKey = "not.existing.key";

    ASSERT_THROW(config.get<int>(notExistingKey), std::runtime_error);
}

TEST_F(ConfigTest, getAny_givenNotExistingKey_shouldThrow)
{
    const std::string notExistingKey = "not.existing.key";

    Config config;

    ASSERT_THROW(config.get(notExistingKey), std::runtime_error);
}

TEST_F(ConfigTest, configHasTest)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    const auto awsAccountId = "9999999999";
    const auto awsAccountKey = "806223445";

    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", awsAccountId);
    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", awsAccountKey);

    Config config;

    const std::string dbHostKey = "db.host";
    const std::string dbPortKey = "db.port";
    const std::string awsAccountIdKey = "aws.accountId";
    const std::string awsAccountKeyKey = "aws.accountKey";

    const std::string notExistingKey = "not.existing.key";
    const std::string notExistingKey2 = "not.existing.key2";

    ASSERT_TRUE(config.has(dbHostKey));
    ASSERT_TRUE(config.has(dbPortKey));
    ASSERT_TRUE(config.has(awsAccountIdKey));
    ASSERT_TRUE(config.has(awsAccountKeyKey));

    ASSERT_FALSE(config.has(notExistingKey));
    ASSERT_FALSE(config.has(notExistingKey2));
}

TEST_F(ConfigTest, configWorksWithJsonAndYaml)
{

    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

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
    const std::string loggingLevelKey = "logging.level";
    const std::string loggingFileKey = "logging.file";

    const auto dbHostValue = config.get<std::string>(dbHostKey);
    const auto dbPortValue = config.get<int>(dbPortKey);
    const auto awsAccountIdValue = config.get<std::string>(awsAccountIdKey);
    const auto awsAccountKeyValue = config.get<std::string>(awsAccountKeyKey);
    const auto awsRegionValue = config.get<std::string>(awsRegionKey);
    const auto authExpiresInValue = config.get<int>(authExpiresInKey);
    const auto authEnabledValue = config.get<bool>(authEnabledKey);
    const auto authRolesValue = config.get<std::vector<std::string>>(authRolesKey);
    const auto loggingLevelValue = config.get<std::string>(loggingLevelKey);
    const auto loggingFileValue = config.get<std::string>(loggingFileKey);

    const auto expectedAuthRoles = std::vector<std::string>{"admin", "user"};

    ASSERT_EQ(dbHostValue, "localhost");
    ASSERT_EQ(dbPortValue, 1996);
    ASSERT_EQ(awsAccountIdValue, awsAccountId);
    ASSERT_EQ(awsAccountKeyValue, awsAccountKey);
    ASSERT_EQ(awsRegionValue, "eu-west-1");
    ASSERT_EQ(authExpiresInValue, 3600);
    ASSERT_EQ(authEnabledValue, true);
    ASSERT_EQ(authRolesValue, expectedAuthRoles);
    ASSERT_EQ(loggingLevelValue, "debug");
    ASSERT_EQ(loggingFileValue, "/var/log/myapp.log");
}


TEST_F(ConfigTest, configWorksWithJsonAndXml)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

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
    const std::string userName = "user.name";
    const std::string userActive = "user.active";

    const auto dbHostValue = config.get<std::string>(dbHostKey);
    const auto dbPortValue = config.get<int>(dbPortKey);
    const auto awsAccountIdValue = config.get<std::string>(awsAccountIdKey);
    const auto awsAccountKeyValue = config.get<std::string>(awsAccountKeyKey);
    const auto awsRegionValue = config.get<std::string>(awsRegionKey);
    const auto authExpiresInValue = config.get<int>(authExpiresInKey);
    const auto authEnabledValue = config.get<bool>(authEnabledKey);
    const auto authRolesValue = config.get<std::vector<std::string>>(authRolesKey);
    const auto userNameValue = config.get<std::string>(userName);
    const auto userActiveValue = config.get<bool>(userActive);

    const auto expectedAuthRoles = std::vector<std::string>{"admin", "user"};

    ASSERT_EQ(dbHostValue, "localhost");
    ASSERT_EQ(dbPortValue, 1996);
    ASSERT_EQ(awsAccountIdValue, awsAccountId);
    ASSERT_EQ(awsAccountKeyValue, awsAccountKey);
    ASSERT_EQ(awsRegionValue, "eu-west-1");
    ASSERT_EQ(authExpiresInValue, 3600);
    ASSERT_EQ(authEnabledValue, true);
    ASSERT_EQ(authRolesValue, expectedAuthRoles);
    ASSERT_EQ(userNameValue, "Koko");
    ASSERT_EQ(userActiveValue, false);
}

TEST_F(ConfigTest, givenCxxEnvAndConfigDir_returnsOptionalKeyValues)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    const auto awsAccountId = "9999999999";
    const auto awsAccountKey = "806223445";

    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", awsAccountId);
    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", awsAccountKey);

    Config config;

    const std::string dbHostKey = "db.host";
    const std::string dbPortKey = "db.port";
    const std::string awsAccountIdKey = "aws.accountId";
    const std::string awsAccountKeyKey = "aws.accountKey";
    const std::string nonExistantKey = "redis.host";

    const auto dbHostValue = config.getOptional<std::string>(dbHostKey);
    const auto dbPortValue = config.getOptional<int>(dbPortKey);
    const auto awsAccountIdValue = config.getOptional<std::string>(awsAccountIdKey);
    const auto awsAccountKeyValue = config.getOptional<std::string>(awsAccountKeyKey);
    const auto nonExistantKeyValue = config.getOptional<std::string>(nonExistantKey);

    ASSERT_EQ(*dbHostValue, "localhost");
    ASSERT_EQ(*dbPortValue, 1996);
    ASSERT_EQ(*awsAccountIdValue, awsAccountId);
    ASSERT_EQ(*awsAccountKeyValue, awsAccountKey);
    ASSERT_EQ(nonExistantKeyValue.has_value(), false);
}
