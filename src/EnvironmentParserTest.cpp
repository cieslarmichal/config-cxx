#include "EnvironmentParser.h"

#include "gtest/gtest.h"

#include "tests/EnvironmentSetter.h"

using namespace ::testing;
using namespace config;
using namespace config::tests;

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

    EnvironmentSetter::setEnvironmentVariable(envName, expectedEnvValue);

    const auto envValue = EnvironmentParser::parseString(envName);

    ASSERT_TRUE(envValue);
    ASSERT_EQ(envValue, expectedEnvValue);
}
