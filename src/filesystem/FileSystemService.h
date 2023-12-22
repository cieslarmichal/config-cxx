#pragma once

#include <string>
#include <vector>

namespace config::filesystem
{
class FileSystemService
{
public:
    std::string read(const std::string& absolutePath) const;
    bool exists(const std::string& absolutePath) const;
    std::vector<std::string> listFiles(const std::string& absolutePath) const;
};
}
