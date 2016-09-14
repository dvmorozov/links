
#include "Data.h"
#include "FileReader.h"
#include "../Controllers/Commands.h"
#include "../main.h"

#include <sstream>
#include <locale>
#include <iomanip>
#include <algorithm>
#include <assert.h>

namespace Bookmarks
{
    FileVector Data::GetFileList()
    {
        return _fileList->GetFileList();
    }

    FileVector Data::GetDirList()
    {
        return _fileList->GetDirList();
    }

    std::vector<std::wstring> FileListReader::ReadFile(std::wstring fileName)
    {
        FILE *f = _wfopen(fileName.c_str(), _T("r"));
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
        //  Временный файл удаляется.
        _wremove(fileName.c_str());
        return lines;
    }
}

