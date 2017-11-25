#pragma once

#include <string>
#include <vector>

#include "..\TextFileReader.h"

namespace Bookmarks
{
    class TextFileReaderTest : public TextFileReader
    {
    private:
        std::wstring FileName;

    public:
        virtual std::vector<std::wstring> ReadFileLines();

        TextFileReaderTest(std::wstring fileName) : FileName(fileName) {}
    };
}
