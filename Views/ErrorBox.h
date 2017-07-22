#pragma once
#include "Page.h"

namespace Bookmarks
{
    class ErrorBox : public Page
    {
    public:
        ErrorBox();
        ~ErrorBox();

        std::wstring msg1;
        std::wstring msg2;

        ErrorBox(std::wstring _msg1, std::wstring _msg2) : msg1(_msg1), msg2(_msg2) {}

        virtual void Render();
    };
}
