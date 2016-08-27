
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
    };

    typedef std::vector<File> FileVector;

    class Data
    {
    private:
        class FileList
        {
        private:
            FileListReader *Reader;

        public:
            FileList(FileListReader *reader) : Reader(reader) {};
            ~FileList() 
            { 
                if (Reader) delete Reader;  
            };

            FileVector ReadFileList();
        };

        FileList *Files;

    public:
        FileVector ReadFileList();

        Data()
        {
            Files = new FileList(GetFileReader());
        }

        ~Data()
        {
            if (Files) 
                delete Files;
        }
    };
}
