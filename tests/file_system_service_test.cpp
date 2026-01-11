#include "file_system_service.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config::filesystem;

namespace
{
const auto projectRootPath = FileSystemService::getExecutablePath();
const auto testDirectoryPath = projectRootPath.parent_path() / "tests";
const auto filesystemTestDirectoryPath = testDirectoryPath / "filesystemData";
const auto testReadingFilePath = filesystemTestDirectoryPath / "testReading.txt";
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

TEST_F(FileSystemServiceTest, givenDirectoryPath_shouldReturnTrue)
{
    const auto isDirectory = FileSystemService::isDirectory(testDirectoryPath);

    ASSERT_TRUE(isDirectory);
}

TEST_F(FileSystemServiceTest, givenInvalidPath_shouldReturnFalse)
{
    const auto isDirectory = FileSystemService::isDirectory(invalidPath);

    ASSERT_FALSE(isDirectory);
}

TEST_F(FileSystemServiceTest, shouldReturnCurrentWorkingDirectory)
{
    const auto currentWorkingDirectory = FileSystemService::getCurrentWorkingDirectory();

    ASSERT_EQ(currentWorkingDirectory, std::filesystem::current_path());
}
