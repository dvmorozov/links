
#include "App/stdafx.h"

#include "Errors.h"
#include "Utils/DebugLog.h"
#include "Shared/UI/ErrorBox.h"

void invalid_query_fl(const char *file, int line)
{
    error       = E_INVALID_QUERY;
    fatal_error = 1;

    //  TODO: use formatting function for this.
    const std::wstring msg =
        std::wstring(L"[") + query + L"] at " + widen(file) + L", " + widen(std::to_string(line));

    throw two_msg_exception(err_invalid_query, msg);
}
//-------------------------------------------------------------------------------------------------

void print_exception(const char *what)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Printing exception with what: " << (what ? what : "(null)"));
    
    if (what) {
        const wchar_t      *what_wide = create_new_wide_string(what);
        DEBUG_LOG("Exception details: " << what);
        Bookmarks::ErrorBox eb(err_system_exception, std::wstring(query) + _T("<br>") +
                                                         std::wstring(err_details) +
                                                         std::wstring(what_wide));
        delete[] what_wide;

        eb.Render();
    }
    error       = E_INVALID_QUERY;
    fatal_error = 1;
    DEBUG_LOG("Exception handling complete, fatal_error set to 1");
    DEBUG_LOG_FUNC_EXIT();
}
//-------------------------------------------------------------------------------------------------

void out_of_memory()
{
    error       = E_OUTOFMEMORY;
    fatal_error = 1;
    throw two_msg_exception(err_out_of_memory, _T(""));
}
//-------------------------------------------------------------------------------------------------

void file_doesnt_exist(const std::wstring &file_path)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("File doesn't exist error for: " << narrow(file_path));
    
    error       = E_FILE_DOESNT_EXIST;
    fatal_error = 1;

    DEBUG_LOG("Throwing two_msg_exception for file not found");
    DEBUG_LOG_FUNC_EXIT();
    throw two_msg_exception(err_file_doesnt_exist, file_path);
}
//-------------------------------------------------------------------------------------------------

void no_permission_fl(const std::wstring &file_path, const char *file, int line)
{
    error       = E_NO_PERMISSION;
    fatal_error = 1;

    //  TODO: use formatting function for this.
    const std::wstring msg = std::wstring(L"[") + file_path + L"] at " + widen(std::string(file)) +
                             L", " + widen(std::to_string(line));

    throw two_msg_exception(err_no_permission, msg);
}
//-------------------------------------------------------------------------------------------------

void no_environment(const TCHAR *env_str)
{
    error       = E_NO_ENVIRONMENT;
    fatal_error = 1;
    throw two_msg_exception(err_no_environment, env_str);
}
//-------------------------------------------------------------------------------------------------

void system_error(const std::wstring &what)
{
    error       = E_SYS_ERROR;
    fatal_error = 1;
    throw two_msg_exception(err_system_exception, std::wstring(L"[") + what + L"]");
}
//-------------------------------------------------------------------------------------------------

void sysutility_error(const std::wstring &what, const std::wstring &command)
{
    error       = E_SYS_UTILITY;
    fatal_error = 1;
    throw two_msg_exception(what, command);
}
//-------------------------------------------------------------------------------------------------
