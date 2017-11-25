#pragma once

#include <string>
#include <vector>

namespace Bookmarks
{
    class TextFileReader
    {
    protected:
        std::vector<std::wstring> ReadFile(std::wstring fileName);

    public:
        virtual std::vector<std::wstring> ReadFileLines() = 0;
        virtual ~TextFileReader() {}
    };

    TextFileReader *GetFileReader();
}
