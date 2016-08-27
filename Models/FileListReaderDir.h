#pragma once

#include <string>
#include <vector>

#include "FileListReader.h"
#include "..\main.h"

namespace Bookmarks
{
    //  ������ ���������� ������� ���������� (������� ��� ���. �� ����������) � ���. ������ ����. ����.
    class FileListReaderDir : public FileListReader
    {
    private:
        //-------------------------------------------------------------------------------------------------
        //  ���� ���������� � Ansi-����. � ���. VC, �� ��� �������� ���������
        //  ���� ���������� � Unicode-����. � ���. VC, �� ������� ����� ��������� + ����. ����� ��������� �����
        //  ���� ������� � cygwin ����. �� ����� �������� ���������, �� ����� ���������� �� �������� �
        //  �����-�� ���� ���������
#if !defined LINUX
#ifndef USE_CYGWIN
        //  https://action.mindjet.com/task/14665015
        std::wstring _cmdDir = _T("dir /OG >>");                             //  ����� ��� Windows
#else
        std::wstring _cmdDir = _T("cygdir.exe -a -1 -X -F -N >>");           //  ����� ��� Cygwin
#endif  //  USE_CYGWIN

        std::wstring _tmpDir = _T("/tmp/links/");
#else
        std::wstring _cmdDir = _T("dir -a -1 -X -F -N >>");                  //  ����� ��� Linux

                                                                            //std::wstring tmpdir = _T("/var/tmp/");                             //  ����� ��������� ��������� ������ � ����� � �������,
                                                                            //  ��� ������������ ��������� ����������
        std::wstring _tmpDir = _T("/tmp/links/");
#endif
    public:
        virtual std::vector<std::wstring> GetFileList();
    };
}
