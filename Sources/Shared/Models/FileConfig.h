
#pragma once
#include "App/Globals.h"
#include "Config.h"

namespace Bookmarks
{
class FileConfig : public Config
{
public:
    virtual std::wstring GetValue(const std::wstring &name) const;
};
}  // namespace Bookmarks
