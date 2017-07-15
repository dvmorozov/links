
#include "FileList.h"
#include "FileReader.h"
#include "../Controllers/Commands.h"
#include "../Utils/entities.h"

#include <sstream>
#include <locale>
#include <iomanip>
#include <algorithm>
#include <assert.h>

namespace Bookmarks
{
    FileVector FileList::ReadFileList()
    {
        FileVector result;

        assert(_reader);
        std::vector<std::wstring> lines = _reader->ReadFileList();

#ifndef LINUX
        assert(lines.size() >= 7);
        //  5 первых строк нужно пропустить
        lines.erase(lines.begin(), lines.begin() + 5);
        //  дл€ Windows 2 последние строки содержат доп. инф., поэтому удал€ютс€
        lines.pop_back();
        lines.pop_back();
#endif // !LINUX

        for (auto l = lines.begin(); l != lines.end(); ++l)
        {
            std::wstring line = *l;
#ifndef LINUX
            //  удал€етс€ перевод строки (2 симв. на Windows)
            line.pop_back();
            //  https://action.mindjet.com/task/14665015
            //  разбивка строки на набор строк
            std::vector<std::wstring> columns;
            std::wstringstream ss(line);
            std::wstring column;
            while (ss >> column)
                columns.push_back(column);

            //  разбор времени создани€ файла/папки
            struct std::tm tm;
            std::wstringstream dateTimeStr(columns[0] + _T(" ") + columns[1]);
            dateTimeStr >> std::get_time(&tm, _T("%d.%m.%Y %H:%M"));
            std::time_t dateTime = mktime(&tm);

            std::wstring col2 = columns[2];
            col2.erase(0, col2.find_first_not_of(' '));         //  prefixing spaces
            col2.erase(col2.find_last_not_of(' ') + 1);         //  surfixing spaces

            bool isFolder = col2 == _T("<DIR>");

            size_t size = col2 != _T("<DIR>") ? std::stoi(col2) : 0;

            std::wstring fileName = columns[3];
            //  Restore folder name.
            std::for_each(columns.begin() + 4, columns.end(), [&](std::wstring &s) { fileName += _T(" ") + s; });

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
#else
            bool isFolder = (fileName.size() <= 4) || (fileName.rfind('.') != (fileName.size() - 4));
            result.push_back(File(line, isFolder, 0, 0));
#endif // !LINUX
        }

        //  —ортировка списка (папки вперед).
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
        auto it = std::copy_if(_fileList.begin(), _fileList.end(), result.begin(), [](File &f) 
        {
            auto nameLCase = f.FileName;
            //  https://action.mindjet.com/task/14817423
            const std::wstring supportedExts[] = { _T(".url"), ExtUtf8 };
            std::transform(nameLCase.begin(), nameLCase.end(), nameLCase.begin(), ::tolower);

            auto isSupported = false;
            if (!f.IsFolder)
            {
                //  Checks file type.
                std::for_each(std::begin(supportedExts), std::end(supportedExts), [nameLCase, &isSupported](const std::wstring ext) mutable
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
