#pragma once

#include <string>
#include <vector>

#include "../main.h"
#include "TextFileReader.h"
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
        time_t DateTime;
        //  File name without dir name.
        std::wstring _fileName;
        bool IsFolder;
        size_t Size;
        //  https://action.mindjet.com/task/14732139
        std::wstring Url;
        std::wstring Name;

        File(std::wstring fileName, bool isFolder, time_t dateTime, size_t size, std::wstring url, std::wstring name) :
            DateTime(dateTime), _fileName(fileName), IsFolder(isFolder), Size(size), Url(url), Name(name)
        {
            _seqNum = _seqNumCounter++;
        }

        File()
        {
            _seqNum = _seqNumCounter++;
            IsFolder = false;
            Size = 0;
            DateTime = 0;
        }

        //  https://action.mindjet.com/task/14732139
        void SetDeleteMark() { _deleted = true; }
        bool GetDeleteMark() { return _deleted; }
        int GetSeqNum() const { return _seqNum; }
    };

    typedef std::vector<File> FileVector;

    class FileList
    {
    private:
        TextFileReader *_reader;
        FileVector _fileList;

        FileVector ReadFileLines();
        void RemoveNonInformativeLines(std::vector<std::wstring> &lines);
        void ParseLine(std::wstring &line, FileVector &result);

        static std::vector<std::wstring> SplitLine(std::wstring &line);
        static std::time_t ParseDateTime(std::vector<std::wstring> lineColumns);
        static std::wstring TrimSpaces(std::wstring);
        //  Restores file or folder name as divided by spaces from the rest of columns.
        static std::wstring ConcatFileName(std::vector<std::wstring> lineColumns, int firstColumnIndex);

    public:
        FileList(TextFileReader *reader) : _reader(reader) {
            _fileList = ReadFileLines();
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
