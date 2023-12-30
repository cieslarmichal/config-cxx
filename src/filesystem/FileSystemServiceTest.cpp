#include "FileSystemService.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

#include "ExecutableFinder.h"

using namespace ::testing;
using namespace config::filesystem;

namespace
{
const auto projectRootPath = ExecutableFinder::getExecutablePath();
const auto testDirectoryPath = projectRootPath.parent_path() / "tests";
const auto filesystemTestDirectoryPath = testDirectoryPath / "filesystemData";
const auto testReadingFilePath = filesystemTestDirectoryPath / "testReading.txt";
const auto testListingFilePath = filesystemTestDirectoryPath / "testListing.txt";
const std::filesystem::path invalidPath = "invalid";
}

class FileSystemServiceTest : public Test
{
public:
    void SetUp() override
    {
        std::filesystem::remove_all(testDirectoryPath);

        std::filesystem::create_directories(filesystemTestDirectoryPath);

        std::ofstream testReadingFile{testReadingFilePath};

        testReadingFile << "example data";

        std::ofstream testListingFile{testListingFilePath};

        testListingFile << "data";
    }

    void TearDown() override
    {
        std::filesystem::remove_all(testDirectoryPath);
    }
};

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnContentOfFile)
{
    const std::string expectedFileContent = "example data";

    const auto actualFileContent = FileSystemService::read(testReadingFilePath);

    ASSERT_EQ(actualFileContent, expectedFileContent);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldThrowException)
{
    ASSERT_THROW(FileSystemService::read(invalidPath), std::runtime_error);
}

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnTrue)
{
    const auto exists = FileSystemService::exists(testDirectoryPath);

    ASSERT_TRUE(exists);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldReturnFalse)
{
    const auto exists = FileSystemService::exists(invalidPath);

    ASSERT_FALSE(exists);
}

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnListOfFiles)
{
    const auto actualFileList = FileSystemService::listFiles(filesystemTestDirectoryPath);

    ASSERT_EQ(actualFileList.size(), 2);

    ASSERT_EQ(std::any_of(actualFileList.begin(), actualFileList.end(),
                          [](const std::string& path) { return path == testReadingFilePath; }),
              true);
    ASSERT_EQ(std::any_of(actualFileList.begin(), actualFileList.end(),
                          [](const std::string& path) { return path == testListingFilePath; }),
              true);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldThrowExceptionWhenListingFiles)
{
    ASSERT_THROW(FileSystemService::listFiles(invalidPath), std::runtime_error);
}
