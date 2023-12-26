#include "FileSystemService.h"

#include <filesystem>
#include <fstream>
#include <sstream>
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

}
