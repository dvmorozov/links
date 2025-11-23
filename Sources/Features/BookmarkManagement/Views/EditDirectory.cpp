
#include "App/stdafx.h"

#include "EditDirectory.h"
#include "Shared/Controllers/Commands.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
EditDirectory::EditDirectory(const std::wstring &directoryName) : _directoryName(directoryName)
{
    _cssClass = L"dialog";
}

void EditDirectory::Render()
{
    PrintHead(_T("Edit Folder"));

    //  TODO: take labels from resource file.
    const std::wstring form_edit_directory = L"\
    <form method=get action=\"%ls\">\n\
        <div class=\"input_box\">\n\
            <div class=\"label\">Folder Name</div>\n\
            <input type=\"text\" size=50 value=\"%ls\" name=\"%ls;edit_folder\"><br>\n\
        </div>\n\
        <div class=\"buttons\">\n\
            <input type=\"submit\" value=\"Cancel\" name=\";key=%ls;edit_folder\">\n\
            <input type=\"submit\" value=\"Ok\" name=\";edit_folder=%ls;key=%ls;edit_folder\">\n\
        </div>\n\
    </form>\n\
    ";

    wprintf(form_edit_directory.c_str(), full_script_name.c_str(), _directoryName.c_str(), query,
            key, _directoryName.c_str(), key);

    PrintTail();
}
}  // namespace Bookmarks
