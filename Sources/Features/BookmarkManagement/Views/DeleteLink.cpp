
#include "App/stdafx.h"

#include "DeleteLink.h"
#include "Shared/Controllers/Commands.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
DeleteLink::DeleteLink(const std::wstring &name, const std::wstring &url,
                       const std::wstring &link_file_name)
    : _name(name), _url(url), _linkFileName(link_file_name)
{
    _cssClass = L"dialog";
}

void DeleteLink::Render()
{
    PrintHead(L"Delete Link");

    //  TODO: take labels from resource file.
    const std::wstring form_delete_link = L"\
    <form method=get action=\"%ls\">\n\
        <div class=\"input_box\">\n\
            <div class=\"label\">Do you really want to delete the link</div>\n\
            <div class=\"label\"><font color=\"red\">%ls</font>, pointing to</div>\n\
            <div class=\"label\"><font color=\"red\">%ls</font>?</div>\n\
        </div>\n\
        <div class=\"buttons\">\n\
            <input id=\"Reset1\" type=\"submit\" value=\"Cancel\" name=\"%ls;del=%ls;key=%ls;del\">\n\
            <input id=\"Submit1\" type=\"submit\" value=\"Ok\" name=\"%ls;del=%ls;key=%ls;del\">\n\
        </div>\n\
    </form>\n\
    ";

    wprintf(form_delete_link.c_str(), full_script_name.c_str(), _name.c_str(), _url.c_str(), query,
            _linkFileName.c_str(), key, query, _linkFileName.c_str(), key);

    PrintTail();
}
}  // namespace Bookmarks
