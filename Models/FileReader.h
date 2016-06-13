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

        //  ������ ������ ��������� �� �����
        std::wstring FileReader::GetParam(std::wstring fileName/*������ ���� � �����*/, std::wstring paramName);
        //  ������ ������ ��������� �� ����� ���. ��������
        std::wstring FileReader::GetParamCurDir(std::wstring fileName/*���� � ����� ���. cwd*/, std::wstring paramName);
    };
}
