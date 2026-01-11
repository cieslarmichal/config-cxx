#include "yaml_config_loader.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "gtest/gtest.h"

#include "file_system_service.h"
#include "environment_setter.h"
#include "yaml-cpp/yaml.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;
using namespace config::filesystem;

namespace
{
using ConfigValue = std::variant<std::nullptr_t, bool, int, double, std::string, float, std::vector<std::string>>;

const auto projectRootPath = FileSystemService::getExecutablePath();
const auto testConfigDirectory = projectRootPath.parent_path() / "testConfig";
const auto testEnvConfigFilePath = testConfigDirectory / "test.yml";
const auto invalidConfigFilePath = testConfigDirectory / "invalid.yml";
const auto customEnvironmentsConfigFilePath = testConfigDirectory / "custom-environment-variables.yml";

const std::string testYaml = R"(
db:
    port: 1996
auth:
    expiresIn: 3600
    enabled: true
    roles:
        - admin
        - user
)";

const std::string envVariablesYaml = R"(
aws:
    accountId: AWS_ACCOUNT_ID
    accountKey: AWS_ACCOUNT_KEY
)";

const std::string invalidYaml = R"(
invalid_key:
    value1: 10
    - value2: 20
)";

class YamlConfigLoaderTest : public Test
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

        testEnvConfigFile << testYaml;

        std::ofstream customEnvironmentsConfigFile{customEnvironmentsConfigFilePath};

        customEnvironmentsConfigFile << envVariablesYaml;

        std::ofstream invalidConfigFile{invalidConfigFilePath};

        invalidConfigFile << invalidYaml;
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

TEST_F(YamlConfigLoaderTest, loadConfigFile)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    std::unordered_map<std::string, ConfigValue> expectedValues = {
        {"db.port", 1996}, {"auth.expiresIn", 3600}, {"auth.enabled", true}};
    std::vector<std::string> roles = {"admin", "user"};
    expectedValues["auth.roles"] = roles;

    std::unordered_map<std::string, ConfigValue> configValues;
    YamlConfigLoader::loadConfigFile(testEnvConfigFilePath, configValues);

    for (const auto& [key, value] : expectedValues)
    {
        auto it = configValues.find(key);
        ASSERT_NE(it, configValues.end());
        EXPECT_EQ(it->second, value);
    }
}

TEST_F(YamlConfigLoaderTest, loadConfigFileWithEnv)
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
    YamlConfigLoader::loadConfigEnvFile(customEnvironmentsConfigFilePath, configValues);

    // Verify that the loaded values match the expected values
    for (const auto& [key, expectedValue] : expectedValues)
    {
        auto it = configValues.find(key);
        ASSERT_NE(it, configValues.end()) << "Key: " << key << " not found in configValues.";

        // Check the actual value against the expected value
        EXPECT_EQ(it->second, expectedValue) << "Mismatch for key: " << key;
    }
}

TEST_F(YamlConfigLoaderTest, loadConfigFileWithInvalidYaml)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testConfigDirectory.string());

    std::unordered_map<std::string, ConfigValue> configValues;

    ASSERT_THROW(YamlConfigLoader::loadConfigFile(invalidConfigFilePath, configValues), YAML::ParserException);
}

TEST_F(YamlConfigLoaderTest, loadConfigFile_withMultilineString_handlesCorrectly)
{
    const std::string multilineYaml = R"(
description: |
  This is a multiline
  string value
  with multiple lines
)";
    
    const auto multilinePath = testConfigDirectory / "multiline.yaml";
    std::ofstream file{multilinePath};
    file << multilineYaml;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    YamlConfigLoader::loadConfigFile(multilinePath, configValues);
    
    ASSERT_TRUE(configValues.find("description") != configValues.end());
    const auto& value = std::get<std::string>(configValues["description"]);
    ASSERT_TRUE(value.find("multiline") != std::string::npos);
}

TEST_F(YamlConfigLoaderTest, loadConfigFile_withBooleanValues_handlesCorrectly)
{
    const std::string boolYaml = R"(
enabled: true
disabled: false
yes_value: yes
no_value: no
)";
    
    const auto boolPath = testConfigDirectory / "bool.yaml";
    std::ofstream file{boolPath};
    file << boolYaml;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    YamlConfigLoader::loadConfigFile(boolPath, configValues);
    
    ASSERT_TRUE(configValues.find("enabled") != configValues.end());
    ASSERT_TRUE(configValues.find("disabled") != configValues.end());
}

TEST_F(YamlConfigLoaderTest, loadConfigFile_withNumericTypes_handlesCorrectly)
{
    const std::string numericYaml = R"(
integer: 42
float: 3.14
scientific: 1.23e-4
negative: -100
)";
    
    const auto numericPath = testConfigDirectory / "numeric.yaml";
    std::ofstream file{numericPath};
    file << numericYaml;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    YamlConfigLoader::loadConfigFile(numericPath, configValues);
    
    ASSERT_TRUE(configValues.find("integer") != configValues.end());
    ASSERT_TRUE(configValues.find("float") != configValues.end());
    ASSERT_TRUE(configValues.find("scientific") != configValues.end());
    ASSERT_TRUE(configValues.find("negative") != configValues.end());
}

TEST_F(YamlConfigLoaderTest, loadConfigFile_whenFileDoesNotExist_doesNotThrow)
{
    const auto nonExistentPath = testConfigDirectory / "nonexistent.yaml";
    std::unordered_map<std::string, ConfigValue> configValues;
    
    ASSERT_NO_THROW(YamlConfigLoader::loadConfigFile(nonExistentPath, configValues));
    ASSERT_TRUE(configValues.empty());
}

TEST_F(YamlConfigLoaderTest, loadConfigFile_withDeeplyNestedStructure_handlesCorrectly)
{
    const std::string deepYaml = R"(
level1:
  level2:
    level3:
      level4:
        value: deep
)";
    
    const auto deepPath = testConfigDirectory / "deep.yaml";
    std::ofstream file{deepPath};
    file << deepYaml;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    YamlConfigLoader::loadConfigFile(deepPath, configValues);
    
    ASSERT_TRUE(configValues.find("level1.level2.level3.level4.value") != configValues.end());
    ASSERT_EQ(std::get<std::string>(configValues["level1.level2.level3.level4.value"]), "deep");
}

TEST_F(YamlConfigLoaderTest, loadConfigFile_withEmptyArray_doesNotCrash)
{
    const std::string emptyArrayYaml = R"(
items: []
empty_list: []
name: "test"
values:
  nested_empty: []
)";
    
    const auto emptyArrayPath = testConfigDirectory / "empty_array.yaml";
    std::ofstream file{emptyArrayPath};
    file << emptyArrayYaml;
    file.close();
    
    std::unordered_map<std::string, ConfigValue> configValues;
    
    // Should not crash when encountering empty arrays
    ASSERT_NO_THROW(YamlConfigLoader::loadConfigFile(emptyArrayPath, configValues));
    
    // Non-empty values should still be loaded
    ASSERT_TRUE(configValues.find("name") != configValues.end());
    ASSERT_EQ(std::get<std::string>(configValues["name"]), "test");
}

}
