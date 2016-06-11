#pragma once

#include <string>
#include <vector>

#include "FileListReader.h"
#include "..\main.h"

namespace Bookmarks
{
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
        std::string cmd_dir = _T("dir /OG >>");                             //  вызов для Windows
#else
        std::string cmd_dir = _T("cygdir.exe -a -1 -X -F -N >>");           //  вызов для Cygwin
#endif  //  USE_CYGWIN

        std::string tmpdir = _T("/tmp/links/");
#else
        std::string cmd_dir = _T("dir -a -1 -X -F -N >>");                  //  вызов для Linux

                                                                            //std::string tmpdir = _T("/var/tmp/");                             //  лучше правильно настроить доступ к папке с файлами,
                                                                            //  чем использовать отдельную директорию
        std::string tmpdir = _T("/tmp/links/");
#endif
    public:
        virtual std::vector<std::string> GetFileList();
    };
}
