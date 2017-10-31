
#include "Config.h"

#include <assert.h>
#include <Windows.h>

#include "../main.h"

namespace Bookmarks
{
    //  https://action.mindjet.com/task/14777741
    LONG GetStringRegKey(HKEY hKey, const std::wstring& subKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
    {
        ULONG nError;
#ifndef LINUX
        HKEY hKeyOut;
        nError = RegCreateKeyEx(hKey, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKeyOut, NULL);
        if (ERROR_SUCCESS != nError)
            return nError;

        strValue = strDefaultValue;
        WCHAR szBuffer[512];
        DWORD dwBufferSize = sizeof(szBuffer);
        nError = RegQueryValueExW(hKeyOut, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
        if (ERROR_SUCCESS == nError)
            strValue = szBuffer;
#endif
        return nError;
    }

    //  https://action.mindjet.com/task/14777741
    std::wstring RegConfig::GetValue(std::wstring name)
    {
        std::wstring result;
#ifndef LINUX
        std::wstring defaultValue = L"";
        std::wstring subKey = L"SOFTWARE\\Links";
        if (ERROR_SUCCESS != GetStringRegKey(HKEY_LOCAL_MACHINE, subKey, name, result, defaultValue))
            throw std::exception("Configuration value not found.");
#endif
        return result;
    }
}
