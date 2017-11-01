
#pragma once
#include "../Models/Data.h"

namespace Bookmarks
{
    class View
    {
    private:
        Data _d;

    public:
        View() {}
        ~View() {}

        //  –ендерит данные в стандартный поток вывода.
        virtual void Render() = 0;
    };
}
