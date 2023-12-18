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

#if defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
    const std::string envNameWithEquals = envName + "=" + expectedEnvValue;
    _putenv_s(envNameWithEquals.c_str());
#else
    setenv(envName, expectedEnvValue, 1);
#endif

    const auto envValue = EnvironmentParser::parseString(envName);

    std::cerr << envValue.value() << std::endl;

    ASSERT_TRUE(envValue);
    ASSERT_EQ(envValue, expectedEnvValue);
}
