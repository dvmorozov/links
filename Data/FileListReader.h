#pragma once

#include <string>
#include <vector>

namespace Bookmarks
{
    class FileListReader
    {
    protected:
        std::vector<std::string> ReadFile(std::string fileName);

    public:
        virtual std::vector<std::string> GetFileList() = 0;
    };

    FileListReader *GetFileReader();
}
