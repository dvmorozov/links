
#include <string>
#include <vector>

#include "FileListReaderDir.h"
#include "..\Controllers\Commands.h"

namespace Bookmarks
{
    std::vector<std::wstring> FileListReaderDir::GetFileList()
    {
        std::vector<std::wstring> result;
        //  ��������� ����� ���������� �����.
        //  ����� ��������� ������ �.�. ����������� �� ��� ������, �����
        //  ����� ������������ �������� ��������� ����������� ������ ���������.
        //  https://action.mindjet.com/task/14719680

        //  ������ ����� ��. �����.
        std::wstring temp = DocumentRoot.substr(0, DocumentRoot.rfind('/')) + _tmpDir + _T("tempXXXXXX");
#ifndef LINUX
        std::replace(temp.begin(), temp.end(), '/', '\\');
#endif

        //  ��� ����� �. ��������� ������ ����, ����� ����. ����� ��������� ���. �����.
        TCHAR* temp_fn = (TCHAR*)malloc((temp.size() + 1) * sizeof(TCHAR));
        _tcscpy(temp_fn, temp.c_str());
        if (NULL == _tmktemp(temp_fn))
        {
            error = E_SYS_UTILITY;
            return result;
        }

        temp = temp_fn;
        free(temp_fn);
        //  ����� ������� ��� ��������� ����������� ��������.
        std::wstring utility_name = _cmdDir;
        utility_name += _T(" \"") + temp + _T("\"");

        if (-1 == _tsystem(utility_name.c_str()))
        {
            error = E_SYS_UTILITY;
            return result;
        }

        result = ReadFile(temp);
        return result;
    }

    FileListReader *GetFileReader()
    {
        return new FileListReaderDir();
    }
}
