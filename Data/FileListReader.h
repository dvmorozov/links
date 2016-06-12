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
        virtual std::vector<std::wstring> GetFileList() = 0;
    };

    FileListReader *GetFileReader();
}
