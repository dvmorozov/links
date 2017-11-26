#pragma once

#include <string>
#include <vector>

namespace Bookmarks
{
    class TextFileReader
    {
    protected:
        std::wstring _fileName;
        std::vector<std::wstring> ReadFile();

    public:
        virtual std::vector<std::wstring> ReadFileLines() = 0;
        void SetFileName(std::wstring fileName);

        virtual ~TextFileReader() {}
    };

    TextFileReader *GetFileReader();
}
