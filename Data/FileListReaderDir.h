#pragma once

#include <string>
#include <vector>

#include "FileListReader.h"
#include "..\main.h"

namespace Bookmarks
{
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
        std::string cmd_dir = _T("dir /OG >>");                             //  ����� ��� Windows
#else
        std::string cmd_dir = _T("cygdir.exe -a -1 -X -F -N >>");           //  ����� ��� Cygwin
#endif  //  USE_CYGWIN

        std::string tmpdir = _T("/tmp/links/");
#else
        std::string cmd_dir = _T("dir -a -1 -X -F -N >>");                  //  ����� ��� Linux

                                                                            //std::string tmpdir = _T("/var/tmp/");                             //  ����� ��������� ��������� ������ � ����� � �������,
                                                                            //  ��� ������������ ��������� ����������
        std::string tmpdir = _T("/tmp/links/");
#endif
    public:
        virtual std::vector<std::string> GetFileList();
    };
}
