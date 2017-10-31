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
        FileReader() {};
        FileReader(std::wstring curDir) : _curDir(curDir) {};
        ~FileReader() {};

        //  Читает строку параметра из файла.
        static std::wstring GetParam(std::wstring fileName/*полный путь к файлу*/, std::wstring paramName);
        //  Читает строку паарметра из файла тек. каталога.
        std::wstring GetParamCurDir(std::wstring fileName/*путь к файлу отн. cwd*/, std::wstring paramName);
    };
}
