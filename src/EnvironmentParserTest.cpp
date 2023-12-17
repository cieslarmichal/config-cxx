#include "EnvironmentParser.h"

#include <cstdlib>

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config;

class EnvironmentParserTest : public Test
{
public:
};

TEST_F(EnvironmentParserTest, returnsNullOptionalIfEnvVariableDoesNotExist)
{
    const auto envValue = EnvironmentParser::parseString("NOT_EXISTING_ENV_VARIABLE");

    ASSERT_FALSE(envValue);
}

TEST_F(EnvironmentParserTest, returnsValueIfEnvVariableExists)
{
    const auto envName = "CONFIG_CXX_TEST_PATH";

    const auto expectedEnvValue = "test";

    setenv(envName, expectedEnvValue, 1);

    const auto envValue = EnvironmentParser::parseString(envName);

    ASSERT_TRUE(envValue);
    ASSERT_EQ(envValue, expectedEnvValue);
}
