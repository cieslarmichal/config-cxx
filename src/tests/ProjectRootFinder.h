#pragma once

#include <filesystem>
#include <string>

namespace config::tests
{
class ProjectRootFinder
{
public:
    static std::filesystem::path getProjectRoot();
};
}
