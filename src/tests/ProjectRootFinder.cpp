#include "ProjectRootFinder.h"

#include <filesystem>

#include "../filesystem/ExecutableFinder.h"

namespace config::tests
{
std::string ProjectRootFinder::getProjectRoot()
{
    std::filesystem::path executablePath{filesystem::ExecutableFinder::getExecutablePath()};

    for (auto path = executablePath.parent_path(); path != path.root_directory(); path = path.parent_path())
    {
        if (path.filename() == "config-cxx")
        {
            return path.generic_string();
        }
    }

    throw std::runtime_error{"Project root not found."};
}
}
