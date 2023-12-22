#include <filesystem>
#include <fstream>

#include "errors/FileNotFound.h"
#include "FileSystemService.h"

namespace config::filesystem
{
std::string FileSystemService::read(const std::string& absolutePath) const
{
    std::ifstream fileStream{absolutePath};

    std::stringstream buffer;

    if (!fileStream.is_open())
    {
        throw errors::FileNotFoundError("File not found: " + absolutePath);
    }

    buffer << fileStream.rdbuf();

    return buffer.str();
}

bool FileSystemService::exists(const std::string& absolutePath) const
{
    return std::filesystem::exists(absolutePath);
}

}
