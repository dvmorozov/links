#pragma once

#include <string>

namespace Bookmarks
{
    //  Read configuration parameters from Windows registry.
    //  https://action.mindjet.com/task/14777741
    class TestRegConfig
    {
    public:
        static std::wstring GetValue(std::wstring name);
    };
}
