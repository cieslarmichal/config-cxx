#pragma once

#include <string>

namespace config::filesystem
{
class ExecutableFinder
{
public:
    static std::string getExecutablePath();
};
}
