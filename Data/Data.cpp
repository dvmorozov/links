
#include "Data.h"
#include "../main.h"

#include <sstream>
#include <locale>
#include <iomanip>
#include <algorithm>
#include <assert.h>

namespace Bookmarks
{
    std::vector<Bookmarks::File> Data::FileList::ReadFileList()
    {
        std::vector<Bookmarks::File> result;

        assert(Reader);
        std::vector<std::string> lines = Reader->GetFileList();

#ifndef LINUX
        //  5 первых строк нужно пропустить
        lines.erase(lines.begin(), lines.begin() + 5);
        //  для Windows 2 последние строки содержат доп. инф., поэтому удаляются
        lines.pop_back();
        lines.pop_back();
#endif // !LINUX

        for (auto l = lines.begin(); l != lines.end(); ++l)
        {
            std::string line = *l;
#ifndef LINUX
            //  удаляется перевод строки (2 симв. на Windows)
            line.pop_back();
            //  https://action.mindjet.com/task/14665015
            //  разбивка строки на набор строк
            std::vector<std::string> columns;
            std::istringstream ss(line);
            std::string column;
            while (ss >> column)
                columns.push_back(column);

            //  разбор времени создания файла/папки
            struct std::tm tm;
            std::istringstream dateTimeStr(columns[0] + " " + columns[1]);
            dateTimeStr >> std::get_time(&tm, "%d.%m.%Y %H:%M");
            std::time_t dateTime = mktime(&tm);

            std::string col2 = columns[2];
            col2.erase(0, col2.find_first_not_of(' '));         //  prefixing spaces
            col2.erase(col2.find_last_not_of(' ') + 1);         //  surfixing spaces

            bool isFolder = col2 == _T ("<DIR>");

            size_t size = col2 != _T("<DIR>") ? std::stoi(col2) : 0;

            std::string name = columns[3];

            result.push_back(File(name, isFolder, dateTime, size));
#else
            bool isFolder = (fileName.size() <= 4) || (fileName.rfind('.') != (fileName.size() - 4));
            result.push_back(File(line, isFolder, 0, 0));
#endif // !LINUX
        }

        //  Сортировка списка.
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

    std::vector<Bookmarks::File> Data::ReadFileList()
    {
        return Files->ReadFileList();
    }

    std::vector<std::string> FileListReader::ReadFile(std::string fileName)
    {
        FILE *f = _tfopen(fileName.c_str(), _T("r"));
        std::vector<std::string> lines;

        if (f)
        {
            TCHAR lineptr[MAX_LINE_LENGTH];
            while (_fgetts(lineptr, MAX_LINE_LENGTH, f))
                if (_tcslen(lineptr))
                    lines.push_back(std::string(lineptr));

            fclose(f);
        }
        return lines;
    }
}

