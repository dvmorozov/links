#pragma once

#include "../Data/Data.h"

namespace Bookmarks
{
    class View
    {
    private:
        Data D;

    public:
        View() {}
        ~View() {}

        //  Рендерит данные в стандартный поток вывода.
        virtual void Render() = 0;
    };
}
