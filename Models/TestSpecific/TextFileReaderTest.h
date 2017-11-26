#pragma once

#include <string>
#include <vector>

#include "..\TextFileReader.h"

namespace Bookmarks
{
    class TextFileReaderTest : public TextFileReader
    {
    public:
        virtual std::vector<std::wstring> ReadFileLines();
    };
}
