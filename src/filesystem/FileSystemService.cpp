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

std::vector<std::string> FileSystemService::listFiles(const std::filesystem::path& absolutePath)
{
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(absolutePath))
    {
        if (entry.is_regular_file())
        {
            files.push_back(entry.path().string());
        }
    }

    return files;
}

}
