#ifndef REGCONFIG_H
#define REGCONFIG_H

#include "Shared/Models/Config.h"

#include "../../App/main.h"

namespace Bookmarks
{
    //  Read configuration parameters from Windows registry.
    //  https://action.mindjet.com/task/14777741
    class RegConfig : public Config
    {
    private:
        //  https://action.mindjet.com/task/14777741
        LONG GetStringRegKey(HKEY hKey, const std::wstring& subKey,
                             const std::wstring &strValueName, std::wstring &strValue,
                             const std::wstring &strDefaultValue);

    public:
        virtual std::wstring GetValue(std::wstring name);
    };
}

#endif // REGCONFIG_H
