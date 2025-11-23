
#pragma once
#include "App/Globals.h"
#include "Shared/Models/FileListReaderDir.h"
#include "Shared/Models/TextFileReader.h"

namespace Bookmarks
{
class File
{
private:
    //  Deletion marker.
    bool       _deleted = false;
    int        _seqNum  = 0;
    static int _seqNumCounter;

public:
    time_t DateTime;
    //  File name without dir name.
    std::wstring FileName;
    bool         IsFolder;
    size_t       Size;
    std::wstring Url;
    std::wstring Name;

    File(std::wstring fileName, bool isFolder, time_t dateTime, size_t size, std::wstring url,
         std::wstring name)
        : DateTime(dateTime), FileName(fileName), IsFolder(isFolder), Size(size), Url(url),
          Name(name)
    {
        _seqNum = _seqNumCounter++;
    }

    File()
    {
        _seqNum  = _seqNumCounter++;
        IsFolder = false;
        Size     = 0;
        DateTime = 0;
    }

    void SetDeleteMark() { _deleted = true; }
    bool GetDeleteMark() { return _deleted; }
    int  GetSeqNum() const { return _seqNum; }
};

typedef std::vector<File> FileVector;

class FileList
{
public:
    FileList(TextFileReader *reader) : _reader(reader) { ReadFileLines(); }
    ~FileList()
    {
        if (_reader)
            delete _reader;
    }

    FileVector  GetFileList();
    FileVector  GetDirList();
    void        DeleteFilesWithDuplicatedUrls();
    static bool HasExtension(std::wstring fileName, std::wstring ext);

private:
    void ReadBookmarkParameters(File &file);
    void ReadFileLines();
    void RemoveNonInformativeLines(std::vector<std::wstring> &lines);
    void ParseLine(std::wstring &line, File &result);

    static std::vector<std::wstring> SplitLine(std::wstring &line, size_t fileNameColumnIndex);
    static std::time_t               ParseDateTime(std::vector<std::wstring> lineColumns);
    static std::wstring              TrimSpaces(std::wstring);
    //  Restores file or folder name as divided by spaces from the rest of columns.
    static std::wstring ConcatFileName(std::vector<std::wstring> lineColumns, int firstColumnIndex);

private:
    TextFileReader *_reader;
    FileVector      _fileList;
};
}  // namespace Bookmarks
