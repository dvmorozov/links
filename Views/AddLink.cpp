
#include "AddLink.h"
#include "../Controllers/Commands.h"
#include "../main.h"

namespace Bookmarks
{
    AddLink::AddLink()
    {
    }


    AddLink::~AddLink()
    {
    }

    void AddLink::Render()
    {
        PrintHead(_T("—оздание —сылки"));

        TCHAR htm_add[] = 
            _T("<form method=get action=\"%s\">\n\
                <p>\n\
                    »м¤ ссылки:<br>\n\
                    <input type=\"text\" size=50 value=\"\" name=\"%s;add\"><br>\n\
                    —сылка указывает на:<br>\n\
                    <input type=\"text\" size=50 value=\"\" name=\";add\"><br>\n\
                </p>\n\
                <p>\n\
                    <input type=\"submit\" value=\"Cancel\" name=\";key=%s;add\">\n\
                    <input type=\"submit\" value=\"Ok\" name=\";key=%s;add\">\n\
                </p>\n\
            </form>");

        wprintf(htm_add, full_script_name, query, key, key);

        PrintTail();
    }
}
