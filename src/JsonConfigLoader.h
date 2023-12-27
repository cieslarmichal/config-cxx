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
    explicit JsonConfigLoader(std::unordered_map<std::string, std::any>& values);

    void loadConfigFile(const std::string& configFilePath);
    void loadConfigEnvFile(const std::string& configFilePath);

private:
    std::unordered_map<std::string, std::any>& values;
};
};
