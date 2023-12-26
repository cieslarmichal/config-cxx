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
const std::filesystem::path testDirectoryPath = projectRootPath / "tests";
const std::filesystem::path filesystemTestDirectoryPath = testDirectoryPath / "filesystemData";
const std::filesystem::path testReadingFilePath = filesystemTestDirectoryPath / "testReading.txt";
const std::string invalidPath = "invalid";
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
