
#include "App/stdafx.h"

#include "AddDirectory.h"
#include "Shared/Controllers/Commands.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
AddDirectory::AddDirectory() { _cssClass = L"dialog"; }

void AddDirectory::Render()
{
    PrintHead(_T("Create Folder"));

    //  TODO: take labels from resource file.
    const std::wstring form_add_directory = L"\
    <form method=get action=\"%ls\">\n\
        <div class=\"input_box\">\n\
            <div class=\"label\">Folder Name</div>\n\
            <input type=\"text\" size=50 value=\"\" name=\"%ls;add_folder\"><br>\n\
        </div>\n\
        <div class=\"buttons\">\n\
            <input type=\"submit\" value=\"Cancel\" name=\";key=%ls;add_folder\">\n\
            <input type=\"submit\" value=\"Ok\" name=\";key=%ls;add_folder\">\n\
        </div>\n\
    </form>\n\
    ";

    wprintf(form_add_directory.c_str(), full_script_name.c_str(), query, key, key);

    PrintTail();
}
}  // namespace Bookmarks
