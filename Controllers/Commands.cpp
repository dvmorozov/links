
#include "../main.h"
#include "../Models/Data.h"
#include "../Models/ConfigTest.h"
#include "../Models/FileReader.h"
#include "../Views/FileListBootstrap.h"
#include "../Views/AddLink.h"
#include "../Views/ErrorBox.h"
#include "../Utils/entities.h"
#include "Commands.h"

//  ������� ��� ���������� ���������� (����������� � url)
//  !!! ����� ������ ���������� ������ ��� ����� � ������, ��������� ����������
//  �� ���������� ����� url, � ������ ����� ����� ���������� ����� � ���� � 
//  ���������� ������ !!!
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

int error = 0;
int fatal_error = 0;                                                //  priznak vyvoda soobscheniya ob oschibke

                                                                    //  ������� �������
                                                                    //  ����� Cancel � �� �.�. ������ ��, ��� � ������� cancel � ok
TCHAR htm_folder_del_conf[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>�������� �����</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>�� ������������� ������ ������� ����� <font color=\"red\">%s</font> � ��� � ����������?</P>\n\
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
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>�������� ������</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>�� ������������� ������ ������� ������ <font color=\"red\">%s</font>, ����������� �� <font color=\"red\">%s</font> ?</P>\n\
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
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>�������������� ������</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                ��� ������:<br>\n\
                <input type=\"text\" size=50 value=\"%s\" name=\"%s;edit\"><br>\n\
                ������ ��������� �� %s\n\
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
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>�������������� �����</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                ��� �����:<br>\n\
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

TCHAR htm_add_folder[] = _T("<html>\n\
    <head>\n\
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>�������� �����</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                ��� �����:<br>\n\
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
        <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
        <meta http-equiv=\"Content-Language\" content=\"ru\">\
        <title>����</title>\n\
    </head>\n\
    <body>\n\
        <form method=get action=\"%s\">\n\
            <p>\n\
                ��� ������������:<br>\n\
                <input type=\"text\" size=50 value=\"\" name=\";log_in\"><br>\n\
                ������:<br>\n\
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

//  ������������ ������� ��������� ������, ������ ���
//  C-������� _rmdir �������, ����� ����� ���� �����
#if !defined LINUX
//TCHAR rm[] = _T("rmdir /S /Q ");                      //  ����� ��� Windows
TCHAR rm[] = _T("rm -r -f ");                           //  ����� ��� Cygwin
#else
TCHAR rm[] = _T("rm -r -f -d ");                        //  ����� ��� Linux
                                                        //TCHAR rmdir[] = _T("rmdir ");
#endif

TCHAR err_change_folder[] = _T("���������� ������� � �����: ");
TCHAR err_out_of_memory[] = _T("������������ ������ ��� ����������");
TCHAR err_invalid_query[] = _T("������������ ������: ");
TCHAR err_what[] = _T("������: ");
TCHAR err_no_environment[] = _T("����������� ��������� ���������: ");
TCHAR err_sys_utility[] = _T("����� ������� %s ���������� � �������");

//  !!! ��������� ����� ��� ��������� ���� ������� - ������ �.�. ������ ������� !!!
TCHAR *query = 0;                                       //  �� ����������� ��������� 0
TCHAR *full_script_name = 0;                            //  �� ����������� ��������� 0
TCHAR *img_path = 0;                                    //  !!! �������� ����������� ������ / !!!
TCHAR *cwd = 0;                                         //  ���� � �������� ��������
                                                        //  ����������� ������ / �� ����������
                                                        //  �� ����������� ��������� 0
TCHAR *command = 0;                                     //  ��-�� �� ���������� � ������� ������� (������������� ������ � ������������� �������)
TCHAR *key = 0;                                         //  ���� ������, ���������� �� �������
//  ��-�� �� ��������� � ������ query.
TCHAR *username = 0;
TCHAR *password = 0;
#define MAX_USER_NAME       100
//-------------------------------------------------------------------------------------------------

void process_query(unsigned char delete_spaces)
{
    int query_len = wcslen(query);
    TCHAR *temp = (TCHAR *)malloc((query_len + 1) * sizeof(TCHAR));
    if (temp)
    {
        //  https://action.mindjet.com/task/14817423
        decode_url(temp, query, delete_spaces);
        decode_html_entities_utf8(query, temp);
        free(temp);
    }
    else
        out_of_memory();
}
//-------------------------------------------------------------------------------------------------

//  ������� ���������� ������� �����
void print_folder_content()
{
    //  https://action.mindjet.com/task/14703903
#ifdef LINKS_LEGACY
    Bookmarks::FileListLegacy p;
#else
    Bookmarks::FileListBootstrap p;
#endif
    p.Render();
}
//-------------------------------------------------------------------------------------------------

void do_edit_conf()
{
    //  command ����� �� �.�. = 0
    //  link �������� ��� �����
    TCHAR *link = (TCHAR*)wcschr(command, '=');
    if (link)
    {
        link++;
        change_folder();
        if (!fatal_error)
        {
            Bookmarks::FileReader fr(cwd);
            std::wstring url = fr.GetParamCurDir(link, ParamURL);
            //  name �������� �������� ��� ����������
            std::wstring name;
#ifdef EXTENDED_URL_FILE
            name = fr.GetParamCurDir(link, ParamName);
            if (name.empty())
#endif
            {   // �������� � ����� �� ������ ��� �� �������������� -
                // ������� �������� �� ����� ����� ��� ����������
                TCHAR *dot;

                name = link;
                dot = (TCHAR*)wcsrchr(link, '.');
                if (dot)
                {
                    *dot = 0;
                    name = link;
                    *dot = '.';
                }
            }
            else
            {
                _tprintf(htm_edit, full_script_name, name.c_str(), query, url.c_str(), key, link, key);
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------
//  �������������� ������
void do_edit()
{
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)wcschr(command, '=');
    if (link)
    {
        link++;
        if (!wcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_EDIT)
            {// ������ ����� = ������ ��� ������
                TCHAR *old_name = (TCHAR*)wcschr(command, '=');
                if (old_name)
                {
                    old_name++;
                    if (get_query_command(0) == CMD_EDIT)
                    {// ������ ����� - ����� ��� ������ (��� ����������)
                        TCHAR *new_name = (TCHAR*)wcschr(command, '=');
                        if (new_name)
                        {
#ifdef EXTENDED_URL_FILE
                            TCHAR *and;
                            new_name++;
                            if (and = (TCHAR*)wcsrchr(new_name, '&'))*and = 0;
#else
                            TCHAR *and;
                            TCHAR *dot;
                            TCHAR *new_name_ext;
                            new_name++;
                            new_name_ext = new_name;
                            //  �� ������ ����� ����� ������� ������ &
                            if (and = (TCHAR*)wcsrchr(new_name, '&'))*and = 0;
                            //  ����� ��������� ����� ��� �� ������ �����������
                            if (dot = (TCHAR*)wcsrchr(old_name, '.'))
                            {// ���� ����������
                                new_name_ext = malloc((wcslen(new_name) + wcslen(dot) + 1) * sizeof(TCHAR));
                                if (new_name_ext)
                                {
                                    wcscpy(new_name_ext, new_name);
                                    wcscat(new_name_ext, dot);
                                }
                                else out_of_memory();
                            }
#endif
                            if (!fatal_error)
                            {// ������� � ����� � �������������� �����
                                change_folder();
                                if (!fatal_error)
                                {
#ifdef EXTENDED_URL_FILE
                                    Bookmarks::FileReader fr(cwd);
                                    std::wstring url = fr.GetParamCurDir(old_name, ParamURL);
                                    //  ������ ����� (!!! ��������� ���������� �� ����� �������� !!!)
                                    create_url_file(new_name, url.c_str());
                                    _wremove(old_name);
#else
                                    //  ��������������
                                    _wrename(old_name, new_name_ext);
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
            if (!wcscmp(link, cancel))
            {// � ���� ���������� ������ ��� ������ ���������
                if (get_query_command(0) == CMD_EDIT)
                {// ������ ����� = ����� ��� ������
                    change_folder();
                    print_folder_content();
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
    Bookmarks::AddLink p;
    p.Render();
}
//-------------------------------------------------------------------------------------------------

void do_log_in_conf()
{
    _tprintf(htm_log_in_conf, full_script_name);
}
//-------------------------------------------------------------------------------------------------

check_log_in_result check_log_in_params()
{
    //  command ����� �� �.�. = 0
    //  komanda nahoditsya za =
    TCHAR *link = (TCHAR*)wcsrchr(query, '=');
    if (link)
    {
        link++;
        if (!wcscmp(link, ok))
        {   //  izvlekaetsya sleduyuschaya chast' stroki - parol'
            if (get_query_command(0) == CMD_LOG_IN)
            {// teper' posle = parol'
                if (password = (TCHAR*)wcsrchr(query, '='))
                {
                    TCHAR *and;
                    password++;
                    //  iz password nuzhno udalit' simvol &
                    if (and = (TCHAR*)wcschr(password, '&'))*and = 0;
                    //  ������� ������� ���������-������.
                    if (get_query_command(0) == CMD_LOG_IN)
                    {// teper' posle = imya pol'zovatelya
                        if (username = (TCHAR*)wcsrchr(query, '='))
                        {
                            username++;
                            // �� ����� ����� ������� ������ &
                            if (and = (TCHAR*)wcschr(username, '&'))*and = 0;
                            //  ������� ������� ���������-�����.
                            if (get_query_command(0) == CMD_LOG_IN)
                            {
                                //  Check username and password.
                                if (!wcscmp(username, Bookmarks::RegConfig::GetValue(_T("UserName")).c_str()) &&
                                    !wcscmp(password, Bookmarks::RegConfig::GetValue(_T("Password")).c_str()))
                                    return nullptr;
                                //  Return to the login page.
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
//  ���������� ��� ����� ����� ������.
std::wstring getKeyFileName()
{
    //  �������� �����.
    time_t t;
    time(&t);
    //  �. �. �����������.
    static TCHAR k[11];
    swprintf(k, _T("%i"), (int)t);
    key = k;
    return GetTmpDirName() + k;
}
//-------------------------------------------------------------------------------------------------
//  �������� ����� ������������ � ������ � ����
void do_log_in()
{
    check_log_in_result f = check_log_in_params();
    if (!f)
    {
        std::wstring file_name = getKeyFileName();
        FILE *f;
        //  proverka suschestvovaniya fayla
        f = _wfopen(file_name.c_str(), _T("r"));
        if (f != NULL) do_log_in_conf(); //  trebuetsya povtornyi vvod -
                                         //  drugaya kopiya skripta uzhe
                                         //  sozdala fayl
        else
        {
            f = _wfopen(file_name.c_str(), _T("w"));
            if (f != NULL)
            {
                if (wcslen(username) > MAX_USER_NAME - 1)
                {
                    fclose(f);
                    _wremove(file_name.c_str());
                    out_of_memory();
                }
                else
                {
                    fwprintf(f, _T("%s\r\n"), username);
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
//  ���������� ������
void do_add()
{
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)wcschr(command, '=');
    if (link)
    {
        link++;
        if (!wcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_ADD)
            {// ������ ����� = url ����� ������
                TCHAR *url = (TCHAR*)wcschr(command, '=');
                if (url)
                {
                    TCHAR *and;
                    url++;
                    //  �� ������ url ����� ������� ������ &
                    if (and = (TCHAR*)wcsrchr(url, '&'))*and = 0;

                    if (get_query_command(0) == CMD_ADD)
                    {// ������ ����� = ��� ����� ������ (��� ����������)
                        TCHAR *name = (TCHAR*)wcschr(command, '=');
                        if (name)
                        {
                            name++;
                            //  �� ����� ����� ������� ������ &
                            if (and = (TCHAR*)wcsrchr(name, '&'))*and = 0;
                            // ������� � ����� � �������� ����� .url
                            change_folder();
                            if (!fatal_error)
                            {
                                create_url_file(name, url);
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
            if (!wcscmp(link, cancel))
            {
                if (get_query_command(0) == CMD_ADD)
                {// ������ ����� = url ����� ������
                    if (get_query_command(0) == CMD_ADD)
                    {// ������ ����� = ��� ����� ������
                        change_folder();
                        print_folder_content();
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
//  �������� ������
void do_del()
{
    //  ������������� ����������
    TCHAR *result = (TCHAR*)wcschr(command, '=');
    if (result)
    {
        if (get_query_command(0) == CMD_DEL)
        {// ������ ����� = ��� �����
            if (!wcsncmp(result + 1, ok, wcslen(ok)))
            {
                change_folder();
                if (!fatal_error)
                {
                    result = (TCHAR*)wcschr(command, '=');
                    if (result)
                    {
                        result++;
                        // �������� ������ - � result - ��� �����
                        _wremove(result);
                        print_folder_content();
                    }
                }
            }
            else
            {//  ����������� ����������� ������� �����
                do_change_folder();
            }
        }
        else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------
//  ������ �� �������� ������
void do_del_conf()
{
    //  command ����� �� �.�. = 0
    //  ������ ���� ������ �� command
    TCHAR *link = (TCHAR*)wcschr(command, '=');
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
            _tprintf(htm_del_conf, full_script_name, name.c_str(), url.c_str(), query, link, key, query, link, key);
        }
        else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_edit_folder_conf()
{
    TCHAR *link = (TCHAR*)wcschr(command, '=');
    if (link)
    {
        link++;
        _tprintf(htm_edit_folder, full_script_name, link, query, key, link, key);
    }
}
//-------------------------------------------------------------------------------------------------

void do_edit_folder()
{
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)wcschr(command, '=');
    if (link)
    {
        link++;
        if (!wcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_EDIT_FOLDER)
            {// ������ ����� = ������ ��� �����
                TCHAR *old_name = (TCHAR*)wcschr(command, '=');
                if (old_name)
                {
                    old_name++;
                    if (get_query_command(0) == CMD_EDIT_FOLDER)
                    {// ������ ����� = ����� ��� �����
                        TCHAR *new_name = (TCHAR*)wcschr(command, '=');
                        if (new_name)
                        {
                            TCHAR *and;
                            new_name++;
                            //  �� ������ ����� ����� ������� ������ &
                            if (and = (TCHAR*)wcsrchr(new_name, '&'))*and = 0;

                            // ������� � ����� � �������������� �����
                            change_folder();
                            if (!fatal_error)
                            {// ��������������
                                _wrename(old_name, new_name);
                                print_folder_content();
                            }
                            else invalid_query();
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
            if (!wcscmp(link, cancel))
            {// � ���� ���������� ������ ��� ������ ���������
                if (get_query_command(0) == CMD_EDIT_FOLDER)
                {// ������ ����� = ����� ��� ������
                    change_folder();
                    print_folder_content();
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
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)wcschr(command, '=');
    if (link)
    {
        link++;
        if (!wcscmp(link, ok))
        {
            if (get_query_command(0) == CMD_ADD_FOLDER)
            {// ������ ����� = ��� ����� �����
                TCHAR *name = (TCHAR*)wcschr(command, '=');
                if (name)
                {
                    TCHAR *and;
                    name++;
                    //  �� ����� ����� ������� ������ &
                    if (and = (TCHAR*)wcsrchr(name, '&'))*and = 0;
                    // ������� � ����� � �������� ����� .url
                    change_folder();
                    if (!fatal_error)
                    {
                        MakeFolder(name);
                        print_folder_content();
                    }
                    else invalid_query();
                }
                else invalid_query();
            }
            else invalid_query();
        }
        else
            if (!wcscmp(link, cancel))
            {
                if (get_query_command(0) == CMD_ADD_FOLDER)
                {// ������ ����� = ��� ����� ������
                    change_folder();
                    print_folder_content();
                }
                else invalid_query();
            }
            else invalid_query();
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------
//  �������� �����
void do_del_folder()
{
    //  ������������� ����������
    TCHAR *result = (TCHAR*)wcschr(command, '=');
    if (result)
    {
        if (!wcsncmp(result + 1, ok, wcslen(ok)))
        {
            if (get_query_command(0) == CMD_DEL_FOLDER)
            {// ������ ����� = ��� �����
                change_folder();
                if (!fatal_error)
                {
                    result = (TCHAR*)wcschr(command, '=');
                    if (result)
                    {
                        result++;
                        // �������� ����� - � result - ��� �����
                        remove_folder(result);
                        print_folder_content();
                    }
                }
                else invalid_query();

            }
            else invalid_query();
        }
        else
        {//  ����������� ����������� ������� �����
            do_change_folder();
        }
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------
//  ������ �� �������� �����
void do_del_folder_conf()
{
    //  command ����� �� �.�. = 0
    TCHAR *folder = (TCHAR*)wcschr(command, '=');
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
    print_folder_content();
}
//-------------------------------------------------------------------------------------------------

int recognize_command(TCHAR *command/*������� ��� ;*/)
{
    int result;
    if (!wcsncmp(command, cmd_edit + 1, wcslen(cmd_edit) - 1)) { result = CMD_EDIT; }
    else
        if (!wcsncmp(command, cmd_edit_conf + 1, wcslen(cmd_edit_conf) - 1)) { result = CMD_EDIT_CONF; }
        else
            if (!wcsncmp(command, cmd_add + 1, wcslen(cmd_add) - 1)) { result = CMD_ADD; }
            else
                if (!wcsncmp(command, cmd_log_in + 1, wcslen(cmd_log_in) - 1)) { result = CMD_LOG_IN; }
                else
                    if (!wcsncmp(command, cmd_add_conf + 1, wcslen(cmd_add_conf) - 1)) { result = CMD_ADD_CONF; }
                    else
                        if (!wcsncmp(command, cmd_del_conf + 1, wcslen(cmd_del_conf) - 1)) { result = CMD_DEL_CONF; }
                        else
                            if (!wcsncmp(command, cmd_del + 1, wcslen(cmd_del) - 1)) { result = CMD_DEL; }
                            else
                                if (!wcsncmp(command, cmd_edit_folder + 1, wcslen(cmd_edit_folder) - 1)) { result = CMD_EDIT_FOLDER; }
                                else
                                    if (!wcsncmp(command, cmd_edit_folder_conf + 1, wcslen(cmd_edit_folder_conf) - 1)) { result = CMD_EDIT_FOLDER_CONF; }
                                    else
                                        if (!wcsncmp(command, cmd_add_folder + 1, wcslen(cmd_add_folder) - 1)) { result = CMD_ADD_FOLDER; }
                                        else
                                            if (!wcsncmp(command, cmd_add_folder_conf + 1, wcslen(cmd_add_folder_conf) - 1)) { result = CMD_ADD_FOLDER_CONF; }
                                            else
                                                if (!wcsncmp(command, cmd_del_folder_conf + 1, wcslen(cmd_del_folder_conf) - 1)) { result = CMD_DEL_FOLDER_CONF; }
                                                else
                                                    if (!wcsncmp(command, cmd_ch_folder + 1, wcslen(cmd_ch_folder) - 1)) { result = CMD_CHANGE_FOLDER; }
                                                    else
                                                        if (!wcsncmp(command, cmd_del_folder + 1, wcslen(cmd_del_folder) - 1)) { result = CMD_DEL_FOLDER; }
                                                        else
                                                            if (!wcsncmp(command, cmd_key + 1, wcslen(cmd_key) - 1)) { result = CMD_KEY; }
                                                            else { invalid_query(); result = CMD_UNKNOWN; }
                                                            return result;
}
//-------------------------------------------------------------------------------------------------
//  !!! ������������ ������� ��������� �� ������ ������� !!!
int get_query_command(unsigned char save_command /* priznak togo, chto zapros peredaetsya v pervonachal'nom vide -
                                                    zapret udaleniya komandy iz stroki */)
{
    if (save_command)
    {
        //  poisk ; v kodirovannoy forme
        command = query + wcslen(query) - 3;
        for (;command >= query; command--)
        {
            if ((command[0] == '%') && (command[1] == '3') &&
                ((command[2] == 'B') || (command[2] == 'b')))
            {
                command += 3;
                //  ���� ���� %, �� ������ ������ �� ���������
                return recognize_command(command);
            }
        }
        //  komanda ne naydena
        //return CMD_LOG_IN_CONF; nuzhna proverka ; kotoraya est' v ssylkah
    }

    command = (TCHAR*)wcsrchr(query, ';');
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

void no_environment(TCHAR *env_str)
{
    error = E_NO_ENVIRONMENT;
    fatal_error = 1;
    throw two_msg_exception(err_no_environment, env_str);
}
//-------------------------------------------------------------------------------------------------

const wchar_t *GetWC(const char *c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}
//-------------------------------------------------------------------------------------------------

void invalid_query()
{
    error = E_INVALID_QUERY;
    fatal_error = 1;
    throw two_msg_exception(err_invalid_query, query);
}
//-------------------------------------------------------------------------------------------------

void print_exception(const char* what)
{
    if (what)
    {
        const wchar_t *whatWide = GetWC(what);
        Bookmarks::ErrorBox eb(err_invalid_query, std::wstring(query) + _T("<br>") + std::wstring(err_what) + std::wstring(whatWide));
        delete[] whatWide;

        eb.Render();
    }
    error = E_INVALID_QUERY;
    fatal_error = 1;
}
//-------------------------------------------------------------------------------------------------

void out_of_memory()
{
    error = E_OUTOFMEMORY;
    fatal_error = 1;
    throw two_msg_exception(err_out_of_memory, _T(""));
}
//-------------------------------------------------------------------------------------------------
//  https://action.mindjet.com/task/14732139
std::wstring GetFullDirName(std::wstring relDirName)
{
    TCHAR current_dir[_MAX_PATH];
    _wgetcwd(current_dir, _MAX_PATH);

    std::wstring result = current_dir;
    size_t lastSlashPos = result.find_last_of(_T("\\/"));
    if (lastSlashPos != std::wstring::npos)
        result = result.substr(0, lastSlashPos + 1);
    result += relDirName;
    return result;
}
//-------------------------------------------------------------------------------------------------
//  Returns path to user content folder.
std::wstring GetUserDirName()
{
    std::wstring result = GetFullDirName(_T("users/links/"));
    result += username;
    result += L"/";

    result += query;
    return result;
}
//-------------------------------------------------------------------------------------------------

std::wstring GetTmpDirName()
{
    return GetFullDirName(_T("tmp/links/"));
}
//-------------------------------------------------------------------------------------------------
//  ������� � ��������� ����� (�� ��������� � �������� �������� �����)
void change_folder()
{
    if (!username)
        invalid_query();
    else
    {
        //  ��������� ������ ����� �����
        std::wstring dirName = GetUserDirName();
        //  Changes current directory.
        if (-1 == _wchdir(dirName.c_str()))
        {
            fatal_error = 1;
            error = E_CHANGE_FOLDER;
            throw two_msg_exception(err_change_folder, dirName.c_str());
        }

        if (cwd)free(cwd);
        cwd = (TCHAR*)_wgetcwd(0, 0);
        if (!cwd)
            out_of_memory();
    }
}
//-------------------------------------------------------------------------------------------------

void exec_utility(TCHAR *folder, TCHAR *util)
{
    TCHAR *utility = (TCHAR *)malloc((wcslen(util) +
#ifdef LINUX
        wcslen(cwd) + 1 +
#endif
        wcslen(folder) + 3) * sizeof(TCHAR));
    if (utility)
    {
        wcscpy(utility, util);
        wcscat(utility, _T("\""));                 //  ������� ���������, ����� ��������� ����� � ���������
#ifdef LINUX
        wcscat(utility, cwd);
        wcscat(utility, _T("/"));
#endif
        wcscat(utility, folder);
        wcscat(utility, _T("\""));

        if (-1 == _wsystem(utility))
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
    TCHAR *saved_command = command;         //  sohranenie
    if (CMD_KEY == get_query_command(0))    //  udalenie klyucha iz zaprosa
    {
        FILE *f;

        key = (TCHAR*)wcsrchr(command, '=');
        key++;
        //  Reading session key file.
        //  https://action.mindjet.com/task/14732139
        std::wstring fileName = GetTmpDirName();
        fileName += key;

        f = _wfopen(fileName.c_str(), _T("r"));
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

void MakeFolder(std::wstring name)
{
    //  ����� ��������� � 1251 (CP_ACP �������. ��, ��������� ����� ��. ����� �������� ���������).
    char dirName[MAX_LINE_LENGTH];
    WideCharToMultiByte(CP_ACP, 0, name.c_str(), -1, dirName, sizeof(dirName), NULL, NULL);
    //  ������� �������� � "������" ���������.
    _mkdir(dirName);
}
//-------------------------------------------------------------------------------------------------
//  https://action.mindjet.com/task/14817423
void prepare_query_buffer(int queryLen)
{
    int bufSize = (queryLen + 1) * sizeof(TCHAR);
    query = (TCHAR*)malloc(bufSize);
    //  Fill by terminating zeros.
    memset(query, 0, bufSize);
}
//-------------------------------------------------------------------------------------------------

#define MAX_DOMAIN 100
#ifdef USE_SCRIPT_NAME
int HandleQuery(TCHAR* encodedQuery, TCHAR* scriptName)
#else
int HandleQuery(TCHAR* encodedQuery)
#endif
{
    if (!encodedQuery || !wcslen(encodedQuery))
    {
        no_environment(_T("QUERY_STRING"));
        return 1;
    }

    //  https://action.mindjet.com/task/14817423
    prepare_query_buffer(_tcslen(encodedQuery));
    _tcscpy(query, encodedQuery);

    //  https://action.mindjet.com/task/14702859
    //  �������� ������ �� HTTPS.
    TCHAR http[] = _T("https://");
    int script_name_len = wcslen(http);         //  ������ ����� ���� � �������
                                                //  ������ ���������� ��������� ���������� ��� ������ �� ������. ����� ��� ����, 
                                                //  ����� ��������� ���. ������, ����� ������ �������� ����� SSH.
                                                //  evernote:///view/14501366/s132/44d4835c-cdac-40e1-acff-2fe610f865c8/44d4835c-cdac-40e1-acff-2fe610f865c8/
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
    TCHAR *server_name = _wgetenv(_T("SERVER_NAME"));
#endif

    if (!wcslen(server_name)) {
        no_environment(_T("SERVER_NAME")); return 1;
    }
#ifdef USE_SCRIPT_NAME
    if (!scriptName || !wcslen(scriptName)) {
        no_environment(_T("SCRIPT_NAME")); return 1;
    }
#endif
    if (!fatal_error)
    {
        full_script_name = 0;
        script_name_len += wcslen(server_name);
#ifdef USE_SCRIPT_NAME
        script_name_len += wcslen(scriptName);
#endif
        full_script_name = (TCHAR *)malloc((script_name_len + 1) * sizeof(TCHAR));
        if (full_script_name)
        {
            //  ��������� ��� ������� ��� �������� ������
            wcscpy(full_script_name, http);
            wcscat(full_script_name, server_name);
#ifdef USE_SCRIPT_NAME
            wcscat(full_script_name, scriptName);
#endif
            TCHAR www_sub[] = _T("/links/");                        //  www - ����� ��-��������� ��� ���-����, �������
                                                                    //  ��������� �� ���� �� �����
            img_path = (TCHAR *)malloc((wcslen(server_name) + wcslen(http) + wcslen(www_sub) + 1) * sizeof(TCHAR));
            //  ��������� ���� � ������� ��� �������� ������ �� ��������
            if (img_path)
            {
                wcscpy(img_path, http);
                wcscat(img_path, server_name);
                wcscat(img_path, www_sub);
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
    if (!fatal_error)
    {
        cwd = (TCHAR*)_wgetcwd(0, 0);
        if (cwd)
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
            switch (get_query_command(1))
            {
            default:
            case(CMD_LOG_IN_CONF): process_query(0);
                if (!fatal_error)do_log_in_conf();
                break;

            case(CMD_LOG_IN): process_query(1);
                if (!fatal_error) {
                    do_log_in();
                }
                break;

            case(CMD_CHANGE_FOLDER): process_query(0);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_change_folder();
                }
                break;

            case(CMD_EDIT): process_query(1);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_edit();
                }
                break;

            case(CMD_EDIT_CONF): process_query(0);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_edit_conf();
                }
                break;

            case(CMD_ADD): process_query(1);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_add();
                }
                break;

            case(CMD_ADD_CONF): process_query(0);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_add_conf();
                }
                break;

            case(CMD_DEL): process_query(1);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_del();
                }
                break;

            case(CMD_DEL_CONF): process_query(0);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_del_conf();
                }
                break;

            case(CMD_EDIT_FOLDER): process_query(1);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_edit_folder();
                }
                break;

            case(CMD_EDIT_FOLDER_CONF): process_query(0);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_edit_folder_conf();
                }
                break;

            case(CMD_ADD_FOLDER): process_query(1);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_add_folder();
                }
                break;

            case(CMD_ADD_FOLDER_CONF): process_query(0);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_add_folder_conf();
                }
                break;

            case(CMD_DEL_FOLDER): process_query(1);
                if (!fatal_error) {
                    get_query_command(0);   //  udalenie komandy iz zaprosa
                    get_key();
                    do_del_folder();
                }
                break;

            case(CMD_DEL_FOLDER_CONF): process_query(0);
                if (!fatal_error) {
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
    if (query)
    {
        free(query);                 //  posle process_query mozhno osvobozhdat'
        query = 0;
    }
    if (img_path) free(img_path);
    if (cwd) free(cwd);
    if (full_script_name) free(full_script_name);
    return 0;
}
