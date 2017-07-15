
#include "Page.h"
#include "../Controllers/Commands.h"
#include "../main.h"

namespace Bookmarks
{
    Page::Page()
    {
    }

    Page::~Page()
    {
    }

    //  https://action.mindjet.com/task/14817423
    void Page::PrintScripts()
    {
        _tprintf(_T("<script src=\"https://code.jquery.com/jquery-2.1.3.js\" type=\"text/javascript\"></script>\n"));
        //  Attach on submit listener.
        /*
        _tprintf(_T("<script type=\"text/javascript\">\n\
            $(function() {\n\
                 $('form').submit(function(ev) {\n\
                    $('form input:text').each(function() {\n\
                         $(this).val(encodeURIComponent($(this).val()));\n\
                    });\n\
                    this.submit();\n\
                });\n\
            });\n\
            </script>\n"));
            */
    }

    //  https://action.mindjet.com/task/14817423
    void Page::PrintStyles()
    {
    }

    void Page::PrintHead(std::wstring title)
    {
        _tprintf(_T("%s"), _T("<html>"));
        _tprintf(_T("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><meta http-equiv=\"Content-Language\" content=\"ru\">\n"));

        PrintStyles();
        PrintScripts();

        _tprintf(_T("<title>%s</title></head>\n"), title.c_str());
        _tprintf(_T("<body>\n"));
    }

    void Page::PrintTail()
    {
        PrintInfo();
        _tprintf(_T("%s"), _T("</body></html>\n"));
    }

    void Page::PrintInfo()
    {
#if _DEBUG
        std::vector<std::wstring> params = {
            _T("SERVER_SOFTWARE"), _T("SERVER_NAME"), _T("SCRIPT_NAME"),
            _T("REQUEST_METHOD"), _T("QUERY_STRING"), _T("DOCUMENT_ROOT")
        };

        _tprintf(_T("%s"), _T("<br><hr><br>\n"));
        for (auto par = params.begin(); par != params.end(); ++par)
            _tprintf(_T("%s = %s<br>\n"), par->c_str(), _wgetenv(par->c_str()));

        _tprintf(_T("HOME = %s<br>\n"), cwd);
        _tprintf((DocumentRoot.substr(0, DocumentRoot.rfind('/')) + _T("<br>\n")).c_str());
        _tprintf(_T("Error = %i<br>\n"), error);
#endif
    }
}