
#include "App/stdafx.h"

#include "Page.h"
#include "Utils/DebugLog.h"
#include "../Controllers/Commands.h"
#include "Utils/Errors.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
Page::Page() 
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Page constructor called");
    DEBUG_LOG_FUNC_EXIT();
}

std::wstring Page::GetCommandUrl(TCHAR *cmd, const TCHAR *dir, const TCHAR *file)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Building command URL");
    if (cmd) DEBUG_LOG("Command: " << narrow(std::wstring(cmd)));
    if (dir) DEBUG_LOG("Directory: " << narrow(std::wstring(dir)));
    if (file) DEBUG_LOG("File: " << narrow(std::wstring(file)));
    
    std::wstring command;
    if (dir)
        command += dir;
    command += cmd_key;
    if (key)
        command += key;
    if (cmd)
        command += cmd;
    if (file)
        command += file;
        
    DEBUG_LOG("Generated command URL: " << narrow(command));
    DEBUG_LOG_FUNC_EXIT();
    return command;
}

std::wstring Page::GetFullCommandUrl(TCHAR *cmd, const TCHAR *dir, const TCHAR *file)
{
    return full_script_name + L"?" + GetCommandUrl(cmd, dir, file);
}

void Page::PrintScripts()
{
#ifdef _WINDOWS
    //  This corresponds to current IIS settings.
    wprintf(_T("<script type=\"text/javascript\" src=\"Common.js\"></script>\n"));
#else
    wprintf(_T("<script type=\"text/javascript\" src=\"%ls/links/Common.js\"></script>\n"),
            server_root.c_str());
#endif    
    wprintf(_T("<script src=\"https://code.jquery.com/jquery-2.1.3.js\" ")
            _T("type=\"text/javascript\"></script>\n"));
}

void Page::PrintStyles()
{
    //  TODO: use configuration file for this.
#ifdef _WINDOWS
    //  This corresponds to current IIS settings.
    wprintf(_T("<link rel=\"stylesheet\" type=\"text/css\" href=\"Styles.css\">\n"));
#else
    //  This corresponds to Apache settings.
    wprintf(_T("<link rel=\"stylesheet\" type=\"text/css\" href=\"%ls/links/Styles.css\">\n"),
            server_root.c_str());
#endif
}

void Page::PrintHead(std::wstring title)
{
    wprintf(_T("<html>\n"));
    wprintf(
        _T("<head><meta http-equiv=\"Content-Type\" content=\"text/html\" charset=\"UTF-8\">\n"));

    PrintStyles();
    PrintScripts();

    wprintf(_T("<title>%ls</title>\n</head>\n"), title.c_str());
    wprintf(_T("<body"));

    if (!_cssClass.empty())
        wprintf(L" class=\"%ls\"", _cssClass.c_str());

    wprintf(_T(">\n"));
}

void Page::PrintTail() { wprintf(_T("</body>\n</html>\n")); }

void Page::PrintInfo()
{
#ifdef _DEBUG
    const std::vector<std::wstring> params = {_T("SERVER_SOFTWARE"), _T("SERVER_NAME"),
                                              _T("SCRIPT_NAME"),     _T("REQUEST_METHOD"),
                                              _T("QUERY_STRING"),    _T("DOCUMENT_ROOT")};

    wprintf(_T("\
        <div class=\"error-details\">\n"));
    for (const auto &par : params) {
        std::wstring environment_variable;
        try {
            environment_variable = get_wide_environment_variable(par);
        } catch (std::exception &) {
            environment_variable = err_not_assigned;
        }

        wprintf(_T("\
            <div class=\"label\">%ls [%ls]</div>\n"),
                par.c_str(), environment_variable.c_str());
    }

    wprintf(_T("\
            <div class=\"label\">Current directory [%ls]</div>\n"),
            cwd.c_str());
    wprintf(_T("\
            <div class=\"label\">Data directory [%ls]</div>\n"),
            document_root.c_str());
    wprintf(_T("\
            <div class=\"label\">Images root [%ls]</div>\n"),
            images_root.c_str());
    wprintf(_T("\
            <div class=\"label\">Server name [%ls]</div>\n"),
            server_name.c_str());
    wprintf(_T("\
            <div class=\"label\">Full script name [%ls]</div>\n"),
            full_script_name.c_str());
    wprintf(_T("\
            <div class=\"label\">Script name [%ls]</div>\n"),
            script_name.c_str());
    wprintf(_T("\
            <div class=\"label\">Error code [%i]</div>\n"),
            error);
    wprintf(_T("\
        </div>\n"));
#endif
}
}  // namespace Bookmarks
