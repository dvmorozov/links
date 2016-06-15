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

        //  �������� ������ � ����������� ����� ������.
        virtual void Render() = 0;
    };
}
