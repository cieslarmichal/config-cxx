#include "config-cxx/Config.h"

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config;

class ConfigTest : public Test
{
public:
};

TEST_F(ConfigTest, returnsKeyPath)
{
    const std::string keyPath = "db.host";

    const auto configValue = Config::get<std::string>(keyPath);

    ASSERT_EQ(configValue, keyPath);
}

TEST_F(ConfigTest, returnsKeyPathAsAny)
{
    const std::string path = "db.host";

    const auto configValue = Config::get<std::string>(path);

    ASSERT_EQ(std::any_cast<std::string>(configValue), path);
}
