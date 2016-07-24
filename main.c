//-------------------------------------------------------------------------------------------------
//  чтение названия из файла сделано потому, что в LINUX утилиты 
//  могут не возвращать правильно локализованные названия файлов;
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
    _tprintf(_T("%s"), _T("Content-type: text/html\n\n")); //  тип контента должен передаваться до вывода страницы
    _tprintf(_T("%s"), _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n\n"));
    //  обработка запроса
    DocumentRoot = _tgetenv(_T("DOCUMENT_ROOT"));
    if (!DocumentRoot.size()) {
        no_environment(_T("DOCUMENT_ROOT"));
        return 0;
    }

    //  изменение текущего каталога
    query = _tgetenv(_T("QUERY_STRING"));
    if(query)
    {
        int script_name_len = _tcslen(http);     //  полная длина пути к скрипту
        TCHAR *script_name = _tgetenv(_T("SCRIPT_NAME"));
        //	Вместо переменной окружения использует имя домена из конфиг. файла для того, 
        //	чтобы правильно уст. ссылки, когда сервер работает через SSH.
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
                //  готовится имя скрипта для создания ссылок
                _tcscpy(full_script_name, http);
                _tcscat(full_script_name, server_name);
                _tcscat(full_script_name, script_name);

                img_path = (TCHAR *)malloc((_tcslen(server_name) + _tcslen(http) + _tcslen(www_sub) + 1) * sizeof(TCHAR));
                //  готовится путь к скрипту для создания ссылок на картинки
                if(img_path)
                {
                    _tcscpy(img_path, http);
                    _tcscat(img_path, server_name);
                    _tcscat(img_path, www_sub);
                }
                else
                {
                    error = E_OUTOFMEMORY;
                    //  ошибка не фатальна
                }
            }
            else
                out_of_memory();
        }// if(!fatal_error)

        //  здесь формируется тело страницы
        if(!fatal_error)
        {
            cwd = (TCHAR*)_tgetcwd(0, 0);
            if(cwd)
            {
                //  запрет вставки 0 необходим для того, чтобы 
                //  process_query мог обработать весь запрос;
                //  запрос по нажатию кнопки можно отличать от
                //  перехода по ссылке задавая как метод post
                //  (сейчас везде используется get), либо по коду
                //  команды (с нажатием кнопки связаны определенные
                //  команды); однако при этом требуется поиск команды
                //  на непреобразованной строке запроса;
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
