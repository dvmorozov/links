
#include "TextFileReader.h"

#include "../main.h"

namespace Bookmarks
{
    void TextFileReader::SetFileName(std::wstring fileName)
    {
        _fileName = fileName;
    }

    std::vector<std::wstring> TextFileReader::ReadFile()
    {
        FILE *f = _wfopen(_fileName.c_str(), _T("r"));
        std::vector<std::wstring> lines;

        if (f)
        {
            char lineptr[MAX_LINE_LENGTH];
            while (fgets(lineptr, MAX_LINE_LENGTH, f))
                if (strlen(lineptr))
                {
#ifndef LINUX
                    wchar_t wline[MAX_LINE_LENGTH];
                    MultiByteToWideChar(CP_OEMCP, 0, lineptr, -1, wline, MAX_LINE_LENGTH);
                    lines.push_back(std::wstring(wline));
#endif
                }

            fclose(f);
        }
        //  https://action.mindjet.com/task/14719680
        //  Временный файл удаляется.
        _wremove(_fileName.c_str());
        return lines;
    }
}
