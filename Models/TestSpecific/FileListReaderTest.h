#pragma once

#include <string>
#include <vector>

#include "..\FileListReader.h"

namespace Bookmarks
{
    class FileListReaderTest : public FileListReader
    {
    private:
        std::wstring FileName;

    public:
        virtual std::vector<std::wstring> ReadFileList();

        FileListReaderTest(std::wstring fileName) : FileName(fileName) {}
    };
}
