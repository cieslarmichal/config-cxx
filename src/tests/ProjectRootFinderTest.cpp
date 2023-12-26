#include "ProjectRootFinder.h"

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config::tests;

namespace
{
const std::string projectName{"config-cxx"};
}

class ProjectRootFinderTest : public Test
{
public:
};

TEST(GetProjectPathTest, givenValidProjectName_shouldReturnProjectPath)
{
    const auto projectPath = ProjectRootFinder::getProjectRoot();

    ASSERT_TRUE(projectPath.ends_with(projectName));
}
