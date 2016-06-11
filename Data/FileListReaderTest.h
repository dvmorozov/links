#pragma once

#include <string>
#include <vector>

#include "FileListReader.h"

namespace Bookmarks
{
    class FileListReaderTest : public FileListReader
    {
    private:
        std::string FileName;

    public:
        virtual std::vector<std::string> GetFileList();

        FileListReaderTest(std::string fileName) : FileName(fileName) {}
    };
}
