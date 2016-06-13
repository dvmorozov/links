#pragma once

#include "..\main.h"

#include <string>

namespace Bookmarks
{
    class FileReader
    {
    private:
        std::wstring CurDir;

    public:
        FileReader(std::wstring curDir) : CurDir(curDir) {};
        ~FileReader() {};

        //  читает строку параметра из файла
        std::wstring FileReader::GetParam(std::wstring fileName/*полный путь к файлу*/, std::wstring paramName);
        //  читает строку паарметра из файла тек. каталога
        std::wstring FileReader::GetParamCurDir(std::wstring fileName/*путь к файлу отн. cwd*/, std::wstring paramName);
    };
}
