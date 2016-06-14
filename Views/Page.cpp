
#include "Page.h"
#include "../Models/FileReader.h"
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
                    _tprintf(_T("\
                            <a href=\"%s?%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), url.c_str(), image_path.c_str(), alt.c_str(), size, size);
                else
                    //  передается ссылка на внешний ресурс; script_name содержит ссылку к ресурсу
                    _tprintf(_T("\
                            <a href=\"%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), image_path.c_str(), alt.c_str(), size, size);
            }
            else
            {// вставляется иконка без ссылки
                if (!url.empty()) //  в url передается дополнение к тексту, переданному через alt
                    _tprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), url.c_str(), size, size);
                else
                    _tprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), size, size);
            }
        }
    }

    void Page::PrintHead(std::wstring title)
    {
        _tprintf(_T("%s"), _T("<html>"));
        _tprintf(_T("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><meta http-equiv=\"Content-Language\" content=\"ru\">\n"));
        _tprintf(_T("<title> %s </title></head>\n"), title.c_str());
        _tprintf(_T("<body><br>\n"));
    }

    void Page::PrintTail()
    {
        print_info();
        _tprintf(_T("%s"), _T("</body></html>\n"));
    }

    void Page::InsertRowCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint)
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

    void Page::InsertCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint)
    {
        std::wstring command;
        if (dir) command += dir;
        if (cmd_key) command += cmd_key;
        if (key) command += key;
        if (cmd) command += cmd;
        if (file) command += file;

        InsertButton(image_file, full_script_name ? full_script_name : _T(""), command, 16, hint ? hint : _T(""));
    }

    //  вызывает внешнюю утилиту и читает список файлов
    std::vector<std::wstring> Page::read_folders()
    {
        Bookmarks::Data fl;
        std::vector<Bookmarks::File> files = fl.ReadFileList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->Name);
        return result;
    }

    void Page::insert_add_buttons()
    {
        _tprintf(_T("\n<p>\n"));
        InsertCommandButton(cmd_add_conf, query, _T(""), _T("add_link.bmp"), _T("Создать ссылку"));
        InsertCommandButton(cmd_add_folder_conf, query, _T(""), _T("add_folder.bmp"), _T("Создать папку"));
        _tprintf(_T("\n</p>\n"));
    }

    //  выводит разделитель
    void Page::PrintRowTag()
    {
        if (FirstFolder)
        {
            if (!FirstLink)
                //  закрываем вложенную таблицу и ячейку внешней таблицы
                CloseInnerTable();

            //  открываем ячейку внешней таблицы, создаем вложенную таблицу,
            //  открываем строку и ячейку вложенной таблицы
            OpenInnerTable();
            FirstFolder = false;
            FirstLink = true;
        }
        else
            OpenInnerTableRow();
    }

    void Page::CloseInnerTable()
    {
        _tprintf(_T("%s"), _T("\n\
            </table>\n\
        </td>"));
    }

    void Page::OpenInnerTable()
    {
        _tprintf(_T("%s"), _T("\n\
        <td valign=\"top\">\n\
            <table border=\"0\" cellspacing=\"0\" cellpadding=\"1\">\n\
                <tr>\n\
                    <td>\n"));
    }

    void Page::OpenInnerTableRow()
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
        //  создается внешняя таблица, в кот. помещаются таблицы-колонки
        _tprintf(_T("\n\
<table border=\"1\" cellspacing=\"0\" cellpadding=\"0\">\n\
<tr>\n\
<th>%s\n\
</th>\n\
<th>%s [%s]\n\
</th>\n\
</tr>\n"), _T("Папки"), _T("Ссылки"), query);
    }

    //  при вызове заголовок страницы уже выведен, поэтому
    //  нужно сделать работу настолько, насколько это возможно
    void Page::print_folders()
    {
        OpenOuterTable();

        std::vector<std::wstring> dirs = read_folders();
        for (std::vector<std::wstring>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
        {
            std::wstring s = *dir;
            s.resize(MAX_LINE_LENGTH - 1);
            //  сохранение совместимости со старым способом работы
            TCHAR lineinp[MAX_LINE_LENGTH];
            _tcscpy(lineinp, s.c_str());

            int n = _tcslen(lineinp);
            if (n)
            {
                TCHAR lineptr[MAX_LINE_LENGTH];
                //  выводятся только имена url-файлов или имена папок
                unsigned char folder = 0;
                _tcscpy(lineptr, lineinp);

                while ((lineptr[n - 1] == '\x0a') || (lineptr[n - 1] == '\x0d')
#ifdef LINUX
                    || (lineptr[n - 1] == '*')
#endif
                    ) n--;
                lineptr[n] = 0;         //  удаляем из строки все спец. симв. => 
                                        //  строка содержит имя файла
#ifdef USE_CYGWIN || LINUX
                if (lineptr[n - 1] != '/')
                {// imya fayla
                 //  proverka dopusimosti rasschireniya
                    if (!(
                        ((lineptr[n - 1] == 'l') || (lineptr[n - 1] == 'L')) &&
                        ((lineptr[n - 2] == 'r') || (lineptr[n - 2] == 'R')) &&
                        ((lineptr[n - 3] == 'u') || (lineptr[n - 3] == 'U')) &&
                        (lineptr[n - 4] == '.')
                        ))continue;
                    //  imya fayla s rasschireniem
                }
                else
                {// imay papki
                    lineptr[n - 1] = 0;
                    folder = 1;
                }
#else
                                        //  проверка расширения
                if ((
                    ((lineptr[n - 1] == 'l') || (lineptr[n - 1] == 'L')) &&
                    ((lineptr[n - 2] == 'r') || (lineptr[n - 2] == 'R')) &&
                    ((lineptr[n - 3] == 'u') || (lineptr[n - 3] == 'U')) &&
                    (lineptr[n - 4] == '.')
                    ))
                {// imya fayla s rasschireniem
                }
                else
                {
                    if ((lineptr[n - 1] == '$') && (lineptr[n - 2] == '$') &&
                        (lineptr[n - 3] == '$') && (lineptr[n - 4] == '.'))
                    {
                        // imya sluzhebnogo fayla
                        continue;
                    }
                    else
                    {
                        // imya papki
                        folder = 1;
                    }
                }
#endif
                // для корневой папки на вставляются ссылки "ВВЕРХ" и "КОРЕНЬ"
                if (!_tcscmp(lineptr, _T(".")))
                {// переход к корневой папке закладок
                    if (_tcslen(query))
                    {
                        PrintRowTag();
                        InsertRowCommandButton(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */, _T("to_start_page.bmp"), hint_folder.c_str());
                        _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), home.c_str()/*название*/);
                        CloseInnerTableRow();
                    }
                }
                else
                    if (!_tcscmp(lineptr, _T("..")))
                    {// переход на один уровень вверх или к корневой папке закладок
                        if (_tcslen(query))
                        {
                            TCHAR *temp = (TCHAR *)malloc((_tcslen(query) + 1) * sizeof(TCHAR));
                            PrintRowTag();

                            if (temp)
                            {
                                TCHAR *up_dir;
                                _tcscpy(temp, query);
                                up_dir = (TCHAR*)_tcsrchr(temp, '/');
                                if (up_dir)
                                {
                                    *up_dir = 0;
                                    //  переход к самому верхнему уровню каталога
                                    InsertRowCommandButton(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */, _T("to_upper_folder.bmp"), hint_folder.c_str());
                                    _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _T("ВВЕРХ"));
                                    CloseInnerTableRow();
                                }
                                // верхняя папка является корневой - ссылка не вставляется
                                free(temp);
                            }
                            /*  переход к корневой папке уже вставляется выше */
                        }
                    }
                    else
                        //  если текущий запрос не равен ".", ".." или 0, то
                        //  строка запроса содержит имя текущей папки, которое нужно
                        //  добавлять к именам найденных файлов для того, чтобы
                        //  при последующем запросе произошел переход в нужную папку
                    {
                        if (_tcscmp(query, _T(".")) && _tcscmp(query, _T("..")))
                        {
                            //  извлекается URL данного модуля и создаются ссылки
                            if (folder)
                            {
                                //  full_dir в результате содержит url относительно url скрипта 
                                //  к папке или файлу - используется для создания ссылок
                                TCHAR *full_dir = lineptr;
                                if (_tcslen(query))
                                {
                                    full_dir = (TCHAR *)malloc((_tcslen(query) + _tcslen(lineptr) + 2) * sizeof(TCHAR));
                                    if (full_dir)
                                    {
                                        _tcscpy(full_dir, query);
                                        _tcscat(full_dir, _T("/"));
                                        _tcscat(full_dir, lineptr);
                                    }
                                    /*
                                    else
                                    {// oshibka fatal'na - nichego ne vyvoditsya,
                                    // poskol'ku zagolovok stranitsy uzhe vyveden
                                    }
                                    */
                                }

                                PrintRowTag();
                                if (full_dir)
                                    //  переход по каталогу на один уровень вверх
                                    InsertRowCommandButton(cmd_ch_folder, full_dir/* url */, ok/* to, chto posle komandy */, _T("folder.bmp"), hint_folder.c_str());

                                _tprintf(_T("<td width=\"100%%\">%s</td>"), lineptr);
                                InsertRowCommandButton(cmd_del_folder_conf, query, lineptr, _T("delete_folder.bmp"), hint_delete.c_str());
                                InsertRowCommandButton(cmd_edit_folder_conf, query, lineptr, _T("edit_folder.bmp"), hint_edit.c_str());
                                CloseInnerTableRow();
                                if (full_dir != lineptr)free(full_dir);
                            }
                            else
                            {
                                //  !!! нужно скопировать расширение непосредственно
                                //  из lineptr, чтобы сохранились исходные символы !!!
                                //  воостанавливается
                                Bookmarks::FileReader fr(cwd);
                                std::wstring url = fr.GetParamCurDir(lineptr, str_url);
#ifdef EXTENDED_URL_FILE
                                std::wstring name = fr.GetParamCurDir(lineptr, str_name);
#endif

                                if (FirstLink)
                                {
                                    if (!FirstFolder)
                                        //  закрываем вложенную таблицу и ячейку внешней таблицы
                                        _tprintf(_T("%s"), _T("\
    </table>\n\
</td>\n"));
                                    //  открываем ячейку внешней таблицы, создаем вложенную таблицу,
                                    //  открываем строку и ячейку вложенной таблицы
                                    OpenInnerTable();
                                    FirstLink = false;
                                    FirstFolder = true;
                                }
                                else
                                    OpenInnerTableRow();

                                if (!url.empty())
                                {   //  vstavlyaetsya vneschnyaya ssylka
                                    InsertLinkButton(_T("link.bmp"), url, _T(""), 16, _T("Ссылка"));
                                }
                                else
                                    //  ??? здесь нужна простая иконка
                                    InsertLinkButton(_T("error.bmp"), _T(""), _T("невозможно прочитать URL из файла!"), 16, _T("Ошибка: "));
#ifdef EXTENDED_URL_FILE
                                if (!name.empty())
                                    _tprintf(_T("<td width=\"100%%\">%s</td>\n"), name.c_str());
                                else
#endif
                                {// делаем название из имени файла без расширения
                                    TCHAR *dot = (TCHAR*)_tcsrchr(lineptr, '.');
                                    if (dot)*dot = 0;
                                    _tprintf(_T("<td width=\"100%%\">%s</td>\n"), lineptr);
                                    if (dot)*dot = '.';  //  восстановление имени файла
                                }
                                //  вставка иконок
                                InsertRowCommandButton(cmd_del_conf, query, lineptr, _T("delete_link.bmp"), hint_delete.c_str());
                                InsertRowCommandButton(cmd_edit_conf, query, lineptr, _T("edit_link.bmp"), hint_edit.c_str());
                                CloseInnerTableRow();
                            }
                        }
                    }
            }
        }

        if (!FirstFolder || !FirstLink)
            //  закрывается вложенная таблица и ячейка внешней таблицы
            _tprintf(_T("%s"), _T("\
    </table>\n\
</td>\n"));
        //  закрывается строка внешней таблицы и внешняя таблица
        _tprintf(_T("%s"), _T("\
</tr>\n\
</table>\n"));
    }


    void Page::Render()
    {
        PrintHead(_T("Избранные Ссылки"));
        print_folders();
        insert_add_buttons();
        PrintTail();
    }

    void Page::print_info()
    {
#if _DEBUG
        std::vector<std::wstring> params = {
            _T("SERVER_SOFTWARE"), _T("SERVER_NAME"), _T("SCRIPT_NAME"),
            _T("REQUEST_METHOD"), _T("QUERY_STRING"), _T("DOCUMENT_ROOT")
        };

        _tprintf(_T("%s"), _T("<br><hr><br>\n"));
        for (auto par = params.begin(); par != params.end(); ++par)
            _tprintf(_T("%s = %s<br>\n"), par->c_str(), _tgetenv(par->c_str()));

        _tprintf(_T("HOME = %s<br>\n"), cwd);
        _tprintf((document_root.substr(0, document_root.rfind('/')) + _T("\n")).c_str());
        _tprintf(_T("Error = %i<br>\n"), error);
#endif
    }

    void Page::print_html_head(TCHAR *title)
    {
        Page p;
        p.PrintHead(title);
    }

    void Page::print_html_tail()
    {
        Page p;
        p.PrintTail();
    }
}
