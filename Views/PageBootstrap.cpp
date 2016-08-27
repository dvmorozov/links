#include "PageBootstrap.h"
#include <map>

#include "../main.h"
#include "../Controllers/Commands.h"
#include "../Models/FileReader.h"

namespace Bookmarks
{
    PageBootstrap::PageBootstrap()
    {
    }


    PageBootstrap::~PageBootstrap()
    {
    }

    void PageBootstrap::InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        std::map<std::wstring, std::wstring> styles = { 
            { _T("link.bmp"), _T("glyphicon glyphicon-link") },
            { _T("error.bmp"), _T("glyphicon glyphicon-ban-circle") },
            { _T("folder.bmp"), _T("glyphicon glyphicon-folder-open") },
            { _T("edit_link.bmp"), _T("glyphicon glyphicon-edit") },
            { _T("delete_link.bmp"), _T("glyphicon glyphicon-remove") },
            { _T("edit_folder.bmp"), _T("glyphicon glyphicon-edit") },
            { _T("delete_folder.bmp"), _T("glyphicon glyphicon-remove") },
            { _T("to_upper_folder.bmp"), _T("glyphicon glyphicon-arrow-up") },
            { _T("to_start_page.bmp"), _T("glyphicon glyphicon-home") },
            { _T("add_link.bmp"), _T("glyphicon glyphicon-link") },
            { _T("add_folder.bmp"), _T("glyphicon glyphicon-folder-open") }
        };

        _tprintf(_T(
                "<button type=\"button\" class=\"btn btn-default\" style=\"min-height: 34px; min-width: 40px;\" aria-label=\"%s\" onclick=\"window.location.href='%s';\">\
                    <span class=\"%s\" aria-hidden=\"true\"></span>\
                </button>"
        ), (alt + url).c_str(), (!script_name.empty() ? (!url.empty() ? script_name + _T("?") + url : script_name) : _T("#")).c_str(), (styles.count(image_file) ? styles[image_file] : std::wstring()).c_str());
    }

    void PageBootstrap::PrintHead(std::wstring title)
    {
        _tprintf(_T("%s"), _T("<html>"));
        _tprintf(_T("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><meta http-equiv=\"Content-Language\" content=\"ru\">\n"));
        _tprintf(_T("<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\" rel=\"stylesheet\">\n"));
        _tprintf(_T("<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css\" rel=\"stylesheet\">\n"));
        _tprintf(_T("<link href=\"https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.2/themes/smoothness/jquery-ui.css\" rel=\"stylesheet\">\n"));
        _tprintf(_T("<script src=\"https://code.jquery.com/jquery-2.1.3.js\" type=\"text/javascript\"></script>\n"));
        _tprintf(_T("<script src=\"https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.2/jquery-ui.min.js\" type=\"text/javascript\"></script>\n"));
        _tprintf(_T("<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js\" type=\"text/javascript\"></script>\n"));
        _tprintf(_T("<style type=\"text/css\">\
            .trlist {\
                border-radius: 4px;\
                color: #2A6496;\
            }\n\
            .trlist:hover{\
                background-color: #eeeeee;\
                cursor: pointer;\
            }</style>\n"));
        _tprintf(_T("<title> %s </title></head>\n"), title.c_str());
        _tprintf(_T("<body><br>\n"));
    }

    void PageBootstrap::PrintTail()
    {
        Page::PrintTail();
    }

    void PageBootstrap::OpenOuterTable()
    {
        _tprintf(_T("\n\
<table class=\"table\">\n\
    <tbody>\n\
        <tr>\n\
            <th>%s\n\
            </th>\n\
            <th>%s [%s]\n\
            </th>\n\
        </tr><tr>\n"), _T("Папки"), _T("Ссылки"), query);
    }

    void PageBootstrap::OpenInnerTable()
    {
        _tprintf(_T("<td valign=\"top\">\n\
                <table class=\"table table-striped\">\n\
                    <tbody>"));
    }

    //  https://action.mindjet.com/task/14720269
    void PageBootstrap::OpenInnerTableRow(std::wstring url)
    {
        _tprintf(_T("<tr class=\"trlist\" onclick=\"window.location.href='%s'\">"), url.c_str());
    }

    //  Выводит строку со ссылкой.
    void PageBootstrap::PrintLinkRow(TCHAR *lineptr)
    {
        //  !!! нужно скопировать расширение непосредственно
        //  из lineptr, чтобы сохранились исходные символы !!!
        //  воостанавливается
        Bookmarks::FileReader fr(cwd);
        std::wstring url = fr.GetParamCurDir(lineptr, ParamURL);
#ifdef EXTENDED_URL_FILE
        std::wstring name = fr.GetParamCurDir(lineptr, ParamName);
#endif
        //  https://action.mindjet.com/task/14720269
        url.empty() ? OpenInnerTableRow() : OpenInnerTableRow(url);

#ifdef EXTENDED_URL_FILE
        //  вывод имени ссылки
        if (!name.empty())
        {
            _tprintf(_T("<td width=\"100%%\">%s</td>\n"), name.c_str());
        }
        else
#endif
        {// делаем название из имени файла без расширения
            TCHAR *dot = (TCHAR*)_tcsrchr(lineptr, '.');
            if (dot)*dot = 0;
            _tprintf(_T("<td width=\"100%%\">%s</td>\n"), lineptr);
            if (dot)*dot = '.';  //  восстановление имени файла
        }
        //  вставка иконок
        InsertRowCommandButton(cmd_del_conf, query, lineptr, _T("delete_link.bmp"), HintDelete.c_str());
        InsertRowCommandButton(cmd_edit_conf, query, lineptr, _T("edit_link.bmp"), HintEdit.c_str());
        CloseInnerTableRow();
    }
}
