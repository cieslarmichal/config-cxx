#include "FileSystemService.h"

#include <format>

#include "gtest/gtest.h"

using namespace ::testing;
using namespace config::filesystem;

namespace
{
const std::string testFilesDirectory{std::format("{}/apps/common/filesystem/testFiles", getProjectPath("chatroom"))};

class FileSystemServiceTest : public Test
{
public:
    FileSystemService fileSystemService;
};

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnContentOfFile)
{
    const auto actualFileContent = fileSystemService.read(pathForReading);

    ASSERT_EQ(actualFileContent, exampleContent);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldThrowException)
{
    ASSERT_THROW(fileSystemService.read(incorrectPath), std::runtime_error);
}

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnTrue)
{
    const auto actualFileExistence = fileSystemService.exists(pathForReading);

    ASSERT_TRUE(actualFileExistence);
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldReturnFalse)
{
    const auto actualFileExistence = fileSystemService.exists(incorrectPath);

    ASSERT_FALSE(actualFileExistence);
}

TEST_F(FileSystemServiceTest, givenCorrectPath_shouldReturnListOfFiles)
{
    const auto actualFileList = fileSystemService.listFiles(testFilesDirectory);

    ASSERT_EQ(actualFileList.size(), 3);
    ASSERT_TRUE(
        std::ranges::any_of(actualFileList, [](const auto& filename) { return filename == filenameForReading; }));
    ASSERT_TRUE(
        std::ranges::any_of(actualFileList, [](const auto& filename) { return filename == filenameForWriting; }));
    ASSERT_TRUE(std::ranges::any_of(actualFileList,
                                    [](const auto& filename) { return filename == filenameForWritingAtPosition; }));
}

TEST_F(FileSystemServiceTest, givenIncorrectPath_shouldThrowExceptionWhenListingFiles)
{
    ASSERT_THROW(fileSystemService.listFiles(incorrectPath), std::runtime_error);
}
