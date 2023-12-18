#include "EnvironmentParser.h"

#include <cstdlib>

#include "gtest/gtest.h"

#ifdef _WIN32
#include <windows.h>
#endif

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

#ifdef _WIN32
    SetEnvironmentVariableA(envName, expectedEnvValue);
#elif __MINGW32__
    char s[] = "CONFIG_CXX_TEST_PATH=test";
    putenv(s);
#else
    setenv(envName, expectedEnvValue, 1);
#endif

    const auto envValue = EnvironmentParser::parseString(envName);

    std::cerr << envValue.value() << std::endl;

    ASSERT_TRUE(envValue);
    ASSERT_EQ(envValue, expectedEnvValue);
}
