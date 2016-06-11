#include "PageBootstrap.h"
#include <map>

#include "../main.h"

namespace Bookmarks
{
    PageBootstrap::PageBootstrap()
    {
    }


    PageBootstrap::~PageBootstrap()
    {
    }

    void PageBootstrap::InsertButton(std::string image_file, const std::string script_name, const std::string url, int size, const std::string alt)
    {
        std::map<std::string, std::string> styles = { 
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
        ), (alt + url).c_str(), (!script_name.empty() ? (!url.empty() ? script_name + "?" + url : script_name) : "#").c_str(), (styles.count(image_file) ? styles[image_file] : std::string()).c_str());
    }

    void PageBootstrap::PrintHead(std::string title)
    {
        _tprintf(_T("%s"), _T("<html>"));
        _tprintf(_T("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><meta http-equiv=\"Content-Language\" content=\"ru\">\n"));
        _tprintf(_T("<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\" rel=\"stylesheet\"/>\n"));
        _tprintf(_T("<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css\" rel=\"stylesheet\"/>\n"));
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
        printf(_T("\n\
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
