//-------------------------------------------------------------------------------------------------
//  ������ �������� �� ����� ������� ������, ��� � LINUX ������� 
//  ����� �� ���������� ��������� �������������� �������� ������;
//  stroka parametrov chitaetsya on konza k nachalu;
//  vse parametry komandy dolzhny peredavat'sya s imenem komandy
//-------------------------------------------------------------------------------------------------

#include "main.h"
#include "Controllers/Commands.h"

#define MAX_DOMAIN 100

#ifdef UNICODE
int wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ] )
#else
int main(int argc, TCHAR *argv[])
#endif  //  UNICODE
{
    setlocale(LC_ALL, ".1251");
    _tprintf(_T("%s"), _T("Content-type: text/html\n\n")); //  ��� �������� ������ ������������ �� ������ ��������
    _tprintf(_T("%s"), _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n\n"));
    //  ��������� �������
    document_root = _tgetenv(_T("DOCUMENT_ROOT"));
    if (!document_root.size()) {
        no_environment(_T("DOCUMENT_ROOT"));
        return 0;
    }

    //  ��������� �������� ��������
    query = _tgetenv(_T("QUERY_STRING"));
    if(query)
    {
        int script_name_len = _tcslen(http);     //  ������ ����� ���� � �������
        TCHAR *script_name = _tgetenv(_T("SCRIPT_NAME"));
        //	������ ���������� ��������� ���������� ��� ������ �� ������. ����� ��� ����, 
        //	����� ��������� ���. ������, ����� ������ �������� ����� SSH.
        //	evernote:///view/14501366/s132/44d4835c-cdac-40e1-acff-2fe610f865c8/44d4835c-cdac-40e1-acff-2fe610f865c8/
#ifdef _WINDOWS
        TCHAR server_name[MAX_DOMAIN];
        memset(server_name, 0, sizeof(server_name));

        GetPrivateProfileString(
          _T("Links"),
          _T("Domain"),
          _T("fiteasily.com"),
          server_name,
          MAX_DOMAIN,
          _T("links.ini")
        );
#else
        TCHAR *server_name = _tgetenv(_T("SERVER_NAME"));
#endif

        if (!server_name) {
            no_environment(_T("SERVER_NAME")); return 0;
        }
        if (!script_name) {
            no_environment(_T("SCRIPT_NAME")); return 0;
        }
        if(!fatal_error)
        {
            full_script_name = 0;
            script_name_len += _tcslen(server_name);
            script_name_len += _tcslen(script_name);

            full_script_name = (TCHAR *)malloc((script_name_len + 1) * sizeof(TCHAR));
            if(full_script_name)
            {
                //  ��������� ��� ������� ��� �������� ������
                _tcscpy(full_script_name, http);
                _tcscat(full_script_name, server_name);
                _tcscat(full_script_name, script_name);

                img_path = (TCHAR *)malloc((_tcslen(server_name) + _tcslen(http) + _tcslen(www_sub) + 1) * sizeof(TCHAR));
                //  ��������� ���� � ������� ��� �������� ������ �� ��������
                if(img_path)
                {
                    _tcscpy(img_path, http);
                    _tcscat(img_path, server_name);
                    _tcscat(img_path, www_sub);
                }
                else
                {
                    error = E_OUTOFMEMORY;
                    //  ������ �� ��������
                }
            }
            else
                out_of_memory();
        }// if(!fatal_error)

        //  ����� ����������� ���� ��������
        if(!fatal_error)
        {
            cwd = (TCHAR*)_tgetcwd(0, 0);
            if(cwd)
            {
                //  ������ ������� 0 ��������� ��� ����, ����� 
                //  process_query ��� ���������� ���� ������;
                //  ������ �� ������� ������ ����� �������� ��
                //  �������� �� ������ ������� ��� ����� post
                //  (������ ����� ������������ get), ���� �� ����
                //  ������� (� �������� ������ ������� ������������
                //  �������); ������ ��� ���� ��������� ����� �������
                //  �� ����������������� ������ �������;
                //  !!! esli zapros poluchen v rezul'tate perehoda po ssylke,
                //  to nuzhno vyzyvat' process_query(0) !!!
                switch(get_query_command(1))
                {
                default:
                case(CMD_LOG_IN_CONF): process_query(0); 
                    if(!fatal_error)do_log_in_conf(); 
                    break;

                case(CMD_LOG_IN): process_query(1); 
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        do_log_in(); 
                    }
                    break;

                case(CMD_CHANGE_FOLDER): process_query(0); 
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_change_folder(); 
                    }
                    break;

                case(CMD_EDIT): process_query(1); 
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_edit(); 
                    }
                    break;

                case(CMD_EDIT_CONF): process_query(0); 
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_edit_conf(); 
                    }
                    break;

                case(CMD_ADD): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_add(); 
                    }
                    break;

                case(CMD_ADD_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_add_conf(); 
                    }
                    break;

                case(CMD_DEL): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_del(); 
                    }
                    break;

                case(CMD_DEL_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_del_conf(); 
                    }
                    break;

                case(CMD_EDIT_FOLDER): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_edit_folder(); 
                    }
                    break;

                case(CMD_EDIT_FOLDER_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_edit_folder_conf(); 
                    }
                    break;

                case(CMD_ADD_FOLDER): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_add_folder(); 
                    }
                    break;

                case(CMD_ADD_FOLDER_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_add_folder_conf(); 
                    }
                    break;

                case(CMD_DEL_FOLDER): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_del_folder(); 
                    }
                    break;

                case(CMD_DEL_FOLDER_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  udalenie komandy iz zaprosa
                        get_key();
                        do_del_folder_conf(); 
                    }
                    break;
                }// switch(get_query_command)
            }// if(cwd)
            else
                out_of_memory();
        }
        if(query)free(query);   //  posle process_query mozhno osvobozhdat'
        if(img_path)free(img_path);
        if(cwd)free(cwd);
        if(full_script_name)free(full_script_name);
    }// if(query)
    else 
        no_environment(_T("QUERY_STRING"));

    return 0;
}
//-------------------------------------------------------------------------------------------------
