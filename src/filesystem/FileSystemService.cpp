#include "FileSystemService.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace config::filesystem
{
std::string FileSystemService::read(const std::string& absolutePath)
{
    std::ifstream fileStream{absolutePath};

    std::stringstream buffer;

    if (!fileStream.is_open())
    {
        throw std::runtime_error("File not found: " + absolutePath);
    }

    buffer << fileStream.rdbuf();

    return buffer.str();
}

bool FileSystemService::exists(const std::string& absolutePath)
{
    return std::filesystem::exists(absolutePath);
}

std::vector<std::string> FileSystemService::listFiles(const std::string& absolutePath)
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
