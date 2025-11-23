#pragma once

#include <string>
#include <vector>

#include "Shared/Models/TextFileReader.h"

namespace Bookmarks
{
    class TextFileReaderTest : public TextFileReader
    {
    public:
        virtual std::vector<std::wstring> ReadFileLines();
    };
}
