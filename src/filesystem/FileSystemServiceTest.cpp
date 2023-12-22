#include "FileSystemService.h"

#include <algorithm>
#include <fstream>

#include "gtest/gtest.h"

#include "../tests/ProjectRootFinder.h"

using namespace ::testing;
using namespace config::filesystem;
using namespace config::tests;

namespace
{
const std::string projectRoot = ProjectRootFinder::getProjectRoot();
const std::string testDirectory = projectRoot + "/src/filesystem/testData";
const std::string invalidPath = "/invalid";
}

class FileSystemServiceTest : public Test
{
public:
};

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnContentOfFile)
{
    const std::string expectedFileContent = "example data\n";

    const auto actualFileContent = FileSystemService::read(testDirectory + "/testReading.txt");

    ASSERT_EQ(actualFileContent, expectedFileContent);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldThrowException)
{
    ASSERT_THROW(FileSystemService::read(invalidPath), std::runtime_error);
}

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnTrue)
{
    const auto exists = FileSystemService::exists(testDirectory);

    ASSERT_TRUE(exists);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldReturnFalse)
{
    const auto exists = FileSystemService::exists(invalidPath);

    ASSERT_FALSE(exists);
}

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnListOfFiles)
{
    const auto actualFileList = FileSystemService::listFiles(testDirectory);

    ASSERT_EQ(actualFileList.size(), 3);

    ASSERT_EQ(std::any_of(actualFileList.begin(), actualFileList.end(),
                          [](const std::string& path) { return path == testDirectory + "/testReading.txt"; }),
              true);
    ASSERT_EQ(std::any_of(actualFileList.begin(), actualFileList.end(),
                          [](const std::string& path) { return path == testDirectory + "/testListing1.txt"; }),
              true);
    ASSERT_EQ(std::any_of(actualFileList.begin(), actualFileList.end(),
                          [](const std::string& path) { return path == testDirectory + "/testListing2.txt"; }),
              true);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldThrowExceptionWhenListingFiles)
{
    ASSERT_THROW(FileSystemService::listFiles(invalidPath), std::runtime_error);
}
