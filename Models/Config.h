#pragma once

#include <string>

namespace Bookmarks
{
    //  Abstract base class for reading configuration.
    class Config
    {
    public:
        virtual std::wstring GetValue(std::wstring name) = 0;
    };
}
