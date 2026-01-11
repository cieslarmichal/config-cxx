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

TEST_F(FileSystemServiceTest, givenFilePath_shouldReturnFalseForIsDirectory)
{
    const auto isDirectory = FileSystemService::isDirectory(testReadingFilePath);

    ASSERT_FALSE(isDirectory);
}

TEST_F(FileSystemServiceTest, getSystemRootPath_returnsValidPath)
{
    const auto rootPath = FileSystemService::getSystemRootPath();

    ASSERT_FALSE(rootPath.empty());
#ifdef _WIN32
    ASSERT_TRUE(rootPath.string().ends_with(":\\"));
#else
    ASSERT_EQ(rootPath.string(), "/");
#endif
}

TEST_F(FileSystemServiceTest, isRelative_withRelativePath_returnsTrue)
{
    const std::filesystem::path relativePath = "config/test.json";
    
    ASSERT_TRUE(FileSystemService::isRelative(relativePath));
}

TEST_F(FileSystemServiceTest, isRelative_withAbsolutePath_returnsFalse)
{
    const auto absolutePath = std::filesystem::absolute("config");
    
    ASSERT_FALSE(FileSystemService::isRelative(absolutePath));
}

TEST_F(FileSystemServiceTest, isRelative_withCurrentDirectory_returnsTrue)
{
    const std::filesystem::path currentDir = ".";
    
    ASSERT_TRUE(FileSystemService::isRelative(currentDir));
}

TEST_F(FileSystemServiceTest, isRelative_withParentDirectory_returnsTrue)
{
    const std::filesystem::path parentDir = "..";
    
    ASSERT_TRUE(FileSystemService::isRelative(parentDir));
}

TEST_F(FileSystemServiceTest, isRelative_withRootPath_returnsFalse)
{
    const auto rootPath = FileSystemService::getSystemRootPath();
    
    ASSERT_FALSE(FileSystemService::isRelative(rootPath));
}
