
#include "App/stdafx.h"

#include "TextFileReader.h"
#include "Utils/DebugLog.h"
#include "Utils/Errors.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
void TextFileReader::SetFileName(std::wstring fileName) 
{ 
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Setting file name: " << narrow(fileName));
    FileName = fileName; 
    DEBUG_LOG_FUNC_EXIT();
}

std::vector<std::wstring> TextFileReader::ReadFile()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Reading file: " << narrow(FileName));
    
#ifdef _WINDOWS
    FILE *f = _wfopen(FileName.c_str(), _T("r"));
#else
    FILE *f = std::fopen(narrow(FileName).c_str(), "r");
#endif
    if (!f) {
        DEBUG_LOG("Failed to open file: " << narrow(FileName));
        no_permission(FileName);
    }

    std::vector<std::wstring> lines;
    DEBUG_LOG("File opened successfully, reading lines");

    try {
        //  TODO: Remake to allow unlimited string length.
        char lineptr[MAX_LINE_LENGTH];
        int lineCount = 0;
        while (fgets(lineptr, MAX_LINE_LENGTH, f)) {
            if (strlen(lineptr)) {
#ifdef _WINDOWS
                lines.push_back(widen(lineptr, CodePage));
#else
                lines.push_back(widen(lineptr));
#endif
                lineCount++;
            }
        }
        DEBUG_LOG("Read " << lineCount << " lines from file");
    } catch (std::exception &e) {
        DEBUG_LOG("Exception while reading file: " << e.what());
        fclose(f);
        throw;
    }

    fclose(f);
    DEBUG_LOG("File reading completed successfully");
    DEBUG_LOG_FUNC_EXIT();
    return lines;
}
}  // namespace Bookmarks
