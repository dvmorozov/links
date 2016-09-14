
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
        //  File copying must be here because this function is called many times during the test.
        CopyFile(
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\test.txt"),
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\义耱\\test.txt"),
            FALSE
        );
        CopyFile(
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\57564efb.url"),
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\义耱\\57564efb.url"),
            FALSE
        );
        CopyFile(
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\14732139\\57584c98.url"),
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\义耱\\57584c98.url"),
            FALSE
        );
        CopyFile(
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\14732139\\57584dec.url"),
            _T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\义耱\\57584dec.url"),
            FALSE
        );
        return new FileListReaderTest(_T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\义耱\\test.txt"));
    }
}
