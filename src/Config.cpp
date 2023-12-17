#include "config-cxx/Config.h"

#include <vector>

namespace config
{
template <typename T>
T Config::get(const std::string& path)
{
    return std::any_cast<T>(path);
}

std::any Config::get(const std::string& path)
{
    return path;
}

template int Config::get<int>(const std::string&);
template float Config::get<float>(const std::string&);
template std::string Config::get<std::string>(const std::string&);
template std::vector<std::string> Config::get<std::vector<std::string>>(const std::string&);
}
