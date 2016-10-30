#pragma once

#include <string>

namespace Bookmarks
{
    //  Read configuration parameters from Windows registry.
    //  https://action.mindjet.com/task/14777741
    class RegConfig
    {
    public:
        static std::wstring GetValue(std::wstring name);
    };
}
