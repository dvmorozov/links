
#pragma once
#include "App/Globals.h"
#include "Shared/UI/Page.h"

namespace Bookmarks
{
class Login : public Page
{
public:
    Login();
    Login(const std::wstring& errorMessage);

    virtual void Render();

private:
    std::wstring _errorMessage;
};
}  // namespace Bookmarks
