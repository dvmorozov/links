
#pragma once
#include "Page.h"

namespace Bookmarks
{
    class AddLink : public Page
    {
    public:
        AddLink();
        ~AddLink();

        virtual void Render();
    };
}
