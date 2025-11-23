#include "App/stdafx.h"

#include "FileListReaderDir.h"
#include "Utils/DebugLog.h"
#include "../Controllers/Commands.h"
#include "Utils/Errors.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
std::vector<std::wstring> FileListReaderDir::ReadFileLines()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Reading directory listing");
#ifdef _WINDOWS
#ifndef USE_CYGWIN
    // https://superuser.com/questions/1291786/why-does-windows-dir-command-produce-characters-instead-of-russian-alphabet
    const std::wstring cmdDir = _T("dir /OG >");  // Windows shell command.
#else
    const std::wstring cmdDir = _T("cygdir.exe -a -1 -X -F -N >>");  // Cygwin shell command.
#endif  // USE_CYGWIN

#else
    //  This command displays file names without quotes.
    //  Because the file name may contain spaces it should't be split.
    const std::wstring cmdDir = _T("dir -alXFN --time-style=long-iso >>");  // Linux shell command.
#endif

    std::vector<std::wstring> result;
    // Get the temporary file name.
    // TODO: move to a separate function.
    // Temporary file names should be unique in case multiple instances of this program are running
    // simultaneously.

    // Temporary file name template.
    std::wstring tempFileNameTemplate =
        concatenate_paths(get_temp_directory_path(), L"tempXXXXXX");
#ifdef _WINDOWS
    std::replace(tempFileNameTemplate.begin(), tempFileNameTemplate.end(), '/', '\\');
    // The file name should contain the full path for the utility to check if the file exists.
    TCHAR *tempFn = (TCHAR *)malloc((tempFileNameTemplate.size() + 1) * sizeof(TCHAR));
    wcscpy(tempFn, tempFileNameTemplate.c_str());
    if (NULL == _wmktemp(tempFn))
#else
    char *tempFn = malloc_narrow(tempFileNameTemplate);
    int   fd      = mkstemp(tempFn);
    std::wstring tempFnWide = widen(tempFn);
    if (fd == -1)
#endif
    {
        free(tempFn);
        sysutility_error(L"Failed to create file: " + tempFnWide, L"");
        return result;
    }

#ifdef _WINDOWS
    tempFileNameTemplate = tempFn;
#else
    tempFileNameTemplate = widen(tempFn);
#endif
    free(tempFn);
#ifndef _WINDOWS
    close(fd);
#endif
    // Calls the system utility to read the content of the directory.
    std::wstring command = cmdDir;
    //  TODO: use formatting function for this.
    command += _T(" \"") + tempFileNameTemplate + _T("\"");

#ifdef _WINDOWS
    if (-1 == _wsystem(command.c_str()))
#else
    if (-1 == std::system(narrow(command).c_str()))
#endif
    {
        sysutility_error(L"Failed to execute command: " + command, L"");
        return result;
    }

    try {
        SetFileName(tempFileNameTemplate);
        //  Read the utility output.
        result = ReadFile();
        //  TODO: use file holder.
        std::filesystem::remove(tempFileNameTemplate);
        return result;
    } catch (std::exception &) {
        std::filesystem::remove(tempFileNameTemplate);
        throw;
    }
}

// Factory method returning FileListReader.
TextFileReader *FileListReaderDir::GetFileReader(
#ifdef _WINDOWS
    UINT codePage
#endif
)
{
    return new FileListReaderDir(
#ifdef _WINDOWS
        codePage
#endif
    );
}
}  // namespace Bookmarks
