//-------------------------------------------------------------------------------------------------
//  ������ �������� �� ����� ������� ������, ��� � LINUX ������� 
//  ����� �� ���������� ��������� �������������� �������� ������;
//  stroka parametrov chitaetsya on konza k nachalu;
//  vse parametry komandy dolzhny peredavat'sya s imenem komandy
//-------------------------------------------------------------------------------------------------

#include "main.h"
#include "Controllers/Commands.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#ifdef UNICODE
int wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ] )
#else
int main(int argc, TCHAR *argv[])
#endif  //  UNICODE
{
#ifdef UNICODE
    _setmode(_fileno(stdout), _O_U8TEXT);
#endif
    setlocale(LC_ALL, ".1251");
    _tprintf(_T("%s"), _T("Content-type: text/html\n\n")); //  ��� �������� ������ ������������ �� ������ ��������
    _tprintf(_T("%s"), _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n\n"));
    //  ��������� �������
    DocumentRoot = _wgetenv(_T("DOCUMENT_ROOT"));
    if (!DocumentRoot.size()) {
        no_environment(_T("DOCUMENT_ROOT"));
        return 0;
    }

    return HandleQuery(_wgetenv(_T("QUERY_STRING"))
#ifdef USE_SCRIPT_NAME
        , _wgetenv(_T("SCRIPT_NAME"))
#endif
    );
}
//-------------------------------------------------------------------------------------------------
