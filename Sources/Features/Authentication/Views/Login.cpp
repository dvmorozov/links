
#include "App/stdafx.h"

#include "Login.h"
#include "Shared/Controllers/Commands.h"

namespace Bookmarks
{
Login::Login() { _cssClass = L"dialog"; }

Login::Login(const std::wstring& errorMessage) : _errorMessage(errorMessage) { _cssClass = L"dialog"; }

void Login::Render()
{
    PrintHead(_T("Login"));

    // Display error message if present
    if (!_errorMessage.empty()) {
        const std::wstring error_display = L"\
    <div class=\"input_box\" style=\"margin-bottom: 10px;\">\n\
        <div class=\"label\" style=\"color: red; font-weight: bold;\">%ls</div>\n\
    </div>\n";
        wprintf(error_display.c_str(), _errorMessage.c_str());
    }

    //  TODO: take labels from resource file.
    const std::wstring form_login = L"\
    <form method=get action=\"%ls\">\n\
        <div class=\"input_box\">\n\
            <div class=\"label\">Username</div>\n\
            <input type=\"text\" size=50 value=\"\" name=\";log_in\"><br>\n\
            <div class=\"label\">Password</div>\n\
            <input type=\"text\" size=50 value=\"\" name=\";log_in\"><br>\n\
        </div>\n\
        <div class=\"buttons\">\n\
            <input type=\"submit\" value=\"Ok\" name=\";log_in\">\n\
        </div>\n\
    </form>\n\
    ";

    wprintf(form_login.c_str(), full_script_name.c_str());

    PrintTail();
}
}  // namespace Bookmarks
