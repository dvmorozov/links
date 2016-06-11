
#pragma once

#include <string>
#include <vector>

#include "../main.h"

namespace Bookmarks
{
    class File
    {
    public:
        //  https://action.mindjet.com/task/14665015
        time_t DateTime;
        std::string Name;
        bool IsFolder;
        size_t Size;

        File(std::string name, bool isFolder, time_t dateTime, size_t size) : Name(name), IsFolder(isFolder), DateTime(dateTime), Size(size) {}
    };

    class Data
    {
    private:
        class FileListReader
        {
        protected:
            std::vector<std::string> ReadFile(std::string fileName);

        public:
            virtual std::vector<std::string> GetFileList() = 0;
        };

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

        class FileListReaderTest : public FileListReader
        {
        private:
            std::string Data;

        public:
            virtual std::vector<std::string> GetFileList();

            FileListReaderTest(std::string data) : Data(data) {}
        };

        class FileList
        {
        private:
            FileListReader *Reader;

        public:
            FileList() { Reader = new FileListReaderDir(); };
            FileList(FileListReader *reader) : Reader(reader) {};
            ~FileList() { if (Reader) delete Reader;  };

            std::vector<Bookmarks::File> ReadFileList();
        };

        FileList Files;

    public:
        std::vector<Bookmarks::File> ReadFileList();
    };
}
