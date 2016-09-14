#include "FileReader.h"

namespace Bookmarks
{
    std::wstring FileReader::GetParam(std::wstring fileName/*полный путь к файлу*/, std::wstring paramName)
    {
#ifndef _WINDOWS
        {
            unsigned int i;
            for (i = 0; i < fileName.size(); i++)
            {
                if (fileName[i] == '\\') fileName[i] = '/';
            }
        }
#endif
        FILE *f = _wfopen(fileName.c_str(), _T("r"));
        std::wstring result;

        if (f)
        {
            char lineptr[MAX_LINE_LENGTH];
            //  fgets дополняет строку 0-м
            while (fgets(lineptr, MAX_LINE_LENGTH, f))
            {
                wchar_t wlineptr[MAX_LINE_LENGTH];
                MultiByteToWideChar(CP_ACP, 0, lineptr, -1, wlineptr, MAX_LINE_LENGTH);
                std::wstring wline = wlineptr;

                //  очистка прочитанной строки от завершающих символов новой строки
                while (wline.size())
                    if ((wline.back() == '\x0a') || (wline.back() == '\x0d'))
                        wline.pop_back();
                    else
                        break;

                if (wline.find(paramName) != std::string::npos)
                {
                    result = wline.substr(paramName.size() + 1);
                    break;
                }
            }
            fclose(f);
        }
        return result;
    }

    std::wstring FileReader::GetParamCurDir(std::wstring fileName/*путь к файлу отн. cwd*/, std::wstring paramName)
    {
        std::wstring path = _curDir + _T("\\") + fileName;
        return GetParam(path, paramName);
    }
}
