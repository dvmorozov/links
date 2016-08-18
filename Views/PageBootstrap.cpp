#include "PageBootstrap.h"
#include <map>

#include "../main.h"
#include "../Controllers/Commands.h"

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
        _tprintf(_T("<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\" rel=\"stylesheet\"/>\n"));
        _tprintf(_T("<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css\" rel=\"stylesheet\"/>\n"));
        _tprintf(_T("<link href=\"https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.2/themes/smoothness/jquery-ui.css\" rel = \"stylesheet\"/>\n"));
        _tprintf(_T("<script src=\"https://code.jquery.com/jquery-2.1.3.js\"></script>\n"));
        _tprintf(_T("<script src=\"https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.2/jquery-ui.min.js\"></script>\n"));
        _tprintf(_T("<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js\"></script>\n"));
        _tprintf(_T("<title> %s </title></head>\n"), title.c_str());
        _tprintf(_T("<body><br>\n"));
    }

    void PageBootstrap::PrintTail()
    {
        Page::PrintTail();
    }

    void PageBootstrap::OpenOuterTable()
    {
        //  создаетс€ внешн€€ таблица, в кот. помещаютс€ таблицы-колонки (во внеш. т. 2 колонки)
        _tprintf(_T("\n\
<table class=\"table\">\n\
<tr>\n\
<th>%s\n\
</th>\n\
<th>%s [%s]\n\
</th>\n\
</tr>\n"), _T("ѕапки"), _T("—сылки"), query);
    }

    void PageBootstrap::CloseInnerTable()
    {
        _tprintf(_T("\n\
            </table>\n\
        </td>"));
    }

    void PageBootstrap::OpenInnerTable()
    {
        _tprintf(_T("\n\
        <td valign=\"top\">\n\
            <table class=\"table table-striped\">"));
    }
}
