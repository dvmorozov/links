#pragma once
#include "Page.h"

namespace Bookmarks
{
    class PageBootstrap : public Page
    {
    protected:
        virtual void InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt);
        virtual void PrintHead(std::wstring title);
        virtual void PrintTail();

        virtual void OpenOuterTable();
        virtual void OpenInnerTable();
        virtual void CloseInnerTable();

    public:
        PageBootstrap();
        ~PageBootstrap();
    };
}
