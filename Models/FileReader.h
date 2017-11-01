#pragma once

#include "..\main.h"

#include <string>

namespace Bookmarks
{
    class FileReader
    {
    private:
        std::wstring _curDir;

    public:
        FileReader() {}
        FileReader(std::wstring curDir) : _curDir(curDir) {}
        ~FileReader() {}

        //  Read parameter from file.
        static std::wstring GetParam(std::wstring fileName  /* absolute path to the file */, std::wstring paramName);
        //  Read parameter from file from current directory.
        std::wstring GetParamCurDir(std::wstring fileName   /* path to the file relative to cwd*/, std::wstring paramName);
    };
}
