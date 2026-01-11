#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace config::filesystem
{
class FileSystemService
{
public:
    static std::string read(const std::filesystem::path& absolutePath);
    static bool exists(const std::filesystem::path& absolutePath);
    static bool isDirectory(const std::filesystem::path& absolutePath);
    static bool isRelative(const std::filesystem::path& path);
    static std::filesystem::path getSystemRootPath();
    static std::filesystem::path getCurrentWorkingDirectory();
    static std::filesystem::path getExecutablePath();
};
}
