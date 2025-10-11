
#pragma once
#include "App/Globals.h"
#include "Page.h"

namespace Bookmarks
{
class ErrorBox : public Page
{
public:
    ErrorBox(std::wstring _msg1, std::wstring _msg2);

    virtual void Render();

private:
    ErrorBox();

private:
    std::wstring msg1;
    std::wstring msg2;
};
}  // namespace Bookmarks
