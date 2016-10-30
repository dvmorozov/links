
#include "Config.h"

#include <assert.h>
#include <Windows.h>

namespace Bookmarks
{
    //  https://action.mindjet.com/task/14777741
    LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
    {
        strValue = strDefaultValue;
        WCHAR szBuffer[512];
        DWORD dwBufferSize = sizeof(szBuffer);
        ULONG nError;
        nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
        if (ERROR_SUCCESS == nError)
        {
            strValue = szBuffer;
        }
        return nError;
    }

    //  https://action.mindjet.com/task/14777741
    std::wstring RegConfig::GetValue(std::wstring name)
    {
        std::wstring result;
        if (ERROR_SUCCESS != GetStringRegKey(HKEY_LOCAL_MACHINE, std::wstring(L"\\SOFTWARE\\Links") + name, result, L""))
            throw std::exception("Configuration value not found.");
        return result;
    }
}
