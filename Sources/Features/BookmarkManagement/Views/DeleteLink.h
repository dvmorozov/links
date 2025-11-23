
#pragma once
#include "App/Globals.h"
#include "Shared/UI/Page.h"

namespace Bookmarks
{
class DeleteLink : public Page
{
public:
    DeleteLink(const std::wstring &name, const std::wstring &url,
               const std::wstring &link_file_name);

    virtual void Render();

private:
    std::wstring _name;
    std::wstring _url;
    std::wstring _linkFileName;
};
}  // namespace Bookmarks
