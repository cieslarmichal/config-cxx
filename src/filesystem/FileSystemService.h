#pragma once

#include <string>
#include <vector>

namespace config::filesystem
{
class FileSystemService
{
public:
    static std::string read(const std::string& absolutePath);
    static bool exists(const std::string& absolutePath);
    static std::vector<std::string> listFiles(const std::string& absolutePath);
};
}
