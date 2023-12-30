#include "ExecutableFinder.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <climits>
#include <unistd.h>
#endif

namespace config::filesystem
{
std::filesystem::path ExecutableFinder::getExecutablePath()
{
#ifdef _WIN32
    wchar_t path[MAX_PATH] = {0};

    GetModuleFileNameW(NULL, path, MAX_PATH);

    std::wstring wpath(path);

    const std::filesystem::path path = std::string(wpath.begin(), wpath.end());

    return path;
#else
    char result[PATH_MAX];

    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    const std::filesystem::path path = std::string(result, (count > 0) ? static_cast<unsigned long>(count) : 0);

    return path;
#endif
}
}
