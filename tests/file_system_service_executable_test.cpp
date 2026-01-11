#include "file_system_service.h"

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

class FileSystemServiceExecutableTest : public Test
{
public:
};

TEST_F(FileSystemServiceExecutableTest, shouldReturnAbsolutePathToTestExecutable)
{
    const auto executablePath = FileSystemService::getExecutablePath();

    std::cerr << executablePath.string() << std::endl;

    ASSERT_TRUE(executablePath.string().ends_with(testExecutableName));
}
