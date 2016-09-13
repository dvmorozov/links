#pragma once

#include "FileList.h"

namespace Bookmarks
{
    class Data
    {
    private:
        FileList *_fileList;

    public:
        FileVector GetFileList();
        FileVector GetDirList();

        Data()
        {
            _fileList = new FileList(GetFileReader());
        }

        ~Data()
        {
            if (_fileList) 
                delete _fileList;
        }
    };
}
