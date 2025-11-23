
#include "App/stdafx.h"

#include "../Controllers/Commands.h"
#include "ErrorBox.h"

namespace Bookmarks
{
ErrorBox::ErrorBox() { _cssClass = L"dialog"; }

ErrorBox::ErrorBox(std::wstring _msg1, std::wstring _msg2, bool showHomeButton) : msg1(_msg1), msg2(_msg2), _showHomeButton(showHomeButton)
{
    _cssClass = L"dialog";
}

void ErrorBox::Render()
{
    PrintHead(L"Error");

    const std::wstring error_box_start = L"\
    <div class=\"input_box\">\n\
        <div class=\"label\">%ls</div>\n\
        <div class=\"label\">%ls</div>\n";

    const std::wstring error_box_finish = L"\
    </div>\n";

    const std::wstring buttons_box = L"\
    <div class=\"buttons\">\n\
        <button onclick=openUrl('%ls')>Home</button>\n\
    </div>\n";

    wprintf(error_box_start.c_str(), msg1.c_str(), msg2.c_str());

    PrintInfo();

    wprintf(error_box_finish.c_str());
    
    // Only show Home button if requested
    if (_showHomeButton) {
        wprintf(buttons_box.c_str(), GetFullCommandUrl(cmd_ch_folder, L"", ok).c_str());
    }

    PrintTail();
}
}  // namespace Bookmarks
