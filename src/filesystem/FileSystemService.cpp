#include "FileSystemService.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace config::filesystem
{
std::string FileSystemService::read(const std::filesystem::path& absolutePath)
{
    std::ifstream fileStream{absolutePath};

    std::stringstream buffer;

    if (!fileStream.is_open())
    {
        throw std::runtime_error("File not found: " + absolutePath.string());
    }

    buffer << fileStream.rdbuf();

    return buffer.str();
}

bool FileSystemService::exists(const std::filesystem::path& absolutePath)
{
    return std::filesystem::exists(absolutePath);
}

bool FileSystemService::isDirectory(const std::filesystem::path& absolutePath)
{
    return std::filesystem::is_directory(absolutePath);
}

bool FileSystemService::isRelative(const std::filesystem::path& path)
{
    return path.is_relative();
}

std::filesystem::path FileSystemService::getCurrentWorkingDirectory()
{
    return std::filesystem::current_path();
}

std::filesystem::path FileSystemService::getSystemRootPath()
{
#if defined(_WIN32)
    return fs::current_path().root_path();
#else
    return "/";
#endif
}

}
