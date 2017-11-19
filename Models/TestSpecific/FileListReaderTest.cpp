
#include <string>
#include <vector>

#include "FileListReaderTest.h"
#include "..\..\Models\Config.h"
#include "..\..\Models\ConfigFactory.h"
#include "..\..\main.h"

#include <memory>

namespace Bookmarks
{
    std::vector<std::wstring> FileListReaderTest::ReadFileList()
    {
        return ReadFile(FileName);
    }

    FileListReader *GetFileReader()
    {
        std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
        //  File copying must be here because this function is called many times during the test.
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\dir_windows.txt")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Тест\\dir_windows.txt")).c_str(),
            FALSE
        );
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\57564efb.url")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Тест\\57564efb.url")).c_str(),
            FALSE
        );
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\14732139\\57584c98.url")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Тест\\57584c98.url")).c_str(),
            FALSE
        );
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\14732139\\57584dec.url")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Тест\\57584dec.url")).c_str(),
            FALSE
        );
        return new FileListReaderTest(config->GetValue(_T("TestFolder")) + _T("\\Тест\\dir_windows.txt"));
    }
}
