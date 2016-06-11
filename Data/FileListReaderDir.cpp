
#include <string>
#include <vector>

#include "FileListReaderDir.h"

namespace Bookmarks
{
    std::vector<std::string> FileListReaderDir::GetFileList()
    {
        //  собираетс€ вектор строк файла-списка
        std::vector<std::string> lines;

        //  получение имени временного файла
        //  !!! имена временных файлов д.б. уникальными на тот случай, когда
        //  будет одновременно запущено несколько экземпл€ров данной программы !!!
        //  получение названий каталогов
        TCHAR temp_fn[11];
        _tcscpy(temp_fn, _T("tempXXXXXX"));
        _tmktemp(temp_fn);

        std::string temp = document_root.substr(0, document_root.rfind('/')) + tmpdir + temp_fn;
#ifndef LINUX
        std::replace(temp.begin(), temp.end(), '/', '\\');
#endif
        std::string utility_name = cmd_dir;
        utility_name += _T(" \"") + temp + _T("\"");

        if (-1 == _tsystem(utility_name.c_str()))
        {
            error = E_SYS_UTILITY;
            //  ошибка не фатальна - ничего не выводитс€
        }
        else
            lines = ReadFile(temp);

        /*  этот метод не позвол€ет получить никакой
        полезной информации, кроме имени файла (невозможно
        отделить названи€ файлов от названий директорий);
        хот€ в заголовочниках POSIX предусмотрены эти данные
        DIR *dp;
        struct dirent *ep;

        dp = opendir (_T("./"));
        if (dp != NULL)
        {
        while (ep = readdir (dp))
        {
        _tprintf(_T("%s\n"), ep->d_name);
        }
        (void) closedir (dp);
        }
        else
        puts(_T("Couldn't open the directory."));
        */
        return lines;
    }
}
