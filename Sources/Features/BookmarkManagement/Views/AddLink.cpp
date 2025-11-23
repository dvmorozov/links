
#include "App/stdafx.h"

#include "AddLink.h"
#include "Utils/DebugLog.h"
#include "Utils/Miscellaneous.h"
#include "Shared/Controllers/Commands.h"

namespace Bookmarks
{
AddLink::AddLink() 
{ 
    DEBUG_LOG_FUNC();
    DEBUG_LOG("AddLink constructor called");
    _cssClass = L"dialog"; 
    DEBUG_LOG_FUNC_EXIT();
}

void AddLink::Render()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Rendering Add Link dialog");
    
    PrintHead(_T("Add link"));

    //  TODO: take labels from resource file.
    const std::wstring form_add_link = L"\
    <form method=get action=\"%ls\">\n\
        <div class=\"input_box\">\n\
            <div class=\"label\">Title</div>\n\
            <input type=\"text\" size=50 value=\"\" name=\"%ls;add\">\n\
            <div class=\"label\">URL</div>\n\
            <input type=\"text\" size=50 value=\"\" name=\";add\">\n\
        </div>\n\
        <div class=\"buttons\">\n\
            <input type=\"submit\" value=\"Cancel\" name=\";key=%ls;add\">\n\
            <input type=\"submit\" value=\"Ok\" name=\";key=%ls;add\">\n\
        </div>\n\
    </form>\n\
    ";

    DEBUG_LOG("Rendering form with script name: " << narrow(full_script_name));
    wprintf(form_add_link.c_str(), full_script_name.c_str(), query, key, key);

    PrintTail();
    DEBUG_LOG("Add Link dialog rendered successfully");
    DEBUG_LOG_FUNC_EXIT();
}
}  // namespace Bookmarks
