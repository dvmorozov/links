
#include "TextFileReaderTest.h"
#include "Shared/Models/Config.h"
#include "Shared/Models/ConfigFactory.h"
#include "App/main.h"

#include <string>
#include <vector>
#include <memory>

namespace Bookmarks
{
    std::vector<std::wstring> TextFileReaderTest::ReadFileLines()
    {
        return ReadFile();
    }

    //  Factory method returning FileListReader.
    TextFileReader *GetFileReader()
    {
        std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
        //  File copying must be here because this function is called many times during the test.
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\dir_windows.txt")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Test\\dir_windows.txt")).c_str(),
            FALSE
        );
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\57564efb.url")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Test\\57564efb.url")).c_str(),
            FALSE
        );
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\14732139\\57584c98.url")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Test\\57584c98.url")).c_str(),
            FALSE
        );
        CopyFile(
            (config->GetValue(_T("TestFolder")) + _T("\\14732139\\57584dec.url")).c_str(),
            (config->GetValue(_T("TestFolder")) + _T("\\Test\\57584dec.url")).c_str(),
            FALSE
        );
        auto reader = new TextFileReaderTest();
        reader->SetFileName(config->GetValue(_T("TestFolder")) + _T("\\Test\\dir_windows.txt"));
        return reader;
    }
}
