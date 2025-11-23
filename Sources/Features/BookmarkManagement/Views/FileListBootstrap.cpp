
#include "App/stdafx.h"

#include "FileListBootstrap.h"
#include "Shared/Controllers/Commands.h"
#include "../Models/LinkFileReader.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
FileListBootstrap::FileListBootstrap() {}

void FileListBootstrap::InsertButton(std::wstring imageFile, const std::wstring endpointUrl,
                                     const std::wstring url, int size, const std::wstring alt)
{
    std::map<std::wstring, std::wstring> styles = {
        {_T("link.bmp"), _T("glyphicon glyphicon-link")},
        {_T("error.bmp"), _T("glyphicon glyphicon-ban-circle")},
        {_T("folder.bmp"), _T("glyphicon glyphicon-folder-open")},
        {_T("edit_link.bmp"), _T("glyphicon glyphicon-edit")},
        {_T("delete_link.bmp"), _T("glyphicon glyphicon-remove")},
        {_T("edit_folder.bmp"), _T("glyphicon glyphicon-edit")},
        {_T("delete_folder.bmp"), _T("glyphicon glyphicon-remove")},
        {_T("to_upper_folder.bmp"), _T("glyphicon glyphicon-arrow-up")},
        {_T("to_start_page.bmp"), _T("glyphicon glyphicon-home")},
        {_T("add_link.bmp"), _T("glyphicon glyphicon-link")},
        {_T("add_folder.bmp"), _T("glyphicon glyphicon-folder-open")}};

    auto buttonScript =
        imageFile == _T("link.bmp") ?
                                    //  Link is opened in new tab.
            _T("<button type=\"button\" class=\"btn btn-default\" style=\"min-height: 34px; min-width: 40px;\" aria-label=\"%ls\" onclick=\"window.open('%ls', '_blank'); if(event.stopPropagation) event.stopPropagation(); else event.cancelBubble=true;\">\
                    <span class=\"%ls\" aria-hidden=\"true\"></span>\
                </button>")
                                    :
                                    //  Non-clickable button with error message.
            _T("<button type=\"button\" class=\"btn btn-default\" style=\"min-height: 34px; min-width: 40px;\" aria-label=\"%ls\" onclick=\"window.location.href='%ls'; if(event.stopPropagation) event.stopPropagation(); else event.cancelBubble=true;\">\
                    <span class=\"%ls\" aria-hidden=\"true\"></span>\
                </button>");

    wprintf(buttonScript, (alt + url).c_str(),
            (!endpointUrl.empty() ? (!url.empty() ? endpointUrl + _T("?") + url : endpointUrl)
                                  : _T("#"))
                .c_str(),
            (styles.count(imageFile) ? styles[imageFile] : std::wstring()).c_str());
}

void FileListBootstrap::PrintStyles()
{
    FileListLegacy::PrintStyles();

    wprintf(
        _T("<link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\" ")
        _T("rel=\"stylesheet\">\n"));
    wprintf(_T("<link ")
            _T("href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/")
            _T("bootstrap-theme.min.css\" rel=\"stylesheet\">\n"));
    wprintf(_T("<link ")
            _T("href=\"https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.2/themes/smoothness/")
            _T("jquery-ui.css\" rel=\"stylesheet\">\n"));
    wprintf(_T("<style type=\"text/css\">\
            .trlist {\
                border-radius: 4px;\
                color: #2A6496;\
            }\n\
            .trlist:hover{\
                background-color: #eeeeee;\
                cursor: pointer;\
            }</style>\n"));
}

void FileListBootstrap::PrintScripts()
{
    FileListLegacy::PrintScripts();

    wprintf(_T("<script ")
            _T("src=\"https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.2/jquery-ui.min.js\" ")
            _T("type=\"text/javascript\"></script>\n"));
    wprintf(
        _T("<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js\" ")
        _T("type=\"text/javascript\"></script>\n"));
}

void FileListBootstrap::OpenOuterTable()
{
    wprintf(_T("\n\
<table class=\"table\">\n\
    <tbody>\n\
        <tr>\n\
            <th>%ls\n\
            </th>\n\
            <th>%ls [%ls]\n\
            </th>\n\
        </tr><tr>\n"),
            _T("Folders"), _T("Links"), query);
}

void FileListBootstrap::OpenInnerTable()
{
    wprintf(_T("<td valign=\"top\">\n\
                <table class=\"table table-striped\">\n\
                    <tbody>"));
}

void FileListBootstrap::OpenInnerFileTableRow(std::wstring url)
{
    wprintf(_T("<tr class=\"trlist\" onclick=\"window.open('%ls', '_blank');\">"), url.c_str());
}

void FileListBootstrap::OpenInnerDirTableRow(std::wstring url)
{
    wprintf(_T("<tr class=\"trlist\" onclick=\"window.location.href='%ls'\">"), url.c_str());
}

//  Renders a row with a link.
void FileListBootstrap::PrintFileRow(const File &file)
{
    file.Url.empty() ? OpenInnerFileTableRow() : OpenInnerFileTableRow(file.Url);

    //  The row looks better with button. The button is also clickable.
    if (!file.Url.empty()) {  //  Link button is inserted.
        InsertTableLinkButton(_T("link.bmp"), file.Url, _T(""), 16, _T("Link"));
    } else
        //  TODO: just a simple icon here, not a link.
        InsertTableLinkButton(_T("error.bmp"), _T(""), _T("Cannot read URL from file!"), 16,
                              _T("Error: "));

    wprintf(_T("<td>%ls</td>\n"), GetLinkName(file).c_str());

    //  Renders command buttons.
    InsertTableCommandButton(cmd_delete_conf, query, file.FileName.c_str(), _T("delete_link.bmp"),
                             HintDelete.c_str());
    InsertTableCommandButton(cmd_edit_conf, query, file.FileName.c_str(), _T("edit_link.bmp"),
                             HintEdit.c_str());
    CloseInnerTableRow();
}
}  // namespace Bookmarks
