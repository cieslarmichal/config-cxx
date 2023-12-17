#include "ConfigDirectoryPathResolver.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <climits>
#include <unistd.h>
#endif

namespace config
{

std::string ConfigDirectoryPathResolver::getConfigDirectoryPath()
{
    return std::string();
}

std::string ConfigDirectoryPathResolver::getExecutablePath()
{
#ifdef _WIN32
    wchar_t path[MAX_PATH] = {0};

    GetModuleFileNameW(NULL, path, MAX_PATH);

    std::wstring wpath(path);

    return std::string(wpath.begin(), wpath.end());
#else
    char result[PATH_MAX];

    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    // TODO: check if works with curly braces
    return std::string{result, (count > 0) ? static_cast<unsigned long>(count) : 0};
#endif
}

}
