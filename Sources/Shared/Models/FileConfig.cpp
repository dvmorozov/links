
#include "App/stdafx.h"

#include "FileConfig.h"
#include "Utils/DebugLog.h"
#include "Utils/Errors.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
std::wstring FileConfig::GetValue(const std::wstring &name) const
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Getting config value for: " << narrow(name));
    
    //  TODO: read this from file.
    try {
        static const std::map<std::wstring, std::wstring> configuration = {
            //  TODO: username must be taken from the command.
            {L"UserName", L"test"},
            {L"Password", L"test"},
        //  Paths must end with the delimiter.
        //  For now, the path must be terminated by path delimiter.
        //  TODO: remove the condition that the path must be terminated by path delimiter.
#ifdef _TEST
            //  This is for using in tests.
            //  Query mustn't contain full script name
            //  but every parameter must start from ';' (%3B).
            {L"LoginQuery", L"%3Blog_in=test&%3Blog_in=test&%3Blog_in=Ok"},
            //  These parameters substitute missing environment variables.
            {L"DOCUMENT_ROOT",
             L"C:\\Users\\dmitry\\Projects\\library-everything\\links\\TestData\\"},
            {L"QUERY_STRING",
             L"http://localhost/cgi-bin/links-legacy.exe/?;log_in=test&;log_in=test&;log_in=Ok"},
            {L"SCRIPT_NAME", L"/cgi-bin/links-legacy.exe/"},
            {L"SERVER_NAME", L"localhost"},
            {L"TestFolder", L"C:\\Users\\dmitry\\Projects\\library-everything\\links\\TestData\\"},
            {L"SERVER_SOFTWARE", L"Microsoft-IIS/7.5"},
            {L"REQUEST_METHOD", L"GET"},
#else
#ifdef _WINDOWS
            //{ L"DOCUMENT_ROOT", L"X:\\Library\\11_links\\" },
            {L"DOCUMENT_ROOT", L"C:\\inetpub\\"}
#else
            {L"DOCUMENT_ROOT", L"/home/dmitry/Applications/links/"}
#endif
#endif
        };

        std::wstring result = configuration.at(name);
        DEBUG_LOG("Config value found: " << narrow(result));
        DEBUG_LOG_FUNC_EXIT();
        return result;
    } catch (std::exception &e) {
        DEBUG_LOG("Configuration parameter not found: " << narrow(name));
        system_error(L"Configuration parameter not found: " + name);
        DEBUG_LOG_FUNC_EXIT();
        throw e;
    }
}
}  // namespace Bookmarks
