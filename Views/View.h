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

        //  �������� ������ � ����������� ����� ������.
        virtual void Render() = 0;
    };
}
