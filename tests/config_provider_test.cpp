#include "config_provider.h"

#include "gtest/gtest.h"

#include "environment_setter.h"

using namespace ::testing;
using namespace config::environment;
using namespace config::tests;

class ConfigProviderTest : public Test
{
public:
};

TEST_F(ConfigProviderTest, parseEnvironmentVariable_returnsNullOptionalIfEnvVariableDoesNotExist)
{
    const auto envValue = ConfigProvider::parseEnvironmentVariable("NOT_EXISTING_ENV_VARIABLE");

    ASSERT_FALSE(envValue);
}

TEST_F(ConfigProviderTest, parseEnvironmentVariable_returnsValueIfEnvVariableExists)
{
    const auto envName = "CONFIG_CXX_TEST_PATH";

    const auto expectedEnvValue = "test";

    EnvironmentSetter::setEnvironmentVariable(envName, expectedEnvValue);

    const auto envValue = ConfigProvider::parseEnvironmentVariable(envName);

    ASSERT_TRUE(envValue);
    ASSERT_EQ(envValue, expectedEnvValue);
}

TEST_F(ConfigProviderTest, parseEnvironmentVariable_returnsEmptyStringWhenEnvIsEmpty)
{
    const auto envName = "CONFIG_CXX_EMPTY_TEST";
    
    EnvironmentSetter::setEnvironmentVariable(envName, "");
    
    const auto envValue = ConfigProvider::parseEnvironmentVariable(envName);
    
    ASSERT_TRUE(envValue);
    ASSERT_EQ(*envValue, "");
}



TEST_F(ConfigProviderTest, getCxxEnv_returnsValueWhenEnvIsSet)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "production");
    
    const auto cxxEnv = ConfigProvider::getCxxEnv();
    
    ASSERT_EQ(cxxEnv, "production");
    
    // Clean up
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "");
}

TEST_F(ConfigProviderTest, getCxxEnv_returnsDevelopmentWhenEnvIsEmpty)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "");
    
    const auto cxxEnv = ConfigProvider::getCxxEnv();
    
    ASSERT_EQ(cxxEnv, "development");
}

TEST_F(ConfigProviderTest, getCxxEnv_returnsTestWhenSet)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "test");
    
    const auto cxxEnv = ConfigProvider::getCxxEnv();
    
    ASSERT_EQ(cxxEnv, "test");
    
    // Clean up
    EnvironmentSetter::setEnvironmentVariable("CXX_ENV", "");
}



TEST_F(ConfigProviderTest, getCxxConfigDir_returnsValueWhenSet)
{
    const std::string testPath = "/tmp/config";
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", testPath);
    
    const auto configDir = ConfigProvider::getCxxConfigDir();
    
    ASSERT_TRUE(configDir);
    ASSERT_EQ(*configDir, testPath);
    
    // Clean up
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");
}

TEST_F(ConfigProviderTest, getCxxConfigDir_returnsNullOptWhenEmpty)
{
    EnvironmentSetter::setEnvironmentVariable("CXX_CONFIG_DIR", "");
    
    const auto configDir = ConfigProvider::getCxxConfigDir();
    
    ASSERT_FALSE(configDir);
}
