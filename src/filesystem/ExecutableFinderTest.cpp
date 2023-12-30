#include "ExecutableFinder.h"

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config::filesystem;

namespace
{
#ifdef _WIN32
const std::string testExecutableName{"config-cxx-UT.exe"};
#else
const std::string testExecutableName{"config-cxx-UT"};
#endif
}

class ExecutableFinderTest : public Test
{
public:
};

TEST_F(ExecutableFinderTest, shouldReturnAbsolutePathToTestExecutable)
{
    const auto executablePath = ExecutableFinder::getExecutablePath();

    std::cerr << executablePath.string() << std::endl;

    ASSERT_TRUE(executablePath.string().ends_with(testExecutableName));
}
