
#pragma once
#include "App/Globals.h"
#include "Shared/UI/Page.h"

namespace Bookmarks
{
class EditLink : public Page
{
public:
    EditLink(const std::wstring &name, const std::wstring &url, const std::wstring &linkFileName);
    ~EditLink();

    virtual void Render();

private:
    std::wstring _name;
    std::wstring _url;
    std::wstring _linkFileName;
};
}  // namespace Bookmarks
