#pragma once

#include "../Models/Data.h"

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
