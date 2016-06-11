#pragma once
#include "Page.h"

namespace Bookmarks
{
    class PageBootstrap : public Page
    {
    protected:
        virtual void InsertButton(std::string image_file, const std::string script_name, const std::string url, int size, const std::string alt);
        virtual void PrintHead(std::string title);
        virtual void PrintTail();

        virtual void OpenOuterTable();
        virtual void OpenInnerTable();
        virtual void CloseInnerTable();

    public:
        PageBootstrap();
        ~PageBootstrap();
    };
}
