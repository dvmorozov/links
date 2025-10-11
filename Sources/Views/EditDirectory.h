
#pragma once
#include "App/Globals.h"
#include "Page.h"

namespace Bookmarks
{
class EditDirectory : public Page
{
public:
    EditDirectory(const std::wstring &directoryName);

    virtual void Render();

private:
    std::wstring _directoryName;
};
}  // namespace Bookmarks
