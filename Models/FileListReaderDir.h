#pragma once

#include <string>
#include <vector>

#include "FileListReader.h"
#include "..\main.h"

namespace Bookmarks
{
    //  Читает содержимое текущей директории (поэтому имя дир. не передается) с пом. вызова сист. утил.
    class FileListReaderDir : public FileListReader
    {
    private:
        //-------------------------------------------------------------------------------------------------
        //  если скомпилить с Ansi-библ. с пом. VC, то все работает прекрасно
        //  если скомпилить с Unicode-библ. с пом. VC, то русский текст пропадает + англ. текст выводится криво
        //  если скомпил с cygwin библ. то вывод работает нормально, но текст получается от браузера в
        //  какой-то иной кодировке
#if !defined LINUX

#ifndef USE_CYGWIN
        //  https://action.mindjet.com/task/14665015
        std::wstring _cmdDir = _T("dir /OG >>");                            //  Windows shell command.
#else
        std::wstring _cmdDir = _T("cygdir.exe -a -1 -X -F -N >>");          //  Cygwin shell command.
#endif  //  USE_CYGWIN
        std::wstring _tmpDir = _T("/tmp/links/");

#else

        std::wstring _cmdDir = _T("dir -alXFN >>");                         //  Linux shell command.

        //std::wstring tmpdir = _T("/var/tmp/");                            //  лучше правильно настроить доступ к папке с файлами,
                                                                            //  чем использовать отдельную директорию
        std::wstring _tmpDir = _T("/tmp/links/");

#endif
    public:
        virtual std::vector<std::wstring> ReadFileList();
    };
}
