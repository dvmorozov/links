
#include <string>
#include <vector>

#include "FileListReaderTest.h"

namespace Bookmarks
{
    std::vector<std::string> FileListReaderTest::GetFileList()
    {
        return ReadFile(FileName);
    }
}
