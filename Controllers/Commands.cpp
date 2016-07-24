
#include "../main.h"
#include "../Models/Data.h"
#include "../Models/FileReader.h"
#include "../Views/PageBootstrap.h"
#include "Commands.h"

//  команды для исполнения программой (вставляются в url)
//  !!! нужны наборы одинаковых команд для папок и файлов, поскольку расширение
//  не передается через url, а внутри папки может находиться папка и файл с 
//  одинаковым именем !!!
//  komandy dolzhny nachinat'sya s ; - sm. realizatsiyu insert_command_image
TCHAR cmd_edit[] = _T(";edit=");
TCHAR cmd_edit_conf[] = _T(";edit_conf=");
TCHAR cmd_add[] = _T(";add=");
TCHAR cmd_log_in[] = _T(";log_in=");
TCHAR cmd_add_conf[] = _T(";add_conf=");
TCHAR cmd_del[] = _T(";del=");
TCHAR cmd_del_conf[] = _T(";del_conf=");
TCHAR cmd_ch_folder[] = _T(";ch_folder=");
TCHAR cmd_edit_folder[] = _T(";edit_folder=");
TCHAR cmd_edit_folder_conf[] = _T(";edit_folder_conf=");
TCHAR cmd_add_folder[] = _T(";add_folder=");
TCHAR cmd_add_folder_conf[] = _T(";add_folder_conf=");
TCHAR cmd_del_folder[] = _T(";del_folder=");
TCHAR cmd_del_folder_conf[] = _T(";del_folder_conf=");
TCHAR cmd_key[] = _T(";key=");

TCHAR ok[] = _T("Ok");
TCHAR cancel[] = _T("Cancel");
TCHAR pict_error[] = _T("error.bmp");

std::wstring DocumentRoot;

//  Работа с URL-файлами.
const std::wstring Ext = _T(".url");            //  д. вкл. точку
const std::wstring ParamURL = _T("URL");

#ifdef EXTENDED_URL_FILE
const std::wstring ParamName = _T("Name");

TCHAR url_file_template[] = _T("\n\
[InternetShortcut]\n\
URL=%s\n\
Name=%s\n\
");
#else
TCHAR url_file_template[] = _T("\n\
[InternetShortcut]\n\
URL=%s\n\
");
#endif

int error = 0;
int fatal_error = 0;                                                //  priznak vyvoda soobscheniya ob oschibke

                                                                    //  шаблоны страниц
                                                                    //  слова Cancel и Ок д.б. такими же, как в строках cancel и ok
TCHAR htm_folder_del_conf[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>Удаление Папки</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>Вы действительно хотите удалить папку <font color=\"red\">%s</font> и все её содержимое?</P>\n\
            <p>\n\
                <input id=\"Reset1\" type=\"submit\" value=\"Cancel\" name=\"%s;key=%s;del_folder\">\n\
                <input id=\"Submit1\" type=\"submit\" value=\"Ok\" name=\"%s;del_folder=%s;key=%s;del_folder\">\n\
            </p>\n\
        </form>\n\
    </body>\n\
</html>\n\
");

TCHAR htm_del_conf[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>Удаление Ссылки</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>Вы действительно хотите удалить ссылку <font color=\"red\">%s</font>, указывающую на <font color=\"red\">%s</font> ?</P>\n\
            <p>\n\
                <input id=\"Reset1\" type=\"submit\" value=\"Cancel\" name=\"%s;del=%s;key=%s;del\">\n\
                <input id=\"Submit1\" type=\"submit\" value=\"Ok\" name=\"%s;del=%s;key=%s;del\">\n\
            </p>\n\
        </form>\n\
    </body>\n\
</html>\n\
");

TCHAR htm_edit[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>Редактирование Ссылки</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                Имя ссылки:<br>\n\
                <input type=\"text\" size=50 value=\"%s\" name=\"%s;edit\"><br>\n\
                Ссылка указывает на %s\n\
            </p>\n\
            <p>\n\
                <input type=\"submit\" value=\"Cancel\" name=\";key=%s;edit\">\n\
                <input type=\"submit\" value=\"Ok\" name=\";edit=%s;key=%s;edit\">\n\
            </p>\n\
        </form>\n\
    </body>\n\
</html>\n\
");

TCHAR htm_edit_folder[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>Редактирование Папки</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                Имя папки:<br>\n\
                <input type=\"text\" size=50 value=\"%s\" name=\"%s;edit_folder\"><br>\n\
            </p>\n\
            <p>\n\
                <input type=\"submit\" value=\"Cancel\" name=\";key=%s;edit_folder\">\n\
                <input type=\"submit\" value=\"Ok\" name=\";edit_folder=%s;key=%s;edit_folder\">\n\
            </p>\n\
        </form>\n\
    </body>\n\
</html>\n\
");

TCHAR htm_add[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>Создание Ссылки</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                Имя ссылки:<br>\n\
                <input type=\"text\" size=50 value=\"\" name=\"%s;add\"><br>\n\
                Ссылка указывает на:<br>\n\
                <input type=\"text\" size=50 value=\"\" name=\";add\"><br>\n\
            </p>\n\
            <p>\n\
                <input type=\"submit\" value=\"Cancel\" name=\";key=%s;add\">\n\
                <input type=\"submit\" value=\"Ok\" name=\";key=%s;add\">\n\
            </p>\n\
        </form>\n\
    </body>\n\
</html>\n\
");

TCHAR htm_add_folder[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>Создание Папки</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                Имя папки:<br>\n\
                <input type=\"text\" size=50 value=\"\" name=\"%s;add_folder\"><br>\n\
            </p>\n\
            <p>\n\
                <input type=\"submit\" value=\"Cancel\" name=\";key=%s;add_folder\">\n\
                <input type=\"submit\" value=\"Ok\" name=\";key=%s;add_folder\">\n\
            </p>\n\
        </form>\n\
    </body>\n\
</html>\n\
");

TCHAR htm_log_in_conf[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>Вход</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                Имя пользователя:<br>\n\
                <input type=\"text\" size=50 value=\"\" name=\";log_in\"><br>\n\
                Пароль:<br>\n\
                <input type=\"text\" size=50 value=\"\" name=\";log_in\"><br>\n\
            </p>\n\
            <p>\n\
                <input type=\"submit\" value=\"Ok\" name=\";log_in\">\n\
            </p>\n\
        </form>\n\
    </body>\n\
</html>\n\
");

//#define USE_CYGWIN

//  используется утилита командной строки, потому что
//  C-функция _rmdir требует, чтобы папка была пуста
#if !defined LINUX
//TCHAR rm[] = _T("rmdir /S /Q ");                      //  вызов для Windows
TCHAR rm[] = _T("rm -r -f ");                           //  вызов для Cygwin
#else
TCHAR rm[] = _T("rm -r -f -d ");                        //  вызов для Linux
                                                        //TCHAR rmdir[] = _T("rmdir ");
#endif

TCHAR http[] = _T("http://");
TCHAR www[] = _T("../users/links/");                    //  путь для перехода в папку с контентом;
                                                        //  должен содержать разделитель / на конце
TCHAR tmp[] = _T("../tmp/links/");                      //  put' dlya perehode v papku vremennyh dannyh
TCHAR www_sub[] = _T("/links/");                        //  www - папка по-умолчанию для веб-узла, поэтому
                                                        //  указывать ее явно не нужно
TCHAR head_error[] = _T("Ошибка");

TCHAR err_change_folder[] = _T("Невозможно перейти в папку: ");
TCHAR err_out_of_memory[] = _T("Недостаточно памяти для выполнения");
TCHAR err_invalid_query[] = _T("Недопустимый запрос: ");
TCHAR err_no_environment[] = _T("Отсутствует требуемое окружение: ");
TCHAR err_sys_utility[] = _T("Вызов утилиты %s завершился с ошибкой");

//  !!! проверять нужно все возможные типы запроса - запрос м.б. введен вручную !!!
TCHAR *query = 0;                                       //  не допускается равенство 0
TCHAR *full_script_name = 0;                            //  не допускается равенство 0
TCHAR *img_path = 0;                                    //  !!! включает завершающий символ / !!!
TCHAR *cwd = 0;                                         //  путь к текущему каталогу
                                                        //  завершающий символ / не включается
                                                        //  не допускается равенство 0
TCHAR *command = 0;                                     //  ук-ль на переданную в запросе команду (освобождается вместе с освобождением запроса)
TCHAR *key = 0;                                         //  ключ сессии, полученный из запроса
//  Ук-ли на параметры в строке query.
TCHAR *username = 0;
TCHAR *password = 0;
#define MAX_USER_NAME       100
//-------------------------------------------------------------------------------------------------

void PrintHtmlHead(TCHAR *title);
void PrintHtmlTail();
//-------------------------------------------------------------------------------------------------
//  поиск % и вставка соответ. символов
//  '+' заменяются на пробелы
void process_query(unsigned char delete_spaces)
{
    int query_len = _tcslen(query);
    TCHAR *temp = (TCHAR *)malloc((query_len + 1) * sizeof(TCHAR));
    if (temp)
    {
        TCHAR *ptr, *prev_ptr;
        ptr = query;
        prev_ptr = ptr;
        temp[0] = 0;

        //  когда браузер отправляет запрос в ответ на нажатие кнопки,
        //  то он заменяет пробелы на '+', а '+' на % с кодом 
        //  (=> сначала нужно преобр. '+'  в ' ', а потом % в символы);
        //  когда браузер переходит по ссылке он заменяет пробел на
        //  % с кодом, а '+' остаются как были
        //  (=> нужно только преобр. %  в символы)

        //  '+' заменяются на пробелы
        if (delete_spaces)
        {
            while (ptr = (TCHAR*)_tcschr(ptr, '+'))
            {
                *ptr = ' ';
                ptr++;
            }
        }

        ptr = query;
        //  преобразование символов, заданных с пом. кодов
        while (ptr = (TCHAR*)_tcschr(ptr, '%'))
        {
            if ((int)(ptr - query) <= query_len - 3)
            {
                TCHAR chr[2];
                TCHAR saved;

                chr[0] = (hex_char_to_int(*(ptr + 1)) << 4)
                    + hex_char_to_int(*(ptr + 2));
                chr[1] = 0;

                saved = *ptr;
                *ptr = 0;
                _tcscat(temp, prev_ptr);
                *ptr = saved;

                _tcscat(temp, chr);
                ptr += 3;
                prev_ptr = ptr;
            }
            else break;
        }
        _tcscat(temp, prev_ptr);            //  дописываем остаток строки

        query = temp;                       //  пред. ук-ль не освобождается,
                                            //  поскольку получен через getenv
    }
    else
    {
        out_of_memory();
        query = 0;                          //  чтобы не было недопустимого осв. указателя
    }
}
//-------------------------------------------------------------------------------------------------
//  здесь ошибки не фатальны
void create_url_file(
    const TCHAR *name,                      //  имя файла без пути
    const TCHAR *url)
{
#ifdef EXTENDED_URL_FILE
    //  в качестве имени файла исп. случайное числовое значение
    //  при использовании случайных названий полученных с пом.
    //  tmktemp оказалось, что эти названия часто повторялись
    TCHAR t[9];
    //srand((unsigned)time(NULL));
    _tsprintf(t, _T("%08x"), /*(int)rand()*/(int)time(0));

    std::wstring temp = t + Ext;

    FILE *f = _tfopen(temp.c_str(), _T("w"));
    if (f)
    {
        _ftprintf(f, url_file_template, url, name);
        fclose(f);
    }
#else
    TCHAR *full_file_name = malloc((_tcslen(name) + _tcslen(ext) + 1) * sizeof(TCHAR));
    if (full_file_name)
    {
        _tcscpy(full_file_name, name);
        _tcscat(full_file_name, ext);
        {
            FILE *f = _tfopen(full_file_name, _T("w"));
            if (f)
            {
                _ftprintf(f, url_file_template, url);
                fclose(f);
            }
        }
        free(full_file_name);
    }
#endif
}
//-------------------------------------------------------------------------------------------------

//  выводит содержимое текущей папки
void print_folder_content()
{
    Bookmarks::PageBootstrap p;
    p.Render();
}
//-------------------------------------------------------------------------------------------------

void do_edit_conf()
{
    //  command здесь не д.б. = 0
    //  link содержит имя файла
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if (link)
    {
        link++;
        change_folder();
        if (!fatal_error)
        {
            Bookmarks::FileReader fr(cwd);
            std::wstring url = fr.GetParamCurDir(link, ParamURL);
            //  name содержит название без расширения
            std::wstring name;
#ifdef EXTENDED_URL_FILE
            name = fr.GetParamCurDir(link, ParamName);
            if (name.empty())
#endif
            {   // название в файле не задано или не поддерживается -
                // сделаем название из имени файла без расширения
                TCHAR *dot;

                name = link;
                dot = (TCHAR*)_tcsrchr(link, '.');
                if (dot)
                {
                    *dot = 0;
                    name = link;
                    *dot = '.';
                }
            }
            else
            {
                _tprintf(htm_edit, full_script_name, name, query, url, key, link, key);
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------
//  редактирование ссылки
void do_edit()
{
    //  command здесь не д.б. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if (link)
    {
        link++;
        if (!_tcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_EDIT)
            {// теперь после = старое имя ссылки
                TCHAR *old_name = (TCHAR*)_tcschr(command, '=');
                if (old_name)
                {
                    old_name++;
                    if (get_query_command(0) == CMD_EDIT)
                    {// теперь после = новое имя ссылки (без расширения)
                        TCHAR *new_name = (TCHAR*)_tcschr(command, '=');
                        if (new_name)
                        {
#ifdef EXTENDED_URL_FILE
                            TCHAR *and;
                            new_name++;
                            if (and = (TCHAR*)_tcsrchr(new_name, '&'))*and = 0;
#else
                            TCHAR *and;
                            TCHAR *dot;
                            TCHAR *new_name_ext;
                            new_name++;
                            new_name_ext = new_name;
                            //  из нового имени нужно удалить символ &
                            if (and = (TCHAR*)_tcsrchr(new_name, '&'))*and = 0;
                            //  нужно соединить новое имя со старым расширением
                            if (dot = (TCHAR*)_tcsrchr(old_name, '.'))
                            {// есть расширение
                                new_name_ext = malloc((_tcslen(new_name) + _tcslen(dot) + 1) * sizeof(TCHAR));
                                if (new_name_ext)
                                {
                                    _tcscpy(new_name_ext, new_name);
                                    _tcscat(new_name_ext, dot);
                                }
                                else out_of_memory();
                            }
#endif
                            if (!fatal_error)
                            {// переход в папку и переименование файла
                                change_folder();
                                if (!fatal_error)
                                {
#ifdef EXTENDED_URL_FILE
                                    Bookmarks::FileReader fr(cwd);
                                    std::wstring url = fr.GetParamCurDir(old_name, ParamURL);
                                    //  замена файла (!!! остальная информация из файла теряется !!!)
                                    create_url_file(new_name, url.c_str());
                                    _tremove(old_name);
#else
                                    //  переименование
                                    _trename(old_name, new_name_ext);
#endif
                                    print_folder_content();
                                }
#ifndef EXTENDED_URL_FILE
                                if (new_name_ext != new_name)free(new_name_ext);
#endif
                            }
                        }
                        else invalid_query();
                    }
                    else invalid_query();
                }
                else invalid_query();
            }
            else invalid_query();
        }
        else
            if (!_tcscmp(link, cancel))
            {// в этой подкоманде старое имя ссылки сокращено
                if (get_query_command(0) == CMD_EDIT)
                {// теперь после = новое имя ссылки
                    change_folder();
                    if (!fatal_error)print_folder_content();
                }
                else invalid_query();
            }
            else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_add_conf()
{
    _tprintf(htm_add, full_script_name, query, key, key);
}
//-------------------------------------------------------------------------------------------------

void do_log_in_conf()
{
    _tprintf(htm_log_in_conf, full_script_name);
}
//-------------------------------------------------------------------------------------------------

check_log_in_result check_log_in_params()
{
    //  command здесь не д.б. = 0
    //  komanda nahoditsya za =
    TCHAR *link = (TCHAR*)_tcsrchr(query, '=');
    if (link)
    {
        link++;
        if (!_tcscmp(link, ok))
        {   //  izvlekaetsya sleduyuschaya chast' stroki - parol'
            if (get_query_command(0) == CMD_LOG_IN)
            {// teper' posle = parol'
                if (password = (TCHAR*)_tcsrchr(query, '='))
                {
                    TCHAR *and;
                    password++;
                    //  iz password nuzhno udalit' simvol &
                    if (and = (TCHAR*)_tcschr(password, '&'))*and = 0;
                    //  Удаляет остаток параметра-пароля.
                    if (get_query_command(0) == CMD_LOG_IN)
                    {// teper' posle = imya pol'zovatelya
                        if (username = (TCHAR*)_tcsrchr(query, '='))
                        {
                            username++;
                            // из имени нужно удалить символ &
                            if (and = (TCHAR*)_tcsrchr(username, '&'))*and = 0;
                            //  Удаляет остаток параметра-имени.
                            if (get_query_command(0) == CMD_LOG_IN)
                            {
                                // proverka imeni pol'zovatelya i parolya
                                if (!_tcscmp(username, _T("lvbnhbq vjhjpjd")) && !_tcscmp(password, _T("dct pfrkflrb")))
                                {
                                    return nullptr;
                                }
                                else return do_log_in_conf;
                            }
                            else return invalid_query;
                        }
                        else return invalid_query;
                    }
                    else return invalid_query;
                }
                else return invalid_query;
            }
            else return invalid_query;
        }
        else return do_log_in_conf;
    }
    else return invalid_query;
}

//-------------------------------------------------------------------------------------------------
//  Возвращает имя файла ключа сессии.
std::wstring get_key_file_name()
{
    //  Создание ключа.
    time_t t;
    time(&t);
    //  Д. б. статическим.
    static TCHAR k[11];
    _tsprintf(k, _T("%i"), (int)t);
    key = k;

    return std::wstring(tmp) + k;
}

//-------------------------------------------------------------------------------------------------
//  проверка имени пользователя и пароля и вход
void do_log_in()
{
    check_log_in_result f = check_log_in_params();
    if (!f)
    {
        std::wstring file_name = get_key_file_name();
        FILE *f;
        //  proverka suschestvovaniya fayla
        f = _tfopen(file_name.c_str(), _T("r"));
        if (f != NULL) do_log_in_conf(); //  trebuetsya povtornyi vvod -
                                         //  drugaya kopiya skripta uzhe
                                         //  sozdala fayl
        else
        {
            f = _tfopen(file_name.c_str(), _T("w"));
            if (f != NULL)
            {
                if (_tcslen(username) > MAX_USER_NAME - 1)
                {
                    fclose(f);
                    _tremove(file_name.c_str());
                    out_of_memory();
                }
                else
                {
                    _ftprintf(f, _T("%s\r\n"), username);
                    fclose(f);
                    do_change_folder();
                }
            }
            else out_of_memory();
        }
    }
    else f();
}
//-------------------------------------------------------------------------------------------------
//  добавление ссылки
void do_add()
{
    //  command здесь не д.б. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if (link)
    {
        link++;
        if (!_tcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_ADD)
            {// теперь после = url новой ссылки
                TCHAR *url = (TCHAR*)_tcschr(command, '=');
                if (url)
                {
                    TCHAR *and;
                    url++;
                    //  из нового url нужно удалить символ &
                    if (and = (TCHAR*)_tcsrchr(url, '&'))*and = 0;

                    if (get_query_command(0) == CMD_ADD)
                    {// теперь после = имя новой ссылки (без расширения)
                        TCHAR *name = (TCHAR*)_tcschr(command, '=');
                        if (name)
                        {
                            name++;
                            //  из имени нужно удалить символ &
                            if (and = (TCHAR*)_tcsrchr(name, '&'))*and = 0;
                            // переход в папку и создание файла .url
                            change_folder();
                            if (!fatal_error)
                            {
                                create_url_file(name, url);
                                if (!fatal_error)print_folder_content();
                            }
                        }
                        else invalid_query();
                    }
                    else invalid_query();
                }
                else invalid_query();
            }
            else invalid_query();
        }
        else
            if (!_tcscmp(link, cancel))
            {
                if (get_query_command(0) == CMD_ADD)
                {// теперь после = url новой ссылки
                    if (get_query_command(0) == CMD_ADD)
                    {// теперь после = имя новой ссылки
                        change_folder();
                        if (!fatal_error)print_folder_content();
                    }
                    else invalid_query();
                }
                else invalid_query();
            }
            else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------
//  удаление ссылки
void do_del()
{
    //  распознавание результата
    TCHAR *result = (TCHAR*)_tcschr(command, '=');
    if (result)
    {
        if (get_query_command(0) == CMD_DEL)
        {// теперь после = имя папки
            if (!_tcsncmp(result + 1, ok, _tcslen(ok)))
            {
                change_folder();
                if (!fatal_error)
                {
                    result = (TCHAR*)_tcschr(command, '=');
                    if (result)
                    {
                        result++;
                        // удаление ссылки - в result - имя файла
                        _tremove(result);
                        print_folder_content();
                    }
                }
            }
            else
            {//  отображение содержимого текущей папки
                do_change_folder();
            }
        }
        else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------
//  запрос на удаление ссылки
void do_del_conf()
{
    //  command здесь не д.б. = 0
    //  искать надо вправо от command
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if (link)
    {
        link++;
        change_folder();
        if (!fatal_error)
        {
            Bookmarks::FileReader fr(cwd);
            std::wstring url = fr.GetParamCurDir(link, ParamURL);
            std::wstring name;
#ifdef EXTENDED_URL_FILE
            name = fr.GetParamCurDir(link, ParamName);
            if (name.empty())name = link;
#else
            name = link;
#endif
            _tprintf(htm_del_conf, full_script_name, name, url, query, link, key, query, link, key);
        }
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_edit_folder_conf()
{
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if (link)
    {
        link++;
        _tprintf(htm_edit_folder, full_script_name, link, query, key, link, key);
    }
}
//-------------------------------------------------------------------------------------------------
void do_edit_folder()
{
    //  command здесь не д.б. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if (link)
    {
        link++;
        if (!_tcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_EDIT_FOLDER)
            {// теперь после = старое имя папки
                TCHAR *old_name = (TCHAR*)_tcschr(command, '=');
                if (old_name)
                {
                    old_name++;
                    if (get_query_command(0) == CMD_EDIT_FOLDER)
                    {// теперь после = новое имя папки
                        TCHAR *new_name = (TCHAR*)_tcschr(command, '=');
                        if (new_name)
                        {
                            TCHAR *and;
                            new_name++;
                            //  из нового имени нужно удалить символ &
                            if (and = (TCHAR*)_tcsrchr(new_name, '&'))*and = 0;

                            // переход в папку и переименование файла
                            change_folder();
                            if (!fatal_error)
                            {// переименование
                                _trename(old_name, new_name);
                                print_folder_content();
                            }
                        }
                        else invalid_query();
                    }
                    else invalid_query();
                }
                else invalid_query();
            }
            else invalid_query();
        }
        else
            if (!_tcscmp(link, cancel))
            {// в этой подкоманде старое имя ссылки сокращено
                if (get_query_command(0) == CMD_EDIT_FOLDER)
                {// теперь после = новое имя ссылки
                    change_folder();
                    if (!fatal_error)print_folder_content();
                }
                else invalid_query();
            }
            else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_add_folder_conf()
{
    change_folder();
    if (!fatal_error)
    {
        _tprintf(htm_add_folder, full_script_name, query, key, key);
    }
}
//-------------------------------------------------------------------------------------------------
void do_add_folder()
{
    //  command здесь не д.б. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if (link)
    {
        link++;
        if (!_tcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_ADD_FOLDER)
            {// теперь после = имя новой папки
                TCHAR *name = (TCHAR*)_tcschr(command, '=');
                if (name)
                {
                    TCHAR *and;
                    name++;
                    //  из имени нужно удалить символ &
                    if (and = (TCHAR*)_tcsrchr(name, '&'))*and = 0;
                    // переход в папку и создание файла .url
                    change_folder();
                    if (!fatal_error)
                    {
                        _tmkdir(name);
                        print_folder_content();
                    }
                }
                else invalid_query();
            }
            else invalid_query();
        }
        else
            if (!_tcscmp(link, cancel))
            {
                if (get_query_command(0) == CMD_ADD_FOLDER)
                {// теперь после = имя новой ссылки
                    change_folder();
                    if (!fatal_error)print_folder_content();
                }
                else invalid_query();
            }
            else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------
//  удаление папки
void do_del_folder()
{
    //  распознавание результата
    TCHAR *result = (TCHAR*)_tcschr(command, '=');
    if (result)
    {
        if (!_tcsncmp(result + 1, ok, _tcslen(ok)))
        {
            if (get_query_command(0) == CMD_DEL_FOLDER)
            {// теперь после = имя папки
                change_folder();
                if (!fatal_error)
                {
                    result = (TCHAR*)_tcschr(command, '=');
                    if (result)
                    {
                        result++;
                        // удаление папки - в result - имя папки
                        remove_folder(result);
                        print_folder_content();
                    }
                }
                else invalid_query();

            }
            else invalid_query();
        }
        else
        {//  отображение содержимого текущей папки
            do_change_folder();
        }
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------
//  запрос на удаление папки
void do_del_folder_conf()
{
    //  command здесь не д.б. = 0
    TCHAR *folder = (TCHAR*)_tcschr(command, '=');
    if (folder)
    {
        folder++;
        _tprintf(htm_folder_del_conf, full_script_name, folder, query, key, query, folder, key);
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_change_folder()
{
    change_folder();
    if (!fatal_error)
        print_folder_content();
}
//-------------------------------------------------------------------------------------------------

int recognize_command(TCHAR *command/*команда без ;*/)
{
    int result;
    if (!_tcsncmp(command, cmd_edit + 1, _tcslen(cmd_edit) - 1)) { result = CMD_EDIT; }
    else
        if (!_tcsncmp(command, cmd_edit_conf + 1, _tcslen(cmd_edit_conf) - 1)) { result = CMD_EDIT_CONF; }
        else
            if (!_tcsncmp(command, cmd_add + 1, _tcslen(cmd_add) - 1)) { result = CMD_ADD; }
            else
                if (!_tcsncmp(command, cmd_log_in + 1, _tcslen(cmd_log_in) - 1)) { result = CMD_LOG_IN; }
                else
                    if (!_tcsncmp(command, cmd_add_conf + 1, _tcslen(cmd_add_conf) - 1)) { result = CMD_ADD_CONF; }
                    else
                        if (!_tcsncmp(command, cmd_del_conf + 1, _tcslen(cmd_del_conf) - 1)) { result = CMD_DEL_CONF; }
                        else
                            if (!_tcsncmp(command, cmd_del + 1, _tcslen(cmd_del) - 1)) { result = CMD_DEL; }
                            else
                                if (!_tcsncmp(command, cmd_edit_folder + 1, _tcslen(cmd_edit_folder) - 1)) { result = CMD_EDIT_FOLDER; }
                                else
                                    if (!_tcsncmp(command, cmd_edit_folder_conf + 1, _tcslen(cmd_edit_folder_conf) - 1)) { result = CMD_EDIT_FOLDER_CONF; }
                                    else
                                        if (!_tcsncmp(command, cmd_add_folder + 1, _tcslen(cmd_add_folder) - 1)) { result = CMD_ADD_FOLDER; }
                                        else
                                            if (!_tcsncmp(command, cmd_add_folder_conf + 1, _tcslen(cmd_add_folder_conf) - 1)) { result = CMD_ADD_FOLDER_CONF; }
                                            else
                                                if (!_tcsncmp(command, cmd_del_folder_conf + 1, _tcslen(cmd_del_folder_conf) - 1)) { result = CMD_DEL_FOLDER_CONF; }
                                                else
                                                    if (!_tcsncmp(command, cmd_ch_folder + 1, _tcslen(cmd_ch_folder) - 1)) { result = CMD_CHANGE_FOLDER; }
                                                    else
                                                        if (!_tcsncmp(command, cmd_del_folder + 1, _tcslen(cmd_del_folder) - 1)) { result = CMD_DEL_FOLDER; }
                                                        else
                                                            if (!_tcsncmp(command, cmd_key + 1, _tcslen(cmd_key) - 1)) { result = CMD_KEY; }
                                                            else { invalid_query(); result = CMD_UNKNOWN; }
                                                            return result;
}
//-------------------------------------------------------------------------------------------------
//  !!! распознанная команда удаляется из строки запроса !!!
int get_query_command(unsigned char save_command /* priznak togo, chto zapros peredaetsya v pervonachal'nom vide -
                                                    zapret udaleniya komandy iz stroki */)
{
    if (save_command)
    {
        //  poisk ; v kodirovannoy forme
        command = query + _tcslen(query) - 3;
        for (;command >= query; command--)
        {
            if ((command[0] == '%') && (command[1] == '3') &&
                ((command[2] == 'B') || (command[2] == 'b')))
            {
                command += 3;
                //  если есть %, то значит запрос не обработан
                return recognize_command(command);
            }
        }
        //  komanda ne naydena
        //return CMD_LOG_IN_CONF; nuzhna proverka ; kotoraya est' v ssylkah
    }

    command = (TCHAR*)_tcsrchr(query, ';');
    if (!command)return CMD_LOG_IN_CONF;
    else
    {
        //  teper' mozhno udalit' komandu iz stroki...
        if (!save_command)(*command) = 0;
        command++;
        return recognize_command(command);  //  ...i preobrazovat' ee v kod
    }
}
//-------------------------------------------------------------------------------------------------

void begin_error_box()
{
    _tprintf(_T("%s"), _T("<br><hr>\n<font color=\"red\">"));
}
//-------------------------------------------------------------------------------------------------

void end_error_box()
{
    _tprintf(_T("%s"), _T("</font><hr><br>\n"));
}
//-------------------------------------------------------------------------------------------------

int hex_char_to_int(TCHAR c)
{
    switch (c)
    {
    default:
    case('0'): return 0;
    case('1'): return 1;
    case('2'): return 2;
    case('3'): return 3;
    case('4'): return 4;
    case('5'): return 5;
    case('6'): return 6;
    case('7'): return 7;
    case('8'): return 8;
    case('9'): return 9;
    case('a'):
    case('A'): return 10;
    case('b'):
    case('B'): return 11;
    case('c'):
    case('C'): return 12;
    case('d'):
    case('D'): return 13;
    case('e'):
    case('E'): return 14;
    case('f'):
    case('F'): return 15;
    }
}
//-------------------------------------------------------------------------------------------------

void no_environment(TCHAR *env_str)
{
    Bookmarks::Page::PrintHtmlHead(head_error);
    begin_error_box();
    _tprintf(_T("%s%s<BR>\n"), err_no_environment, env_str);
    end_error_box();
    error = E_NO_ENVIRONMENT;
    fatal_error = 1;
    Bookmarks::Page::PrintHtmlTail();
}
//-------------------------------------------------------------------------------------------------

void invalid_query()
{
    Bookmarks::Page::PrintHtmlHead(head_error);
    begin_error_box();
    _tprintf(_T("%s%s<BR>\n"), err_invalid_query, query);
    end_error_box();
    error = E_INVALID_QUERY;
    fatal_error = 1;
    Bookmarks::Page::PrintHtmlTail();
}
//-------------------------------------------------------------------------------------------------

void out_of_memory()
{
    Bookmarks::Page::PrintHtmlHead(head_error);
    begin_error_box();
    _tprintf(_T("%s<BR>\n"), err_out_of_memory);
    end_error_box();
    error = E_OUTOFMEMORY;
    fatal_error = 1;
    Bookmarks::Page::PrintHtmlTail();
}
//-------------------------------------------------------------------------------------------------
//  переход в требуемую папку (по отношению к заданной корневой папке)
void change_folder()
{
    //  создается строка для размещения имени папки
    int query_len = _tcslen(query);
    TCHAR *temp = (TCHAR *)malloc((query_len + _tcslen(www) + _tcslen(username) + 2) * sizeof(TCHAR));

    if (temp)
    {
        _tcscpy(temp, www);                 //  imya papki dannyh
        _tcscat(temp, username);            //  imya pol'zovatelya
        _tcscat(temp, _T("/"));
        _tcscat(temp, query);               //  imya podpapki
                                            //  переход в запрошенную папку
        if (-1 == _tchdir(temp))
        {
            Bookmarks::Page::PrintHtmlHead(head_error);
            begin_error_box();
            _tprintf(_T("%s%s<BR>\n"), err_change_folder, temp);
            fatal_error = 1;
            end_error_box();
            error = E_CHANGE_FOLDER;
            Bookmarks::Page::PrintHtmlTail();
        }

        free(temp);

        if (cwd)free(cwd);
        cwd = (TCHAR*)_tgetcwd(0, 0);
        if (!cwd)
            out_of_memory();
    }
    else
        out_of_memory();
}
//-------------------------------------------------------------------------------------------------

void exec_utility(TCHAR *folder, TCHAR *util)
{
    TCHAR *utility = (TCHAR *)malloc((_tcslen(util) +
#ifdef LINUX
        _tcslen(cwd) + 1 +
#endif
        _tcslen(folder) + 3) * sizeof(TCHAR));
    if (utility)
    {
        _tcscpy(utility, util);
        _tcscat(utility, _T("\""));                 //  кавычки требуются, чтобы удалялись папки с пробелами
#ifdef LINUX
        _tcscat(utility, cwd);
        _tcscat(utility, _T("/"));
#endif
        _tcscat(utility, folder);
        _tcscat(utility, _T("\""));

        if (-1 == _tsystem(utility))
        {
            error = E_SYS_UTILITY;
            //  oschibka ne fatal'na
        }
        free(utility);
    }
    else
    {
        error = E_OUTOFMEMORY;
        //  oschibka ne fatal'na
    }
}
//-------------------------------------------------------------------------------------------------

void remove_folder(TCHAR *folder)
{
    exec_utility(folder, rm);
}
//-------------------------------------------------------------------------------------------------

void get_key()
{
    TCHAR *saved_command = command;     //  sohranenie
    if (CMD_KEY == get_query_command(0)) //  udalenie klyucha iz zaprosa
    {
        TCHAR file_name[_MAX_PATH];
        FILE *f;

        key = (TCHAR*)_tcsrchr(command, '=');
        key++;
        //  chteniye fayla klyucha
        _tcscpy(file_name, tmp);
        _tcscat(file_name, key);
        //  proverka suschestvovaniya fayla
        f = _tfopen(file_name, _T("r"));
        if (f != NULL)
        {
            username = (TCHAR *)malloc(MAX_USER_NAME);
            if (username != NULL)
            {
                //fscanf(f, "%s", username);    //  ne rabotaet s probelami
                int count = 0;
                while (1)
                {
                    int c = fgetc(f);
                    if ((c == '\r') || (c == '\n') || (count == MAX_USER_NAME - 1) || (c == EOF))break;
                    username[count++] = (char)c;
                }
                username[count] = 0;
                //??? chtenie i proverka vremeni deistviya klucha
                fclose(f);
            }
            else out_of_memory();
        }
        else out_of_memory();
    }
    else invalid_query();
    command = saved_command;
}
//-------------------------------------------------------------------------------------------------
