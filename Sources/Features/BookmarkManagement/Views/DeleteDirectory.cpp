
#include "App/stdafx.h"

#include "DeleteDirectory.h"
#include "Shared/Controllers/Commands.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
DeleteDirectory::DeleteDirectory(const std::wstring &directoryName) : _directoryName(directoryName)
{
    _cssClass = L"dialog";
}

void DeleteDirectory::Render()
{
    PrintHead(_T("Delete Folder"));

    //  TODO: take labels from resource file.
    const std::wstring form_delete_directory = L"\
    <form method=get action=\"%ls\">\n\
        <div class=\"input_box\">\n\
           <div class=\"label\">Do you really want to delete the folder</div>\n\
           <div class=\"label\"><font color=\"red\">%ls</font> and all its contents?</div>\n\
        </div>\n\
        <div class=\"buttons\">\n\
            <input id=\"Reset1\" type=\"submit\" value=\"Cancel\" name=\"%ls;key=%ls;del_folder\">\n\
            <input id=\"Submit1\" type=\"submit\" value=\"Ok\" name=\"%ls;del_folder=%ls;key=%ls;del_folder\">\n\
        </div>\n\
    </form>\n\
    ";

    wprintf(form_delete_directory.c_str(), full_script_name.c_str(), _directoryName.c_str(), query,
            key, query, _directoryName.c_str(), key);

    PrintTail();
}
}  // namespace Bookmarks
