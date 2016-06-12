
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
        std::vector<std::wstring> lines = Reader->GetFileList();

#ifndef LINUX
        //  5 ������ ����� ����� ����������
        lines.erase(lines.begin(), lines.begin() + 5);
        //  ��� Windows 2 ��������� ������ �������� ���. ���., ������� ���������
        lines.pop_back();
        lines.pop_back();
#endif // !LINUX

        for (auto l = lines.begin(); l != lines.end(); ++l)
        {
            std::wstring line = *l;
#ifndef LINUX
            //  ��������� ������� ������ (2 ����. �� Windows)
            line.pop_back();
            //  https://action.mindjet.com/task/14665015
            //  �������� ������ �� ����� �����
            std::vector<std::wstring> columns;
            std::wstringstream ss(line);
            std::wstring column;
            while (ss >> column)
                columns.push_back(column);

            //  ������ ������� �������� �����/�����
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

        //  ���������� ������.
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
                    MultiByteToWideChar(CP_OEMCP, 0, lineptr, sizeof(lineptr), wline, MAX_LINE_LENGTH);
                    lines.push_back(std::wstring(wline));
                }

            fclose(f);
        }
        return lines;
    }
}

