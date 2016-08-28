
#pragma once

#include <string>
#include <vector>

#include "../main.h"
#include "FileListReader.h"
#include "FileListReaderDir.h"

namespace Bookmarks
{
    class File
    {
    public:
        //  https://action.mindjet.com/task/14665015
        time_t DateTime;
        std::wstring Name;
        bool IsFolder;
        size_t Size;

        File(std::wstring name, bool isFolder, time_t dateTime, size_t size) : Name(name), IsFolder(isFolder), DateTime(dateTime), Size(size) {}
        File() { }
    };

    typedef std::vector<File> FileVector;

    class FileList
    {
    private:
        FileListReader *_reader;
        FileVector _files;

        FileVector ReadFileList();

    public:
        FileList(FileListReader *reader) : _reader(reader) {
            _files = ReadFileList();
        };
        ~FileList()
        {
            if (_reader) delete _reader;
        };

        //  https://action.mindjet.com/task/14726166
        FileVector GetFileList();

        //  https://action.mindjet.com/task/14726166
        FileVector GetDirList();
    };

    class Data
    {
    private:
        FileList *_files;

    public:
        FileVector GetFileList();
        FileVector GetDirList();

        Data()
        {
            _files = new FileList(GetFileReader());
        }

        ~Data()
        {
            if (_files) 
                delete _files;
        }
    };
}
