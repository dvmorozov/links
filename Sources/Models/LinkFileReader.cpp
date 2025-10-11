#include "App/stdafx.h"

#include "LinkFileReader.h"
#include "Utils/DebugLog.h"
#include "FileList.h"
#include "Utils/Errors.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
//  TODO: cache file content.
std::wstring LinkFileReader::GetParam(std::wstring filePath, std::wstring paramName)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Getting parameter '" << narrow(paramName) << "' from file: " << narrow(filePath));
    //  TODO: move this into separate function.
#ifndef _WINDOWS
    {
        unsigned int i;
        for (i = 0; i < filePath.size(); i++) {
            if (filePath[i] == '\\')
                filePath[i] = '/';
        }
    }
#endif

    // Check if the file exists
    if (!std::filesystem::exists(filePath)) {
        file_doesnt_exist(L"File does not exist: " + filePath);
    }

    FILE *f = std::fopen(narrow(filePath).c_str(), "r");
    if (!f)
        no_permission(filePath);

    std::wstring result;

    try {
        char linePtr[MAX_LINE_LENGTH];
        // fgets appends a null character to the string.
        while (fgets(linePtr, MAX_LINE_LENGTH, f)) {
            std::wstring wline;
#ifdef _WINDOWS
            auto isUtf8 = !FileList::HasExtension(filePath, ext_url);
            wline       = widen(linePtr, isUtf8 ? CP_UTF8 : 1251);
#else
            //  TODO: check this.
            // wline = widen(linePtr, isUtf8 ? "UTF-8" : "CP1251");
            wline = widen(linePtr, "UTF-8");
#endif
            // Remove trailing newline characters from the read line.
            while (wline.size())
                if ((wline.back() == '\x0a') || (wline.back() == '\x0d'))
                    wline.pop_back();
                else
                    break;

            // Check if wline starts with paramName
            if (wline.rfind(paramName, 0) == 0) {
                result = wline.substr(paramName.size() + 1);
                break;
            }
        }
    } catch (std::exception &) {
        fclose(f);
        throw;
    }

    fclose(f);

    return result;
}

std::wstring
LinkFileReader::GetParamFromCurDirFile(std::wstring fileName /*relative file path from cwd*/,
                                       std::wstring paramName)
{
    std::wstring path = _curDir + path_separator + fileName;
    return GetParam(path, paramName);
}
}  // namespace Bookmarks
