#include "json_config_loader.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "gtest/gtest.h"

#include "file_system_service.h"
#include "nlohmann/json.hpp"
#include "environment_setter.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;
using namespace config::filesystem;

namespace
{
using ConfigValue = std::variant<std::nullptr_t, bool, int, double, std::string, float, std::vector<std::string>>;

const auto projectRootPath = FileSystemService::getExecutablePath();
const auto testConfigDirectory = projectRootPath.parent_path() / "testConfig";
const auto testEnvConfigFilePath = testConfigDirectory / "test.json";
const auto invalidConfigFilePath = testConfigDirectory / "invalid.json";
const auto customEnvironmentsConfigFilePath = testConfigDirectory / "custom-environment-variables.json";

const std::string testJson = R"(
{
    "db": {
        "port": 1996
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

const std::string envVariablesJson = R"(
{
    "aws": {
        "accountId": "AWS_ACCOUNT_ID",
        "accountKey": "AWS_ACCOUNT_KEY"
    }
}
)";

const std::string invalidJson = R"(
{
    "db": {
        "port": 1996
    },
    "auth": {
        "expiresIn": 3600,
        "enabled": true,
        "roles": [
            "admin",
            "user"
        
    }
)";

class JsonConfigLoaderTest : public Test
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

        std::ofstream customEnvironmentsConfigFile{customEnvironmentsConfigFilePath};

        customEnvironmentsConfigFile << envVariablesJson;

        std::ofstream invalidConfigFile{invalidConfigFilePath};

        invalidConfigFile << invalidJson;
    }

    void TearDown() override
    {
        EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
        EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", "");
        EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", "");

        std::filesystem::remove_all(testConfigDirectory);
    }
};

TEST_F(JsonConfigLoaderTest, loadConfigFile)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    std::unordered_map<std::string, ConfigValue> expectedValues = {{"db.port", 1996},
                                                                   {"auth.expiresIn", 3600},
                                                                   {"auth.enabled", true},
                                                                   {"auth.roles.0", "admin"},
                                                                   {"auth.roles.1", "user"}};

    std::unordered_map<std::string, ConfigValue> configValues;
    JsonConfigLoader::loadConfigFile(testEnvConfigFilePath, configValues);

    for (const auto& [key, value] : expectedValues)
    {
        auto it = configValues.find(key);
        ASSERT_NE(it, configValues.end());
        EXPECT_EQ(it->second, value);
    }
}

TEST_F(JsonConfigLoaderTest, loadConfigFileWithEnv)
{
    // Set up environment variables and paths
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", customEnvironmentsConfigFilePath.string());
    const auto awsAccountId = "9999999999";
    const auto awsAccountKey = "806223445";
    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_ID", awsAccountId);
    EnvironmentSetter::setEnvironmentVariable("AWS_ACCOUNT_KEY", awsAccountKey);

    // Define expected values after environment variable overrides
    std::unordered_map<std::string, ConfigValue> expectedValues = {
        {"aws.accountId", awsAccountId},
        {"aws.accountKey", awsAccountKey},
    };

    // Load configuration using JsonConfigLoader
    std::unordered_map<std::string, ConfigValue> configValues;
    JsonConfigLoader::loadConfigEnvFile(customEnvironmentsConfigFilePath, configValues);

    // Verify that the loaded values match the expected values
    for (const auto& [key, expectedValue] : expectedValues)
    {
        auto it = configValues.find(key);
        ASSERT_NE(it, configValues.end()) << "Key: " << key << " not found in configValues.";

        // Check the actual value against the expected value
        EXPECT_EQ(it->second, expectedValue) << "Mismatch for key: " << key;
    }
}

TEST_F(JsonConfigLoaderTest, loadConfigFileWithInvalidJson)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    std::unordered_map<std::string, ConfigValue> configValues;

    ASSERT_THROW(JsonConfigLoader::loadConfigFile(invalidConfigFilePath, configValues), std::runtime_error);
}

TEST_F(JsonConfigLoaderTest, loadConfigEnvFileWithInvalidJson)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    std::unordered_map<std::string, ConfigValue> configValues;

    ASSERT_THROW(JsonConfigLoader::loadConfigEnvFile(invalidConfigFilePath, configValues), std::runtime_error);
}

TEST_F(JsonConfigLoaderTest, loadConfigFile_withNullValue_handlesCorrectly)
{
    const std::string nullJson = R"({
        "value": null
    })";
    
    const auto nullPath = testConfigDirectory / "null.json";
    std::ofstream file{nullPath};
    file << nullJson;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    JsonConfigLoader::loadConfigFile(nullPath, configValues);
    
    ASSERT_TRUE(configValues.find("value") != configValues.end());
    ASSERT_TRUE(std::holds_alternative<std::nullptr_t>(configValues["value"]));
}

TEST_F(JsonConfigLoaderTest, loadConfigFile_withFloatPrecision_handlesCorrectly)
{
    const std::string floatJson = R"({
        "pi": 3.14159,
        "small": 0.00001
    })";
    
    const auto floatPath = testConfigDirectory / "float.json";
    std::ofstream file{floatPath};
    file << floatJson;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    JsonConfigLoader::loadConfigFile(floatPath, configValues);
    
    ASSERT_TRUE(configValues.find("pi") != configValues.end());
    ASSERT_TRUE(configValues.find("small") != configValues.end());
}

TEST_F(JsonConfigLoaderTest, loadConfigFile_withDeeplyNestedStructure_handlesCorrectly)
{
    const std::string deepJson = R"({
        "level1": {
            "level2": {
                "level3": {
                    "level4": {
                        "value": "deep"
                    }
                }
            }
        }
    })";
    
    const auto deepPath = testConfigDirectory / "deep.json";
    std::ofstream file{deepPath};
    file << deepJson;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    JsonConfigLoader::loadConfigFile(deepPath, configValues);
    
    ASSERT_TRUE(configValues.find("level1.level2.level3.level4.value") != configValues.end());
    ASSERT_EQ(std::get<std::string>(configValues["level1.level2.level3.level4.value"]), "deep");
}

TEST_F(JsonConfigLoaderTest, loadConfigFile_whenFileDoesNotExist_doesNotThrow)
{
    const auto nonExistentPath = testConfigDirectory / "nonexistent.json";
    std::unordered_map<std::string, ConfigValue> configValues;
    
    // Should not throw, just return without loading
    ASSERT_NO_THROW(JsonConfigLoader::loadConfigFile(nonExistentPath, configValues));
    ASSERT_TRUE(configValues.empty());
}
} // namespace
