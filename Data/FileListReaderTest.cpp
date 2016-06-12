
#include <string>
#include <vector>

#include "FileListReaderTest.h"

namespace Bookmarks
{
    std::vector<std::string> FileListReaderTest::GetFileList()
    {
        return ReadFile(FileName);
    }

    FileListReader *GetFileReader()
    {
        return new FileListReaderTest("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\test.txt");
    }
}
