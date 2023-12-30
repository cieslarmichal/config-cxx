#include "ExecutableFinder.h"

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config::filesystem;

namespace
{
const std::string testExecutableName{"config-cxx-UT"};
}

class ExecutableFinderTest : public Test
{
public:
};

TEST_F(ExecutableFinderTest, shouldReturnAbsolutePathToTestExecutable)
{
    const auto executablePath = ExecutableFinder::getExecutablePath();

    ASSERT_TRUE(executablePath.string().ends_with(testExecutableName));
}
