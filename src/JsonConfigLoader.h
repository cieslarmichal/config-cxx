#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace config
{
using ConfigValue = std::variant<std::nullptr_t, bool, int, double, std::string, float, std::vector<std::string>>;

class JsonConfigLoader
{
public:
    static void loadConfigFile(const std::filesystem::path& configFilePath,
                               std::unordered_map<std::string, ConfigValue>& configValues);
    static void loadConfigEnvFile(const std::filesystem::path& configFilePath,
                                  std::unordered_map<std::string, ConfigValue>& configValues);
};
};
