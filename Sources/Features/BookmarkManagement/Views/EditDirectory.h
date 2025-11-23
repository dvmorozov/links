
#pragma once
#include "App/Globals.h"
#include "Shared/UI/Page.h"

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
