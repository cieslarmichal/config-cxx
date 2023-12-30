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

TEST_F(ProjectRootFinderTest, shouldReturnProjectRootPath)
{
    const auto projectRootPath = ProjectRootFinder::getProjectRoot();

    ASSERT_TRUE(projectRootPath.string().ends_with(projectName));
}
