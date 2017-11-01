
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
        PrintHead(_T("ќшибка"));
        wprintf(_T("%s"), _T("<br><font color=\"red\">"));
        wprintf(_T("%s%s\n"), msg1.c_str(), msg2.c_str());
        wprintf(_T("%s"), _T("</font>\n"));
        PrintTail();
    }
}
