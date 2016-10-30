
#include <string>
#include <vector>

#include "ConfigTest.h"
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
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\test.txt")).c_str(),
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\义耱\\test.txt")).c_str(),
            FALSE
        );
        CopyFile(
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\57564efb.url")).c_str(),
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\义耱\\57564efb.url")).c_str(),
            FALSE
        );
        CopyFile(
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\14732139\\57584c98.url")).c_str(),
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\义耱\\57584c98.url")).c_str(),
            FALSE
        );
        CopyFile(
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\14732139\\57584dec.url")).c_str(),
            (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\义耱\\57584dec.url")).c_str(),
            FALSE
        );
        return new FileListReaderTest(Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\义耱\\test.txt"));
    }
}
