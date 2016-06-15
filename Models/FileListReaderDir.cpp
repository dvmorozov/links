
#include <string>
#include <vector>

#include "FileListReaderDir.h"

namespace Bookmarks
{
    std::vector<std::wstring> FileListReaderDir::GetFileList()
    {
        //  ���������� ������ ����� �����-������
        std::vector<std::wstring> lines;

        //  ��������� ����� ���������� �����
        //  !!! ����� ��������� ������ �.�. ����������� �� ��� ������, �����
        //  ����� ������������ �������� ��������� ����������� ������ ��������� !!!
        //  ��������� �������� ���������
        TCHAR temp_fn[11];
        _tcscpy(temp_fn, _T("tempXXXXXX"));
        _tmktemp(temp_fn);

        std::wstring temp = document_root.substr(0, document_root.rfind('/')) + _tmpDir + temp_fn;
#ifndef LINUX
        std::replace(temp.begin(), temp.end(), '/', '\\');
#endif
        std::wstring utility_name = _cmdDir;
        utility_name += _T(" \"") + temp + _T("\"");

        if (-1 == _tsystem(utility_name.c_str()))
        {
            error = E_SYS_UTILITY;
            //  ������ �� �������� - ������ �� ���������
        }
        else
            lines = ReadFile(temp);

        /*  ���� ����� �� ��������� �������� �������
        �������� ����������, ����� ����� ����� (����������
        �������� �������� ������ �� �������� ����������);
        ���� � �������������� POSIX ������������� ��� ������
        DIR *dp;
        struct dirent *ep;

        dp = opendir (_T("./"));
        if (dp != NULL)
        {
        while (ep = readdir (dp))
        {
        _tprintf(_T("%s\n"), ep->d_name);
        }
        (void) closedir (dp);
        }
        else
        puts(_T("Couldn't open the directory."));
        */
        return lines;
    }

    FileListReader *GetFileReader()
    {
        return new FileListReaderDir();
    }
}
