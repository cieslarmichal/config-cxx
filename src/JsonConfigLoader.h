#pragma once

#include <any>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace config
{
class JsonConfigLoader
{
public:
    static void loadConfigFile(const std::filesystem::path& configFilePath,
                               std::unordered_map<std::string, std::any>& configValues);
    static void loadConfigEnvFile(const std::filesystem::path& configFilePath,
                                  std::unordered_map<std::string, std::any>& configValues);
};
};
