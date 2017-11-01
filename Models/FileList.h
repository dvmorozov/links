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
    private:
        //  https://action.mindjet.com/task/14732139
        //  Deletion marker.
        bool _deleted = false;
        int _seqNum = 0;
        static int _seqNumCounter;

    public:
        //  https://action.mindjet.com/task/14665015
        time_t DateTime = 0;
        //  File name without dir name.
        std::wstring FileName;
        bool IsFolder = false;
        size_t Size = 0;
        //  https://action.mindjet.com/task/14732139
        std::wstring Url;
        std::wstring Name;

        File(std::wstring fileName, bool isFolder, time_t dateTime, size_t size, std::wstring url, std::wstring name) :
            FileName(fileName), IsFolder(isFolder), DateTime(dateTime), Size(size), Url(url), Name(name) {
            _seqNum = _seqNumCounter++;
        }
        File() { _seqNum = _seqNumCounter++; }

        //  https://action.mindjet.com/task/14732139
        void SetDeleteMark() { _deleted = true; }
        bool GetDeleteMark() { return _deleted; }
        int GetSeqNum() const { return _seqNum; }
    };

    typedef std::vector<File> FileVector;

    class FileList
    {
    private:
        FileListReader *_reader;
        FileVector _fileList;

        FileVector ReadFileList();
        void RemoveNonInformativeLines(std::vector<std::wstring> &lines);
        void ParseLine(std::wstring &line, FileVector &result);

    public:
        FileList(FileListReader *reader) : _reader(reader) {
            _fileList = ReadFileList();
        }
        ~FileList()
        {
            if (_reader) delete _reader;
        }

        //  https://action.mindjet.com/task/14726166
        FileVector GetFileList();

        //  https://action.mindjet.com/task/14726166
        FileVector GetDirList();

        //  https://action.mindjet.com/task/14732139
        void DeleteDuplicates();

        //  https://action.mindjet.com/task/14817423
        static bool HasExtension(std::wstring fileName, std::wstring ext);
    };
}
