#pragma once

#include <filesystem>

namespace config::filesystem
{
class ExecutableFinder
{
public:
    static std::filesystem::path getExecutablePath();
};
}
