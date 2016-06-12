
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

            std::vector<Bookmarks::File> ReadFileList();
        };

        FileList *Files;

    public:
        std::vector<Bookmarks::File> ReadFileList();

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
