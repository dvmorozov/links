
#include "FileList.h"
#include "FileReader.h"
#include "../Controllers/Commands.h"
#include "../Utils/entities.h"
#include "../main.h"

namespace Bookmarks
{
    void FileList::RemoveNonInformativeLines(std::vector<std::wstring> &lines)
    {
#ifndef LINUX
        assert(lines.size() >= 7);
        //  5 первых строк нужно пропустить
        lines.erase(lines.begin(), lines.begin() + 5);
        //  для Windows 2 последние строки содержат доп. инф., поэтому удаляются
        lines.pop_back();
        lines.pop_back();
#else
        assert(lines.size() >= 1);
        //  The first string contains "total" and must be removed.
        lines.erase(lines.begin(), lines.begin() + 1);
#endif // !LINUX
    }

    std::vector<std::wstring> FileList::SplitLine(std::wstring &line)
    {
        std::vector<std::wstring> columns;
        std::wstringstream ss(line);
        std::wstring column;
        while (ss >> column)
            columns.push_back(column);
        return columns;
    }

    std::time_t FileList::ParseDateTime(std::vector<std::wstring> lineColumns)
    {
        struct std::tm tm;

#ifdef LINUX
        std::wstringstream dateTimeStr(lineColumns[5] + _T(" ") + lineColumns[6]);
        dateTimeStr >> std::get_time(&tm, _T("%Y-%m-%d %H:%M"));
#else
        std::wstring dateTimeStr(lineColumns[0] + _T(" ") + lineColumns[1]);
        int day, month, year, hour, min;
        //  Date has following format "%d.%m.%Y %H:%M".
        swscanf(dateTimeStr.c_str(), _T("%2d.%2d.%4d %2d:%2d"), &day, &month, &year, &hour, &min);
        tm.tm_sec = 0;
        tm.tm_min = 0;
        tm.tm_hour = hour;
        tm.tm_mon = month - 1;
        tm.tm_year = year - 1900;
        tm.tm_mday = 0;
        tm.tm_wday = 0;
        tm.tm_yday = 0;
        tm.tm_isdst = 0;
#endif
        std::time_t dateTime = mktime(&tm);
        return dateTime;
    }

    std::wstring FileList::TrimSpaces(std::wstring str)
    {
        str.erase(0, str.find_first_not_of(' '));         //  Removes prefixing spaces.
        str.erase(str.find_last_not_of(' ') + 1);         //  Removes surfixing spaces.
        return str;
    }

    std::wstring FileList::ConcatFileName(std::vector<std::wstring> lineColumns, int firstColumnIndex)
    {
        std::wstring fileName = lineColumns[firstColumnIndex];
        std::for_each(lineColumns.begin() + firstColumnIndex + 1, lineColumns.end(),
                      [&](std::wstring &s) { fileName += _T(" ") + s; });
        return fileName;
    }

    void FileList::ParseLine(std::wstring &line, FileVector &result)
    {
        //  Line terminating symbol is removed.
        line.pop_back();
        //  https://action.mindjet.com/task/14665015
        std::vector<std::wstring> columns = SplitLine(line);
        std::time_t dateTime = ParseDateTime(columns);

#ifndef LINUX
        std::wstring col2 = TrimSpaces(columns[2]);
        bool isFolder = col2 == _T("<DIR>");

        size_t size = col2 != _T("<DIR>") ? std::stoi(col2) : 0;

        std::wstring fileName = ConcatFileName(columns, 3);

#else
        bool isFolder = columns[0][0] == 'd';

        size_t size;
        swscanf(TrimSpaces(columns[4]).c_str(), _T("%zd"), &size);

        std::wstring fileName = ConcatFileName(columns, 7);
#endif // !LINUX
        if (!isFolder)
        {
            //  Read file content.
            FileReader fr;
            std::wstring url = fr.GetParam(fileName, ParamURL);
            std::wstring name = fr.GetParam(fileName, ParamName);

            result.push_back(File(fileName, isFolder, dateTime, size, url, name));
        }
        else
            result.push_back(File(fileName, isFolder, dateTime, size, _T(""), _T("")));
    }

    FileVector FileList::ReadFileList()
    {
        FileVector result;

        assert(_reader);
        std::vector<std::wstring> lines = _reader->ReadFileList();

        RemoveNonInformativeLines(lines);

        for (auto l = lines.begin(); l != lines.end(); ++l)
            ParseLine(*l, result);

        //  Sorts the list (folders at the beginning).
        //  https://action.mindjet.com/task/14640967
        std::sort(result.begin(), result.end(), [](File a, File b) {
            std::transform(a.FileName.begin(), a.FileName.end(), a.FileName.begin(), (int(*)(int))std::tolower);
            std::transform(b.FileName.begin(), b.FileName.end(), b.FileName.begin(), (int(*)(int))std::tolower);
            if (a.IsFolder != b.IsFolder)
                return a.IsFolder && !b.IsFolder;
            else
                return a.FileName < b.FileName;
        });
        return result;
    }

    //  https://action.mindjet.com/task/14817423
    bool FileList::HasExtension(std::wstring fileName, std::wstring ext)
    {
        assert(fileName.size() >= ext.size());
        return (!fileName.compare(fileName.size() - ext.size(), ext.size(), ext));
    }

    //  https://action.mindjet.com/task/14726166
    FileVector FileList::GetFileList()
    {
        FileVector result(_fileList.size());

        //  Copies only files with given extensions.
        auto it = std::copy_if(_fileList.begin(), _fileList.end(), result.begin(), [this](File &f)
        {
            auto nameLCase = f.FileName;
            //  https://action.mindjet.com/task/14817423
            const std::wstring supportedExts[] = { _T(".url"), ExtUtf8 };
            std::transform(nameLCase.begin(), nameLCase.end(), nameLCase.begin(), ::tolower);

            auto isSupported = false;
            if (!f.IsFolder)
            {
                //  Checks file type.
                std::for_each(std::begin(supportedExts), std::end(supportedExts), [nameLCase, &isSupported, this](const std::wstring ext) mutable
                {
                    if (HasExtension(nameLCase, ext))
                        isSupported = true;
                });
            }
            return isSupported;
        });
        result.resize(std::distance(result.begin(), it));   //  Shrink container to new size.

        return result;
    }

    //  https://action.mindjet.com/task/14726166
    FileVector FileList::GetDirList()
    {
        FileVector result(_fileList.size());

        //  Copies only directories.
        auto it = std::copy_if(_fileList.begin(), _fileList.end(), result.begin(), [](File &f) {return f.IsFolder;});
        result.resize(std::distance(result.begin(), it));   //  Shrink container to new size.

        return result;
    }

    //  https://action.mindjet.com/task/14732139
    int File::_seqNumCounter = 1;

    //  https://action.mindjet.com/task/14732139
    //  As usual, the method works with files inside current catalog.
    void FileList::DeleteDuplicates()
    {
        FileVector fileList = GetFileList();
        //  Sort files by URL.
        std::sort(fileList.begin(), fileList.end(), [](File a, File b) {
            return a.Url < b.Url;
        });

        //  Mark duplicates as deleted.
        for (auto it = fileList.begin(); it != fileList.end(); it++)
        {
            if (it != fileList.begin())
            {
                if (it->Url == (it - 1)->Url)
                    it->SetDeleteMark();
            }
        }

        //  Delete duplicates.
        for (auto it = fileList.begin(); it != fileList.end(); it++)
        {
            if (it->GetDeleteMark())
            {
                _wremove(it->FileName.c_str());
                auto seqNum = it->GetSeqNum();
                //  Removes data from main file list.
                auto erased = std::find_if(_fileList.begin(), _fileList.end(), [seqNum](const File &f) {
                    return f.GetSeqNum() == seqNum; });
                if (erased != _fileList.end())
                    _fileList.erase(erased);
            }
        }
    }
}
