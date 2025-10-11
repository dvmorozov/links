#include "RegConfig.h"
#include "../../App/main.h"

#include <assert.h>
#include <Windows.h>

namespace Bookmarks
{
    //  https://action.mindjet.com/task/14777741
    LONG RegConfig::GetStringRegKey(HKEY hKey, const std::wstring& subKey,
                                    const std::wstring &strValueName, std::wstring &strValue,
                                    const std::wstring &strDefaultValue)
    {
        ULONG nError;
        HKEY hKeyOut;
        nError = RegCreateKeyExW(hKey, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WOW64_64KEY, NULL, &hKeyOut, NULL);
        if (ERROR_SUCCESS != nError)
            return nError;

        strValue = strDefaultValue;
        WCHAR szBuffer[512];
        DWORD dwBufferSize = sizeof(szBuffer);
        nError = RegQueryValueExW(hKeyOut, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
        if (ERROR_SUCCESS == nError)
            strValue = szBuffer;
        return nError;
    }

    //  https://action.mindjet.com/task/14777741
    std::wstring RegConfig::GetValue(std::wstring name)
    {
        std::wstring result;
        std::wstring defaultValue = L"";
        std::wstring subKey = L"SOFTWARE\\Links";
        if (ERROR_SUCCESS != GetStringRegKey(HKEY_LOCAL_MACHINE, subKey, name, result, defaultValue))
            throw std::exception("Configuration value not found.");
        return result;
    }
}
