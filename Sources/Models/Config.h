
#pragma once
#include "App/Globals.h"

namespace Bookmarks
{
//  Base class for reading configuration. Must not be abstrct.
class Config
{
public:
    Config()          = default;
    virtual ~Config() = default;

    virtual std::wstring GetValue(const std::wstring &name) const { return L""; }
};
}  // namespace Bookmarks
