#include "file_system_service.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <limits.h>
#include <mach-o/dyld.h>
#else
#include <climits>
#include <unistd.h>
#endif

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
    return std::filesystem::current_path().root_path();
#else
    return "/";
#endif
}

std::filesystem::path FileSystemService::getExecutablePath()
{
#ifdef _WIN32
    wchar_t path[MAX_PATH] = {0};

    GetModuleFileNameW(NULL, path, MAX_PATH);

    // Use filesystem::path constructor that properly handles wide strings
    return std::filesystem::path(path);
#elif __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0)
    {
        return std::filesystem::path(path);
    }
    else
    {
        return {};
    }
#else
    char result[PATH_MAX];

    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    return std::filesystem::path{std::string(result, (count > 0) ? static_cast<unsigned long>(count) : 0)};
#endif
}

}
