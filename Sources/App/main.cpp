//-------------------------------------------------------------------------------------------------
//  Reading the file name from the file is done because in LINUX utilities
//  may not return properly localized file names;
//  the parameter string is read from the console at the beginning;
//  all command parameters must be passed with the command name
//-------------------------------------------------------------------------------------------------

#include "main.h"
#include "Globals.h"
#include "stdafx.h"

#include "Shared/Controllers/Commands.h"
#include "Utils/DebugLog.h"
#include "Utils/Errors.h"
#include "Utils/Miscellaneous.h"
#include "Shared/UI/ErrorBox.h"

int main(int argc, char *argv[])
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Application starting with " << argc << " arguments");
    
    try {
#ifdef UNICODE
#ifdef _WINDOWS
        //  This is Windows specific function.
        //  TODO: check if it is really necessary.
        _setmode(_fileno(stdout), _O_U8TEXT);
        DEBUG_LOG("Set Windows Unicode output mode");
#endif
#endif
        //  Controls output of the application.
        //  It seems than text is converted internally to the selected code page.
#ifdef _WINDOWS
        //  IIS works only with UTF-8.
        setlocale(LC_ALL, "ru_RU.UTF-8");
        DEBUG_LOG("Set Windows locale to ru_RU.UTF-8");
#else
        //  Use the "locale" command to see actual console locale at the system.
        //  Apache displays cyrillic properly only with this settings.
        setlocale(LC_ALL, "C.UTF-8");
        DEBUG_LOG("Set Linux locale to C.UTF-8");
#endif
        //  The content type must be passed before the page is output.
        wprintf(_T("Content-type: text/html\n\n"));
        wprintf(_T("<!DOCTYPE html>\n\n"));
        DEBUG_LOG("Sent HTTP headers");

        //  Request processing.
        std::wstring query_string = get_wide_environment_variable(_T("QUERY_STRING"));
        DEBUG_LOG("Query string: " << narrow(query_string));
        if (!query_string.size()) {
            DEBUG_LOG("ERROR: Empty query string");
            no_environment(_T("QUERY_STRING"));
        }

        DEBUG_LOG("Starting query processing");
        int result = handle_query(query_string.c_str());
        DEBUG_LOG("Query processing completed with result: " << result);
        DEBUG_LOG_FUNC_EXIT();
        return result;
    } catch (auth_handled_exception &) {
        // Authentication error has already been handled and displayed
        // Just exit cleanly without showing additional error pages
        return 0;
    } catch (two_msg_exception &e) {
        try {
            Bookmarks::ErrorBox eb(e.msg1, e.msg2);
            eb.Render();
        } catch (...) {
            wprintf(L"Fatal error in constructing object.");
        }
    } catch (std::exception &e) {
        try {
            const std::wstring message = widen(e.what()
#ifdef _WINDOWS
                                               //  System exceptions are in the system locale.
                                               ,
                                               CP_ACP
#endif
            );
            Bookmarks::ErrorBox eb(err_system_exception, message);
            eb.Render();
        } catch (...) {
            wprintf(L"Fatal error in constructing object.");
        }
    }

    //  The application must always return "success".
    return 0;
}
//-------------------------------------------------------------------------------------------------
