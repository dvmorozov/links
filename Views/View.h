
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

        //  Рендерит данные в стандартный поток вывода.
        virtual void Render() = 0;
    };
}
