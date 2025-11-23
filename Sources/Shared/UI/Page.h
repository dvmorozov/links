
#pragma once
#include "App/Globals.h"
#include "View.h"

namespace Bookmarks
{
class Page : public View
{
public:
    Page();

    virtual void PrintScripts();
    virtual void PrintStyles();
    virtual void PrintHead(std::wstring title);
    virtual void PrintTail();

protected:
    std::wstring GetCommandUrl(TCHAR *cmd, const TCHAR *dir, const TCHAR *file);
    std::wstring GetFullCommandUrl(TCHAR *cmd, const TCHAR *dir, const TCHAR *file);

    static void PrintInfo();

protected:
    std::wstring _cssClass;
};
}  // namespace Bookmarks
