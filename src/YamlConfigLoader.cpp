#include "YamlConfigLoader.h"

#include <iostream>
#include <variant>
#include <functional>

#include "environment/EnvironmentParser.h"
#include "filesystem/FileSystemService.h"
#include "yaml-cpp/yaml.h"

namespace config 
{
namespace
{
ConfigValue getScalarValue(YAML::Node node); 
void flattenConfig(YAML::Node& configNode, std::unordered_map<std::string, ConfigValue>& configValues);
}

void YamlConfigLoader::loadConfigFile(const std::filesystem::path& configFilePath,
                                      std::unordered_map<std::string, ConfigValue>& configValues)
{
    const auto configFileExists = filesystem::FileSystemService::exists(configFilePath);
    
    if (!configFileExists)
    {
        return;
    }

    YAML::Node configNode = YAML::LoadFile(configFilePath);
    flattenConfig(configNode, configValues);

}

void YamlConfigLoader::loadConfigEnvFile(const std::filesystem::path& configFilePath,
                                  std::unordered_map<std::string, ConfigValue>& configValues)
{
    const auto configFileExists = filesystem::FileSystemService::exists(configFilePath);
    
    if (!configFileExists)
    {
        return;
    }

    YAML::Node configNode = YAML::LoadFile(configFilePath);
    flattenConfig(configNode, configValues);

    for (auto it = configValues.begin(); it != configValues.end(); ++it)
    {
        if (std::holds_alternative<std::string>(it->second))
        {
            std::string envKey = std::get<std::string>(it->second);
            const auto envValue = environment::EnvironmentParser::parseString(envKey);

            if (!envValue || envValue->empty())
            {
                std::cout << "Environment variable " + envKey + " not set." << std::endl;

                continue;
            }
            configValues[it->first] = *envValue;
        }

    }
}


namespace
{
void flattenConfig(YAML::Node& configNode, std::unordered_map<std::string, ConfigValue>& configValues)
{
    std::function<void(const YAML::Node&, std::string, std::unordered_map<std::string, ConfigValue>&)> flattenRecursive =
        [&](const YAML::Node& node, std::string prefix, std::unordered_map<std::string, ConfigValue>& configValues) 
    {
            if (node.IsScalar()) 
            {
                configValues[prefix] = getScalarValue(node);               
            } 
            else if (node.IsMap()) 
            {
                for (const auto& pair : node) 
                {
                    std::string prefixKey = prefix.length() == 0 ? 
                          pair.first.as<std::string>() : prefix + "." + pair.first.as<std::string>();
                    flattenRecursive(pair.second, prefixKey, configValues);
                }
            } 
            else if (node.IsSequence()) 
            {    
                 std::vector<std::string> seq;
                 if(node[0].IsScalar())
                 {
                    for (size_t i = 0; i < node.size(); ++i)
                    {
                        seq.push_back(node[i].as<std::string>());
                    }
                    configValues[prefix] = seq;
                 }
                 else
                 {
                    throw std::runtime_error("Unsupported Config: List of non-scalar datatype");
                 }

            } 
    };
    
    flattenRecursive(configNode, "", configValues);
}

ConfigValue getScalarValue(YAML::Node node) 
{
     bool set = false;
     ConfigValue nodeValue;
     try
     {
         double num = node.as<double>();
         if (num != static_cast<double>(static_cast<int>(num)))
         {
             nodeValue = num;
         }
         else
         {
             nodeValue = node.as<int>();
         }
         set = true;
     } catch (const YAML::BadConversion& e) {}

     if (!set)
     {    
         try
         {
             nodeValue = node.as<bool>();
             set = true;
         } catch (const YAML::BadConversion& e) {}
     }


     if (!set)
     {
         nodeValue = node.as<std::string>();
     }

     return nodeValue;
}

}
}
