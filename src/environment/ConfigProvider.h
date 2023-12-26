#pragma once

#include <optional>
#include <string>

namespace config::environment
{
class ConfigProvider
{
public:
    static std::optional<std::string> getCxxEnv();
    static std::optional<std::string> getCxxConfigDir();
};
}
