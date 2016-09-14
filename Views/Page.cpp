
#include "Page.h"
#include "../Models/FileReader.h"
#include "../Controllers/Commands.h"
#include "../main.h"

namespace Bookmarks
{

    Page::Page()
    {
    }


    Page::~Page()
    {
    }

    std::wstring Page::GetImagePath(std::wstring image_file)
    {
        if (!img_path)
            return _T("");
        return std::wstring(img_path) + image_file;
    }

    void Page::InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        std::wstring image_path = GetImagePath(image_file);

        if (!image_path.empty())
        {
            if (!script_name.empty())
            {
                if (!url.empty())
                {
                    _tprintf(_T("\
                            <a href=\"%s?%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), url.c_str(), image_path.c_str(), alt.c_str(), size, size);
                }
                else
                {
                    //  передается ссылка на внешний ресурс; script_name содержит ссылку к ресурсу
                    _tprintf(_T("\
                            <a href=\"%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), image_path.c_str(), alt.c_str(), size, size);
                }
            }
            else
            {// вставляется иконка без ссылки
                if (!url.empty()) //  в url передается дополнение к тексту, переданному через alt
                {
                    _tprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), url.c_str(), size, size);
                }
                else
                {
                    _tprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), size, size);
                }
            }
        }
    }

    void Page::PrintHead(std::wstring title)
    {
        _tprintf(_T("%s"), _T("<html>"));
        _tprintf(_T("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><meta http-equiv=\"Content-Language\" content=\"ru\">\n"));
        _tprintf(_T("<title>%s</title></head>\n"), title.c_str());
        _tprintf(_T("<body><br>\n"));
    }

    void Page::PrintTail()
    {
        PrintInfo();
        _tprintf(_T("%s"), _T("</body></html>\n"));
    }

    void Page::InsertRowCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, TCHAR *image_file, const TCHAR* hint)
    {
        _tprintf(_T("<td>"));
        InsertCommandButton(cmd, dir, file, image_file, hint);
        _tprintf(_T("</td>"));
    }

    void Page::InsertLinkButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        _tprintf(_T("<td>"));
        InsertButton(image_file, script_name, url, 16, alt);
        _tprintf(_T("</td>"));
    }

    std::wstring Page::GetCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file)
    {
        std::wstring command;
        if (dir) command += dir;
        if (cmd_key) command += cmd_key;
        if (key) command += key;
        if (cmd) command += cmd;
        if (file) command += file;
        return command;
    }

    std::wstring Page::GetFullCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file)
    {
        return std::wstring(full_script_name ? full_script_name : _T("")) + _T("?") + GetCommandUrl(cmd, dir, file);
    }

    void Page::InsertCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, TCHAR *image_file, const TCHAR* hint)
    {
        InsertButton(image_file, full_script_name ? full_script_name : _T(""), GetCommandUrl(cmd, dir, file), 16, hint ? hint : _T(""));
    }

    //  вызывает внешнюю утилиту и читает список файлов
    std::vector<std::wstring> Page::ReadFolders()
    {
        FileVector files = _data.GetDirList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->FileName);
        return result;
    }

    std::vector<std::wstring> Page::ReadFiles()
    {
        FileVector files = _data.GetFileList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->FileName);
        return result;
    }

    void Page::InsertAddButtons()
    {
        _tprintf(_T("\n<p>\n"));
        InsertCommandButton(cmd_add_conf, query, _T(""), _T("add_link.bmp"), _T("Создать ссылку"));
        InsertCommandButton(cmd_add_folder_conf, query, _T(""), _T("add_folder.bmp"), _T("Создать папку"));
        _tprintf(_T("\n</p>\n"));
    }

    void Page::OpenInnerTable()
    {
        //  Opens new cell for internal table.
        _tprintf(_T("<td valign=\"top\">\n\
            <table border=\"0\" cellspacing=\"0\" cellpadding=\"1\">\n\
                <tbody>"));
    }

    void Page::CloseInnerTable()
    {
        _tprintf(_T("</tbody>\n\
                </table>\n\
            </td>"));
    }

    //  https://action.mindjet.com/task/14720269
    void Page::OpenInnerFileTableRow(std::wstring url)
    {
        _tprintf(_T("<tr>"));
    }

    //  https://action.mindjet.com/task/14720269
    void Page::OpenInnerDirTableRow(std::wstring url)
    {
        _tprintf(_T("<tr>"));
    }

    //  закрывается ячейка и строка внутренней таблицы
    void Page::CloseInnerTableRow()
    {
        _tprintf(_T("</tr>"));
    }

    void Page::OpenOuterTable()
    {
        //  Creates outer table with 2 columns for folders and files.
        //  Adds new row for internal tables.
        _tprintf(_T("\n\
<table border=\"1\" cellspacing=\"0\" cellpadding=\"0\">\n\
    <tbody>\n\
        <tr>\n\
            <th>%s\n\
            </th>\n\
            <th>%s [%s]\n\
            </th>\n\
        </tr><tr>\n"), _T("Папки"), _T("Ссылки"), query);
    }

    void Page::CloseOuterTable()
    {
        //  For the first closes internal table row.
        _tprintf(_T("</tr>\n\
    </tbody>\n\
</table>\n"));
    }

    void Page::PrintDirList(std::vector<std::wstring> dirs)
    {
        for (std::vector<std::wstring>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
        {
            if (dir->size())
            {
                // для корневой папки на вставляются ссылки "ВВЕРХ" и "КОРЕНЬ"
                if (*dir == _T("."))
                {// переход к корневой папке закладок
                    if (wcslen(query))
                    {
                        OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */));
                        InsertRowCommandButton(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */, _T("to_start_page.bmp"), _hintFolder.c_str());
                        _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _home.c_str()/*название*/);
                        CloseInnerTableRow();
                    }
                }
                else
                if (*dir == _T(".."))
                {// переход на один уровень вверх или к корневой папке закладок
                    if (wcslen(query))
                    {
                        std::wstring upDir = query;
                        //  Получаем имя верхней папки.
                        auto slashPos = upDir.rfind(_T("/"));
                        upDir = slashPos != std::string::npos ? upDir.substr(0, slashPos) : upDir;

                        OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */));
                        //  Переход к самому верхнему уровню каталога (даже если верх. папка явл. корневой).
                        InsertRowCommandButton(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */, _T("to_upper_folder.bmp"), _hintFolder.c_str());
                        _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _T("ВВЕРХ"));

                        CloseInnerTableRow();
                        /*  переход к корневой папке уже вставляется выше */
                    }
                }
                else
                    //  если текущий запрос не равен ".", ".." или 0, то
                    //  строка запроса содержит имя текущей папки, которое нужно
                    //  добавлять к именам найденных файлов для того, чтобы
                    //  при последующем запросе произошел переход в нужную папку
                {
                    //  full_dir в результате содержит url относительно url скрипта 
                    //  к папке или файлу - используется для создания ссылок
                    std::wstring upDir = query;
                    upDir += _T("/");
                    upDir += *dir;
                    OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */));

                    //  переход по каталогу на один уровень вверх
                    InsertRowCommandButton(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */, _T("folder.bmp"), _hintFolder.c_str());

                    //  вывод названия папки
                    _tprintf(_T("<td width=\"100%%\">%s</td>"), dir->c_str());
                    InsertRowCommandButton(cmd_del_folder_conf, upDir.c_str(), dir->c_str(), _T("delete_folder.bmp"), HintDelete.c_str());
                    InsertRowCommandButton(cmd_edit_folder_conf, upDir.c_str(), dir->c_str(), _T("edit_folder.bmp"), HintEdit.c_str());
                    CloseInnerTableRow();
                }
            }
        }
    }

    void Page::PrintFileList(std::vector<std::wstring> files)
    {
        for (std::vector<std::wstring>::iterator fileName = files.begin(); fileName != files.end(); ++fileName)
        {
            //  Выводит строку со ссылкой.
            PrintFileRow(*fileName);
        }
    }

    void Page::PrintFolders()
    {
        OpenInnerTable();
        PrintDirList(ReadFolders());
        CloseInnerTable();
    }

    void Page::PrintFiles()
    {
        OpenInnerTable();
        PrintFileList(ReadFiles());
        CloseInnerTable();
    }

    void Page::PrintFileNameWithoutExt(std::wstring fileName)
    {
        // делаем название из имени файла без расширения
        auto pointPos = fileName.rfind(_T("."));
        std::wstring fileNameWithoutExt = pointPos != std::string::npos ? fileName.substr(0, pointPos) : fileName;
        _tprintf(_T("<td width=\"100%%\">%s</td>\n"), fileNameWithoutExt.c_str());
    }

    //  Выводит строку со ссылкой.
    void Page::PrintFileRow(std::wstring fileName)
    {
        //  !!! нужно скопировать расширение непосредственно
        //  из lineptr, чтобы сохранились исходные символы !!!
        //  воостанавливается
        Bookmarks::FileReader fr(cwd);
        std::wstring url = fr.GetParamCurDir(fileName, ParamURL);
#ifdef EXTENDED_URL_FILE
        std::wstring name = fr.GetParamCurDir(fileName, ParamName);
#endif
        OpenInnerFileTableRow();

        if (!url.empty())
        {   //  vstavlyaetsya vneschnyaya ssylka
            InsertLinkButton(_T("link.bmp"), url, _T(""), 16, _T("Ссылка"));
        }
        else
            //  ??? здесь нужна простая иконка
            InsertLinkButton(_T("error.bmp"), _T(""), _T("невозможно прочитать URL из файла!"), 16, _T("Ошибка: "));
#ifdef EXTENDED_URL_FILE
        //  вывод имени ссылки
        if (!name.empty())
        {
            _tprintf(_T("<td width=\"100%%\">%s</td>\n"), name.c_str());
        }
        else
#endif
            PrintFileNameWithoutExt(fileName);
        //  вставка иконок
        InsertRowCommandButton(cmd_del_conf, query, fileName.c_str(), _T("delete_link.bmp"), HintDelete.c_str());
        InsertRowCommandButton(cmd_edit_conf, query, fileName.c_str(), _T("edit_link.bmp"), HintEdit.c_str());
        CloseInnerTableRow();
    }

    //  при вызове заголовок страницы уже выведен, поэтому
    //  нужно сделать работу настолько, насколько это возможно
    void Page::Render()
    {
        PrintHead(_T("Избранные Ссылки"));
        OpenOuterTable();
        PrintFolders();
        PrintFiles();
        //  закрывается строка внешней таблицы и внешняя таблица
        CloseOuterTable();
        InsertAddButtons();
        PrintTail();
    }

    void Page::PrintInfo()
    {
#if _DEBUG
        std::vector<std::wstring> params = {
            _T("SERVER_SOFTWARE"), _T("SERVER_NAME"), _T("SCRIPT_NAME"),
            _T("REQUEST_METHOD"), _T("QUERY_STRING"), _T("DOCUMENT_ROOT")
        };

        _tprintf(_T("%s"), _T("<br><hr><br>\n"));
        for (auto par = params.begin(); par != params.end(); ++par)
            _tprintf(_T("%s = %s<br>\n"), par->c_str(), _wgetenv(par->c_str()));

        _tprintf(_T("HOME = %s<br>\n"), cwd);
        _tprintf((DocumentRoot.substr(0, DocumentRoot.rfind('/')) + _T("\n")).c_str());
        _tprintf(_T("Error = %i<br>\n"), error);
#endif
    }

    void Page::PrintHtmlHead(TCHAR *title)
    {
        Page p;
        p.PrintHead(title);
    }

    void Page::PrintHtmlTail()
    {
        Page p;
        p.PrintTail();
    }
}
