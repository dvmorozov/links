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
        FileReader(std::wstring curDir) : _curDir(curDir) {};
        ~FileReader() {};

        //  ������ ������ ��������� �� �����
        std::wstring FileReader::GetParam(std::wstring fileName/*������ ���� � �����*/, std::wstring paramName);
        //  ������ ������ ��������� �� ����� ���. ��������
        std::wstring FileReader::GetParamCurDir(std::wstring fileName/*���� � ����� ���. cwd*/, std::wstring paramName);
    };
}
