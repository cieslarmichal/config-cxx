#include "ProjectRootFinder.h"

#include "../filesystem/ExecutableFinder.h"

namespace config::tests
{
std::filesystem::path ProjectRootFinder::getProjectRoot()
{
    const auto executablePath{filesystem::ExecutableFinder::getExecutablePath()};

    for (auto path = executablePath.parent_path(); path != path.root_directory(); path = path.parent_path())
    {
        if (path.filename() == "config-cxx")
        {
            return path;
        }
    }

    throw std::runtime_error{"Project root not found."};
}
}
