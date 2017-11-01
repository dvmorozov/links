
#include "FileListLegacy.h"
#include "../Models/FileReader.h"
#include "../Controllers/Commands.h"
#include "../Utils/entities.h"
#include "../main.h"

namespace Bookmarks
{
    FileListLegacy::FileListLegacy()
    {
    }

    FileListLegacy::~FileListLegacy()
    {
    }

    std::wstring FileListLegacy::GetImagePath(std::wstring image_file)
    {
        if (!img_path)
            return _T("");
        return std::wstring(img_path) + image_file;
    }

    void FileListLegacy::InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        std::wstring image_path = GetImagePath(image_file);

        if (!image_path.empty())
        {
            if (!script_name.empty())
            {
                if (!url.empty())
                {
                    wprintf(_T("\
                            <a href=\"%s?%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), url.c_str(), image_path.c_str(), alt.c_str(), size, size);
                }
                else
                {
                    //  передаетс¤ ссылка на внешний ресурс; script_name содержит ссылку к ресурсу
                    wprintf(_T("\
                            <a href=\"%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), image_path.c_str(), alt.c_str(), size, size);
                }
            }
            else
            {// вставл¤етс¤ иконка без ссылки
                if (!url.empty()) //  в url передаетс¤ дополнение к тексту, переданному через alt
                {
                    wprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), url.c_str(), size, size);
                }
                else
                {
                    wprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), size, size);
                }
            }
        }
    }

    void FileListLegacy::InsertRowCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, const TCHAR *image_file, const TCHAR* hint)
    {
        wprintf(_T("<td>"));
        InsertCommandButton(cmd, dir, file, image_file, hint);
        wprintf(_T("</td>"));
    }

    void FileListLegacy::InsertLinkButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        wprintf(_T("<td>"));
        InsertButton(image_file, script_name, url, 16, alt);
        wprintf(_T("</td>"));
    }

    std::wstring FileListLegacy::GetCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file)
    {
        std::wstring command;
        if (dir) command += dir;
        if (cmd_key) command += cmd_key;
        if (key) command += key;
        if (cmd) command += cmd;
        if (file) command += file;
        return command;
    }

    std::wstring FileListLegacy::GetFullCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file)
    {
        return std::wstring(full_script_name ? full_script_name : _T("")) + _T("?") + GetCommandUrl(cmd, dir, file);
    }

    void FileListLegacy::InsertCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, const TCHAR *image_file, const TCHAR* hint)
    {
        InsertButton(image_file, full_script_name ? full_script_name : _T(""), GetCommandUrl(cmd, dir, file), 16, hint ? hint : _T(""));
    }

    //  вызывает внешнюю утилиту и читает список файлов
    std::vector<std::wstring> FileListLegacy::ReadFolders()
    {
        FileVector files = _data.GetDirList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->FileName);
        return result;
    }

    std::vector<std::wstring> FileListLegacy::ReadFiles()
    {
        FileVector files = _data.GetFileList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->FileName);
        return result;
    }

    void FileListLegacy::InsertAddButtons()
    {
        wprintf(_T("\n<p>\n"));
        InsertCommandButton(cmd_add_conf, query, _T(""), _T("add_link.bmp"), _T("—оздать ссылку"));
        InsertCommandButton(cmd_add_folder_conf, query, _T(""), _T("add_folder.bmp"), _T("—оздать папку"));
        wprintf(_T("\n</p>\n"));
    }

    void FileListLegacy::OpenInnerTable()
    {
        //  Opens new cell for internal table.
        wprintf(_T("<td valign=\"top\">\n\
            <table border=\"0\" cellspacing=\"0\" cellpadding=\"1\">\n\
                <tbody>"));
    }

    void FileListLegacy::CloseInnerTable()
    {
        wprintf(_T("</tbody>\n\
                </table>\n\
            </td>"));
    }

    //  https://action.mindjet.com/task/14720269
    void FileListLegacy::OpenInnerFileTableRow(std::wstring url)
    {
        wprintf(_T("<tr>"));
    }

    //  https://action.mindjet.com/task/14720269
    void FileListLegacy::OpenInnerDirTableRow(std::wstring url)
    {
        wprintf(_T("<tr>"));
    }

    //  закрываетс¤ ¤чейка и строка внутренней таблицы
    void FileListLegacy::CloseInnerTableRow()
    {
        wprintf(_T("</tr>"));
    }

    void FileListLegacy::OpenOuterTable()
    {
        //  Creates outer table with 2 columns for folders and files.
        //  Adds new row for internal tables.
        wprintf(_T("\n\
<table border=\"1\" cellspacing=\"0\" cellpadding=\"0\">\n\
    <tbody>\n\
        <tr>\n\
            <th>%s\n\
            </th>\n\
            <th>%s [%s]\n\
            </th>\n\
        </tr><tr>\n"), _T("ѕапки"), _T("—сылки"), query);
    }

    void FileListLegacy::CloseOuterTable()
    {
        //  For the first closes internal table row.
        wprintf(_T("</tr>\n\
    </tbody>\n\
</table>\n"));
    }

    void FileListLegacy::PrintDirList(std::vector<std::wstring> dirs)
    {
        for (std::vector<std::wstring>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
        {
            if (dir->size())
            {
                // дл¤ корневой папки на вставл¤ютс¤ ссылки "¬¬≈–’" и " ќ–≈Ќ№"
                if (*dir == _T("."))
                {// переход к корневой папке закладок
                    if (wcslen(query))
                    {
                        OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */));
                        InsertRowCommandButton(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */, _T("to_start_page.bmp"), _hintFolder.c_str());
                        wprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _home.c_str()/*название*/);
                        CloseInnerTableRow();
                    }
                }
                else
                if (*dir == _T(".."))
                {// переход на один уровень вверх или к корневой папке закладок
                    if (wcslen(query))
                    {
                        std::wstring upDir = query;
                        //  ѕолучаем им¤ верхней папки.
                        auto slashPos = upDir.rfind(_T("/"));
                        upDir = slashPos != std::string::npos ? upDir.substr(0, slashPos) : upDir;

                        OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */));
                        //  ѕереход к самому верхнему уровню каталога (даже если верх. папка ¤вл. корневой).
                        InsertRowCommandButton(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */, _T("to_upper_folder.bmp"), _hintFolder.c_str());
                        wprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _T("¬¬≈–’"));

                        CloseInnerTableRow();
                        /*  переход к корневой папке уже вставл¤етс¤ выше */
                    }
                }
                else
                    //  если текущий запрос не равен ".", ".." или 0, то
                    //  строка запроса содержит им¤ текущей папки, которое нужно
                    //  добавл¤ть к именам найденных файлов дл¤ того, чтобы
                    //  при последующем запросе произошел переход в нужную папку
                {
                    //  full_dir в результате содержит url относительно url скрипта 
                    //  к папке или файлу - используетс¤ дл¤ создани¤ ссылок
                    std::wstring upDir = query;
                    upDir += _T("/");
                    upDir += *dir;
                    OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */));

                    //  переход по каталогу на один уровень вверх
                    InsertRowCommandButton(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */, _T("folder.bmp"), _hintFolder.c_str());

                    //  вывод названи¤ папки
                    wprintf(_T("<td width=\"100%%\">%s</td>"), dir->c_str());
                    InsertRowCommandButton(cmd_del_folder_conf, upDir.c_str(), dir->c_str(), _T("delete_folder.bmp"), HintDelete.c_str());
                    InsertRowCommandButton(cmd_edit_folder_conf, upDir.c_str(), dir->c_str(), _T("edit_folder.bmp"), HintEdit.c_str());
                    CloseInnerTableRow();
                }
            }
        }
    }

    void FileListLegacy::PrintFileList(std::vector<std::wstring> files)
    {
        for (std::vector<std::wstring>::iterator fileName = files.begin(); fileName != files.end(); ++fileName)
        {
            //  ¬ыводит строку со ссылкой.
            PrintFileRow(*fileName);
        }
    }

    void FileListLegacy::PrintFolders()
    {
        OpenInnerTable();
        PrintDirList(ReadFolders());
        CloseInnerTable();
    }

    void FileListLegacy::PrintFiles()
    {
        OpenInnerTable();
        PrintFileList(ReadFiles());
        CloseInnerTable();
    }

    void FileListLegacy::PrintFileNameWithoutExt(std::wstring fileName)
    {
        // делаем название из имени файла без расширени¤
        auto pointPos = fileName.rfind(_T("."));
        std::wstring fileNameWithoutExt = pointPos != std::string::npos ? fileName.substr(0, pointPos) : fileName;
        wprintf(_T("<td width=\"100%%\">%s</td>\n"), fileNameWithoutExt.c_str());
    }

    //  ¬ыводит строку со ссылкой.
    void FileListLegacy::PrintFileRow(std::wstring fileName)
    {
        //  !!! нужно скопировать расширение непосредственно
        //  из lineptr, чтобы сохранились исходные символы !!!
        //  воостанавливаетс¤
        Bookmarks::FileReader fr(cwd);
        std::wstring url = fr.GetParamCurDir(fileName, ParamURL);
#ifdef EXTENDED_URL_FILE
        std::wstring name = fr.GetParamCurDir(fileName, ParamName);
#endif
        OpenInnerFileTableRow();

        if (!url.empty())
        {   //  vstavlyaetsya vneschnyaya ssylka
            InsertLinkButton(_T("link.bmp"), url, _T(""), 16, _T("—сылка"));
        }
        else
            //  ??? здесь нужна проста¤ иконка
            InsertLinkButton(_T("error.bmp"), _T(""), _T("невозможно прочитать URL из файла!"), 16, _T("ќшибка: "));
#ifdef EXTENDED_URL_FILE
        //  вывод имени ссылки
        if (!name.empty())
        {
            wprintf(_T("<td width=\"100%%\">%s</td>\n"), name.c_str());
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
    void FileListLegacy::Render()
    {
        PrintHead(_T("»збранные —сылки"));
        OpenOuterTable();
        PrintFolders();
        PrintFiles();
        //  закрываетс¤ строка внешней таблицы и внешн¤¤ таблица
        CloseOuterTable();
        InsertAddButtons();
        PrintTail();
    }
}
