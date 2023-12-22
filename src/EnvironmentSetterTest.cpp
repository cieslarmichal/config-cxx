#include "EnvironmentSetter.h"

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config;

class EnvironmentSetterTest : public Test
{
public:
};

TEST_F(EnvironmentSetterTest, setsEnvironmentVariable)
{
    const std::string envName = "CONFIG_CXX_TEST_PATH";

    const std::string envValue = "test";

    EnvironmentSetter::setEnvironmentVariable(envName, envValue);

    const std::string actualEnvValue = std::getenv(envName.c_str());

    ASSERT_EQ(actualEnvValue, envValue);
}
