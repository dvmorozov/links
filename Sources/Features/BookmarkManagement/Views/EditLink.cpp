
#include "App/stdafx.h"

#include "EditLink.h"
#include "Shared/Controllers/Commands.h"

namespace Bookmarks
{
EditLink::EditLink(const std::wstring &name, const std::wstring &url,
                   const std::wstring &link_file_name)
    : _name(name), _url(url), _linkFileName(link_file_name)
{
    //  TODO: create base class Dialog having this attribute set by default.
    _cssClass = L"dialog";
}

EditLink::~EditLink() {}

void EditLink::Render()
{
    PrintHead(L"Edit Link");

    //  TODO: take labels from resource file.
    const std::wstring form_edit_link = L"\
    <form method=get action=\"%ls\">\n\
        <div class=\"input_box\">\n\
            <div class=\"label\">Title</div>\n\
            <input type=\"text\" size=50 value=\"%ls\" name=\"%ls;edit\"><br>\n\
            <div class=\"label\">URL</div>\n\
            <div class=\"label\"><a href=\"%ls\">%ls</a></div>\n\
        </div>\n\
        <div class=\"buttons\">\n\
            <input type=\"submit\" value=\"Cancel\" name=\";key=%ls;edit\">\n\
            <input type=\"submit\" value=\"Ok\" name=\";edit=%ls;key=%ls;edit\">\n\
        </div>\n\
    </form>\n\
    ";

    wprintf(form_edit_link.c_str(), full_script_name.c_str(), _name.c_str(), query, _url.c_str(),
            _url.c_str(), key, _linkFileName.c_str(), key);

    PrintTail();
}
}  // namespace Bookmarks
