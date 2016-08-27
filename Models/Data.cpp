
#include "Data.h"
#include "../main.h"

#include <sstream>
#include <locale>
#include <iomanip>
#include <algorithm>
#include <assert.h>

namespace Bookmarks
{
    FileVector Data::FileList::ReadFileList()
    {
        FileVector result;

        assert(Reader);
        std::vector<std::wstring> lines = Reader->GetFileList();

#ifndef LINUX
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

            std::wstring name = columns[3];
            std::for_each(columns.begin() + 4, columns.end(), [&](std::wstring &s) { name += _T(" ") + s; });

            result.push_back(File(name, isFolder, dateTime, size));
#else
            bool isFolder = (fileName.size() <= 4) || (fileName.rfind('.') != (fileName.size() - 4));
            result.push_back(File(line, isFolder, 0, 0));
#endif // !LINUX
        }

        //  —ортировка списка.
        //  https://action.mindjet.com/task/14640967
        std::sort(result.begin(), result.end(), [](File a, File b) {
            std::transform(a.Name.begin(), a.Name.end(), a.Name.begin(), (int(*)(int))std::tolower);
            std::transform(b.Name.begin(), b.Name.end(), b.Name.begin(), (int(*)(int))std::tolower);
            if (a.IsFolder != b.IsFolder)
                return a.IsFolder && !b.IsFolder;
            else
                return a.Name < b.Name;
        });
        return result;
    }

    FileVector Data::ReadFileList()
    {
        return Files->ReadFileList();
    }

    std::vector<std::wstring> FileListReader::ReadFile(std::wstring fileName)
    {
        FILE *f = _tfopen(fileName.c_str(), _T("r"));
        std::vector<std::wstring> lines;

        if (f)
        {
            char lineptr[MAX_LINE_LENGTH];
            while (fgets(lineptr, MAX_LINE_LENGTH, f))
                if (strlen(lineptr))
                {
                    wchar_t wline[MAX_LINE_LENGTH];
                    MultiByteToWideChar(CP_OEMCP, 0, lineptr, -1, wline, MAX_LINE_LENGTH);
                    lines.push_back(std::wstring(wline));
                }

            fclose(f);
        }
        //  https://action.mindjet.com/task/14719680
        //  ¬ременный файл удал€етс€.
        _tremove(fileName.c_str());
        return lines;
    }
}

