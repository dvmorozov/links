
#include <string>
#include <vector>

#include "FileListReaderTest.h"
#include "..\main.h"

namespace Bookmarks
{
    std::vector<std::wstring> FileListReaderTest::ReadFileList()
    {
        return ReadFile(FileName);
    }

    FileListReader *GetFileReader()
    {
        //  The file is copied by postbuild script.
        return new FileListReaderTest(_T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\test.txt"));
    }
}
