
#include "ErrorBox.h"

namespace Bookmarks
{
    ErrorBox::ErrorBox()
    {
    }


    ErrorBox::~ErrorBox()
    {
    }


    void ErrorBox::Render()
    {
        PrintHead(_T("Ошибка"));
        _tprintf(_T("%s"), _T("<br><font color=\"red\">"));
        _tprintf(_T("%s%s\n"), msg1.c_str(), msg2.c_str());
        _tprintf(_T("%s"), _T("</font>\n"));
        PrintTail();
    }
}