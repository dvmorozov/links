#include "App/stdafx.h"

#include "FileList.h"
#include "Shared/Controllers/Commands.h"
#include "LinkFileReader.h"
#include "Utils/Miscellaneous.h"
#include "Utils/DebugLog.h"

namespace Bookmarks
{
void FileList::RemoveNonInformativeLines(std::vector<std::wstring> &lines)
{
#ifdef _WINDOWS
    if (lines.size() >= 7) {
        // Skip the first 5 lines.
        lines.erase(lines.begin(), lines.begin() + 5);
        // For Windows, the last 2 lines contain additional information, so they are removed.
        lines.pop_back();
        lines.pop_back();
    }
#else
    if (lines.size() >= 1) {
        // The first string contains "total" and must be removed.
        lines.erase(lines.begin(), lines.begin() + 1);
    }
#endif
}

std::vector<std::wstring> FileList::SplitLine(std::wstring &line, size_t fileNameColumnIndex)
{
    std::vector<std::wstring> columns;
    std::wstringstream        ss(line);
    std::wstring              column;
    int                       columnIndex = 0;

    while (ss >> column) {
        columns.push_back(column);
        columnIndex++;
        //  The last column is the file name.
        //  It could contain spaces, so it shouldn't be split.
        //  See the output of the "dir" command.
        if (columnIndex == fileNameColumnIndex) {
            // Extract the rest of the line as the last column
            std::wstring restOfLine;
            std::getline(ss, restOfLine);
            restOfLine = TrimSpaces(restOfLine);
            columns.push_back(restOfLine);
            break;
        }
    }

    return columns;
}

std::time_t FileList::ParseDateTime(std::vector<std::wstring> lineColumns)
{
    struct std::tm tm;

#ifndef _WINDOWS
    // An example of a Linux line:
    // drwxr-xr-x  2 dmitry www-data 4096 2023-09-14 19:52 Travelling/
    if (lineColumns.size() >= 5) {
        std::wstringstream dateTimeStr(lineColumns[5] + _T(" ") + lineColumns[6]);
        dateTimeStr >> std::get_time(&tm, _T("%Y-%m-%d %H:%M"));
    }
#else
    if (lineColumns.size() >= 2) {
        std::wstring dateTimeStr(lineColumns[0] + _T(" ") + lineColumns[1]);
        int          day, month, year, hour, min;
        // Date has the following format "%d/%m/%Y %H:%M".
        swscanf(dateTimeStr.c_str(), _T("%2d/%2d/%4d %2d:%2d"), &day, &month, &year, &hour, &min);
        tm.tm_sec   = 0;
        tm.tm_min   = 0;
        tm.tm_hour  = hour;
        tm.tm_mon   = month - 1;
        tm.tm_year  = year - 1900;
        tm.tm_mday  = 0;
        tm.tm_wday  = 0;
        tm.tm_yday  = 0;
        tm.tm_isdst = 0;
    }
#endif
    std::time_t dateTime = mktime(&tm);
    return dateTime;
}

std::wstring FileList::TrimSpaces(std::wstring str)
{
    str.erase(0, str.find_first_not_of(' '));  // Removes prefixing spaces.
    str.erase(str.find_last_not_of(' ') + 1);  // Removes suffixing spaces.
    return str;
}

std::wstring FileList::ConcatFileName(std::vector<std::wstring> lineColumns, int firstColumnIndex)
{
    std::wstring fileName = lineColumns[firstColumnIndex];
    std::for_each(lineColumns.begin() + firstColumnIndex + 1, lineColumns.end(),
                  [&](std::wstring &s) { fileName += _T(" ") + s; });

    // On Linux, directory names are terminated by "/" (as a result of the "dir" command).
    if (fileName[fileName.size() - 1] == PATH_DELIMITER)
        fileName = fileName.substr(0, fileName.size() - 1);

    return fileName;
}

void FileList::ParseLine(std::wstring &line, File &result)
{
    // Line terminating symbol is removed.
    line.pop_back();

#ifdef _WINDOWS 
    const size_t fileNameColumnIndex = 3;
    std::vector<std::wstring> columns = SplitLine(line, fileNameColumnIndex);
#else    
    const size_t fileNameColumnIndex = 7;
    // Examples of a Linux line:
    // drwxr-xr-x  2 dmitry www-data 4096 2023-09-14 19:52 Travelling/
    // -rw-rw-r-- 1 ubuntu ubuntu  284 2006-06-11 21:32 ONLINECHANGE  Translation Status.url
    std::vector<std::wstring> columns = SplitLine(line, fileNameColumnIndex);
#endif

    // Time must have format HH:MM without AM/PM!
    std::time_t dateTime = ParseDateTime(columns);

#ifdef _WINDOWS
    std::wstring col2     = TrimSpaces(columns[2]);
    bool         isFolder = col2 == _T("<DIR>");

    size_t size = col2 != _T("<DIR>") ? std::stoi(col2) : 0;

    // File name may be located in a few adjacent columns separated by spaces.
    std::wstring fileName = ConcatFileName(columns, fileNameColumnIndex);

#else
    bool isFolder = columns[0][0] == 'd';

    size_t size;
    swscanf(TrimSpaces(columns[4]).c_str(), _T("%zd"), &size);

    std::wstring fileName = ConcatFileName(columns, fileNameColumnIndex);
#endif

    // Remove the terminating '*' character if it exists.
    if (!fileName.empty() && fileName.back() == '*') {
        fileName.pop_back();
    }

    if (!isFolder) {
        // Read file content.
        LinkFileReader fr;
        std::wstring   url  = fr.GetParam(fileName, param_url);
        std::wstring   name = fr.GetParam(fileName, param_name);

        result = File(fileName, isFolder, dateTime, size, url, name);
    } else
        result = File(fileName, isFolder, dateTime, size, _T(""), _T(""));
}

void FileList::ReadFileLines()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("ReadFileLines() called");
    _fileList.clear();

    assert(_reader);
    DEBUG_LOG("Reading lines from file reader");
    std::vector<std::wstring> lines = _reader->ReadFileLines();
    DEBUG_LOG("Read " << lines.size() << " raw lines");

    // Log first few raw lines
    for (size_t i = 0; i < lines.size() && i < 5; ++i) {
        DEBUG_LOG("Raw line " << i << ": " << narrow(lines[i]));
    }

    RemoveNonInformativeLines(lines);
    DEBUG_LOG("After removing non-informative lines: " << lines.size() << " lines");

    for (auto l = lines.begin(); l != lines.end(); ++l) {
        File file;
        ParseLine(*l, file);
        ReadBookmarkParameters(file);
        _fileList.push_back(file);
        
        // Log each parsed file
        DEBUG_LOG("Parsed file: " << narrow(file.FileName) 
                 << " (IsFolder: " << (file.IsFolder ? "YES" : "NO")
                 << ", Size: " << file.Size
                 << ", URL: " << (file.Url.empty() ? "EMPTY" : narrow(file.Url.substr(0, 30)))
                 << ")");
    }

    DEBUG_LOG("Total files parsed: " << _fileList.size());
    DEBUG_LOG_FUNC_EXIT();

    // Sorts the list (folders at the beginning).
    std::sort(_fileList.begin(), _fileList.end(), [](File a, File b) {
        std::transform(a.FileName.begin(), a.FileName.end(), a.FileName.begin(),
                       (int (*)(int))std::tolower);
        std::transform(b.FileName.begin(), b.FileName.end(), b.FileName.begin(),
                       (int (*)(int))std::tolower);
        if (a.IsFolder != b.IsFolder)
            return a.IsFolder && !b.IsFolder;
        else
            return a.FileName < b.FileName;
    });
}

bool FileList::HasExtension(std::wstring fileName, std::wstring ext)
{
    if (ext.empty()) {
        return true;
    }

    assert(fileName.size() >= ext.size());
    return (!fileName.compare(fileName.size() - ext.size(), ext.size(), ext));
}

FileVector FileList::GetFileList()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetFileList() called, _fileList.size() = " << _fileList.size());
    
    FileVector result(_fileList.size());

    // Log all files before filtering
    DEBUG_LOG("Raw file list contents:");
    for (size_t i = 0; i < _fileList.size() && i < 10; ++i) {
        const auto& f = _fileList[i];
        DEBUG_LOG("Raw[" << i << "]: " << narrow(f.FileName) 
                 << " (IsFolder: " << (f.IsFolder ? "YES" : "NO")
                 << ", URL: " << (f.Url.empty() ? "EMPTY" : narrow(f.Url.substr(0, 30)))
                 << ")");
    }

    // Copies only files with given extensions.
    auto it = std::copy_if(_fileList.begin(), _fileList.end(), result.begin(), [this](File &f) {
        auto nameLCase = f.FileName;
        // Empty extensions are allowed and mean that the file is in UTF-8.
        const std::wstring supportedExts[] = {_T(".url"), ext_utf8, _T("")};
        std::transform(nameLCase.begin(), nameLCase.end(), nameLCase.begin(), ::tolower);

        auto isSupported = false;
        if (!f.IsFolder) {
            // Checks file type.
            std::for_each(std::begin(supportedExts), std::end(supportedExts),
                          [nameLCase, &isSupported, &f, this](const std::wstring ext) mutable {
                              if (HasExtension(nameLCase, ext)) {
                                  isSupported = true;
                                  DEBUG_LOG("File " << narrow(f.FileName) << " matches extension " << narrow(ext));
                              }
                          });
            if (!isSupported) {
                DEBUG_LOG("File " << narrow(f.FileName) << " does NOT match any supported extension");
            }
        } else {
            DEBUG_LOG("Skipping folder: " << narrow(f.FileName));
        }
        return isSupported;
    });

    result.resize(std::distance(result.begin(), it));  // Shrink container to new size.
    
    DEBUG_LOG("After filtering: " << result.size() << " supported files found");
    
    // Log filtered results
    for (size_t i = 0; i < result.size() && i < 5; ++i) {
        const auto& f = result[i];
        DEBUG_LOG("Filtered[" << i << "]: " << narrow(f.FileName) 
                 << " (URL: " << (f.Url.empty() ? "EMPTY" : narrow(f.Url.substr(0, 50)))
                 << ")");
    }

    DEBUG_LOG_FUNC_EXIT();
    return result;
}

FileVector FileList::GetDirList()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetDirList() called");
    
    FileVector result(_fileList.size());

    // Copies only directories.
    auto it = std::copy_if(_fileList.begin(), _fileList.end(), result.begin(),
                           [](File &f) { return f.IsFolder; });
    result.resize(std::distance(result.begin(), it));  // Shrink container to new size.

    DEBUG_LOG("GetDirList() returning " << result.size() << " directories");
    DEBUG_LOG_FUNC_EXIT();
    return result;
}

int File::_seqNumCounter = 1;

// As usual, the method works with files inside the current catalog.
void FileList::DeleteFilesWithDuplicatedUrls()
{
    FileVector fileList = GetFileList();
    // Sort files by URL.
    std::sort(fileList.begin(), fileList.end(), [](File a, File b) { return a.Url < b.Url; });

    // Mark duplicates as deleted.
    for (auto it = fileList.begin(); it != fileList.end(); it++) {
        if (it != fileList.begin()) {
            if (it->Url == (it - 1)->Url)
                it->SetDeleteMark();
        }
    }

    // Delete duplicates.
    for (auto it = fileList.begin(); it != fileList.end(); it++) {
        if (it->GetDeleteMark()) {
            std::filesystem::remove(it->FileName.c_str());
            auto seqNum = it->GetSeqNum();
            // Removes data from the main file list.
            auto erased = std::find_if(_fileList.begin(), _fileList.end(),
                                       [seqNum](const File &f) { return f.GetSeqNum() == seqNum; });
            if (erased != _fileList.end())
                _fileList.erase(erased);
        }
    }
}

void FileList::ReadBookmarkParameters(File &file)
{
    Bookmarks::LinkFileReader fr(cwd);

    file.Url = fr.GetParamFromCurDirFile(file.FileName, param_url);
    if (!file.Url.empty()) {
        // Add 'http://' prefix if necessary to build a correct link.
        // Transform URL to lower case.
        std::transform(file.Url.begin(), file.Url.end(), file.Url.begin(), tolower);
        if (file.Url.find(_T("http://")) == std::string::npos &&
            file.Url.find(_T("https://")) == std::string::npos)
            file.Url = _T("http://") + file.Url;
    }

#ifdef EXTENDED_URL_FILE
    file.Name = fr.GetParamFromCurDirFile(file.FileName, param_name);
#endif
}
}  // namespace Bookmarks
