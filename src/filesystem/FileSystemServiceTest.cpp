#include "FileSystemService.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

#include "../tests/ProjectRootFinder.h"

using namespace ::testing;
using namespace config::filesystem;
using namespace config::tests;

namespace
{
const std::filesystem::path projectRootPath = ProjectRootFinder::getProjectRoot();
const std::filesystem::path testDirectoryPath = projectRootPath / "src" / "filesystem" / "testData";
const std::filesystem::path testReadingFilePath = testDirectoryPath / "testReading.txt";
const std::filesystem::path testListingFilePath = testDirectoryPath / "testListing.txt";
const std::string invalidPath = "invalid";
}

class FileSystemServiceTest : public Test
{
public:
    void SetUp() override
    {
        std::filesystem::remove_all(testDirectoryPath);

        std::filesystem::create_directory(testDirectoryPath);

        std::ofstream testReadingFile{testReadingFilePath};

        testReadingFile << "example data";

        std::ofstream testListingFile{testListingFilePath};

        testListingFile << "data";
    }

    void TearDown() override
    {
        if (std::filesystem::exists(testDirectoryPath))
        {
            std::filesystem::remove_all(testDirectoryPath);
        }
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
    const auto actualFileList = FileSystemService::listFiles(testDirectoryPath);

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
