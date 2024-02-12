#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace config
{
const std::string configTagName = "configuration";
using BaseTypes = std::variant<std::nullptr_t, bool, int, float, double, std::string>;
using ConfigValue = std::variant<std::nullptr_t, bool, int, double, std::string, float, std::vector<std::string>>;

class XmlConfigLoader
{
public:
    static void loadConfigFile(const std::filesystem::path& configFilePath,
                               std::unordered_map<std::string, ConfigValue>& configValues);
    static void loadConfigEnvFile(const std::filesystem::path& configFilePath,
                                  std::unordered_map<std::string, ConfigValue>& configValues);
};
} // config namespace