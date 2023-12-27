#pragma once

#include <any>
#include <string>
#include <unordered_map>

namespace config
{
class JsonConfigLoader
{
public:
    static void loadConfigFile(const std::string& configFilePath,
                               std::unordered_map<std::string, std::any>& configValues);
    static void loadConfigEnvFile(const std::string& configFilePath,
                                  std::unordered_map<std::string, std::any>& configValues);
};
};
