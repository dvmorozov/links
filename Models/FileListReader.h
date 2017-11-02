#pragma once

#include <string>
#include <vector>

namespace Bookmarks
{
    class FileListReader
    {
    protected:
        std::vector<std::wstring> ReadFile(std::wstring fileName);

    public:
        virtual std::vector<std::wstring> ReadFileList() = 0;
        virtual ~FileListReader() {}
    };

    FileListReader *GetFileReader();
}
