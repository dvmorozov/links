//-------------------------------------------------------------------------------------------------
//  ������ �������� �� ����� ������� ������, ��� � LINUX ������� 
//  ����� �� ���������� ��������� �������������� �������� ������
//-------------------------------------------------------------------------------------------------

#define LINUX     //  native
#define EXTENDED_URL_FILE

#include <stdio.h>
#include <stdlib.h>

#define TCHAR       char
#define _T(string)  string

#define _tfopen     fopen
#define _tmkdir     mkdir
#define _tgetenv    getenv
#define _tgetcwd    getcwd
#define _tsystem    system
#define _tchdir     chdir
#define _tcslen     strlen
#define _tcschr     strchr
#define _tcscat     strcat
#define _tcscpy     strcpy
#define _ftprintf   fprintf
#define _tprintf    printf
#define _tcsrchr    strrchr
#define _tcscmp     strcmp
#define _trename    rename
#define _tcsncmp    strncmp
#define _tremove    remove
#define _fgetts     fgets
#define _tmktemp    mktemp

#if defined _WINDOWS

#include <string.h>
#include <io.h>
#include <direct.h>
#include <time.h>

#endif

//  ���� ���������� � Ansi-����. � ���. VC, �� ��� �������� ���������
//  ���� ���������� � Unicode-����. � ���. VC, �� ������� ����� ��������� + ����. ����� ��������� �����
//  ���� ������� � cygwin ����. �� ����� �������� ���������, �� ����� ���������� �� �������� �
//  �����-�� ���� ���������

#define N                   5
#define MAX_LINE_LENGTH     1000                        //  ����������� ���������� ����� ������ �����, ����������� ����� ������ � �����

#define EXT_LEN             4
TCHAR ext[EXT_LEN + 1] = _T(".url");

TCHAR str_url[] = _T("URL=");

#ifdef EXTENDED_URL_FILE
TCHAR str_name[] = _T("Name=");

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
//  ������� �������
//  ����� Cancel � �� �.�. ������ ��, ��� � ������� cancel � ok
TCHAR htm_folder_del_conf[] = _T("<html>\
    <head>\
        <title>�������� �����</title>\
    </head>\
    <body>\
        <form method=get action=\"%s\">\
            <p>�� ������������� ������ ������� ����� <font color=\"red\">%s</font> � ��� � ����������?</P>\
            <p>\
                <input id=\"Reset1\" type=\"submit\" value=\"Cancel\" name=\"%s;del_folder\">\
                <input id=\"Submit1\" type=\"submit\" value=\"Ok\" name=\"%s;del_folder=%s;del_folder\">\
            </p>\
        </form>\
    </body>\
</html>\
");
TCHAR htm_del_conf[] = _T("<html>\
    <head>\
        <title>�������� ������</title>\
    </head>\
    <body>\
        <form method=get action=\"%s\">\
            <p>�� ������������� ������ ������� ������ <font color=\"red\">%s</font>, ����������� �� <font color=\"red\">%s</font> ?</P>\
            <p>\
                <input id=\"Reset1\" type=\"submit\" value=\"Cancel\" name=\"%s;del=%s;del\">\
                <input id=\"Submit1\" type=\"submit\" value=\"Ok\" name=\"%s;del=%s;del\">\
            </p>\
        </form>\
    </body>\
</html>\
");
TCHAR htm_edit[] = _T("<html>\
    <head>\
        <title>�������������� ������</title>\
    </head>\
    <body>\
        <form method=get action=\"%s\">\
            <p>\
                ��� ������:<br>\
                <input type=\"text\" size=50 value=\"%s\" name=\"%s;edit\"><br>\
                ������ ��������� �� %s\
            </p>\
            <p>\
                <input type=\"submit\" value=\"Cancel\" name=\";edit\">\
                <input type=\"submit\" value=\"Ok\" name=\";edit=%s;edit\">\
            </p>\
        </form>\
    </body>\
</html>\
");
TCHAR htm_edit_folder[] = _T("<html>\
    <head>\
        <title>�������������� �����</title>\
    </head>\
    <body>\
        <form method=get action=\"%s\">\
            <p>\
                ��� �����:<br>\
                <input type=\"text\" size=50 value=\"%s\" name=\"%s;edit_folder\"><br>\
            </p>\
            <p>\
                <input type=\"submit\" value=\"Cancel\" name=\";edit_folder\">\
                <input type=\"submit\" value=\"Ok\" name=\";edit_folder=%s;edit_folder\">\
            </p>\
        </form>\
    </body>\
</html>\
");
TCHAR htm_add[] = _T("<html>\
    <head>\
        <title>�������� ������</title>\
    </head>\
    <body>\
        <form method=get action=\"%s\">\
            <p>\
                ��� ������:<br>\
                <input type=\"text\" size=50 value=\"\" name=\"%s;add\"><br>\
                ������ ��������� ��:<br>\
                <input type=\"text\" size=50 value=\"\" name=\";add\"><br>\
            </p>\
            <p>\
                <input type=\"submit\" value=\"Cancel\" name=\";add\">\
                <input type=\"submit\" value=\"Ok\" name=\";add\">\
            </p>\
        </form>\
    </body>\
</html>\
");
TCHAR htm_add_folder[] = _T("<html>\
    <head>\
        <title>�������� �����</title>\
    </head>\
    <body>\
        <form method=get action=\"%s\">\
            <p>\
                ��� �����:<br>\
                <input type=\"text\" size=50 value=\"\" name=\"%s;add_folder\"><br>\
            </p>\
            <p>\
                <input type=\"submit\" value=\"Cancel\" name=\";add_folder\">\
                <input type=\"submit\" value=\"Ok\" name=\";add_folder\">\
            </p>\
        </form>\
    </body>\
</html>\
");

#if !defined LINUX
#define DIR_LENGTH      29
//TCHAR dir[] = _T("dir /AD /B /ON >> ");                           //  ����� ��� Windows
TCHAR dir[DIR_LENGTH + 1] = _T("cygdir.exe -a -1 -X -F -N >> ");    //  ����� ��� Cygwin
#define TMPDIR_LENGTH   0
TCHAR tmpdir[TMPDIR_LENGTH + 1] = _T("");
#else
#define DIR_LENGTH      22
TCHAR dir[DIR_LENGTH + 1] = _T("dir -a -1 -X -F -N >> ");           //  ����� ��� Linux
//#define TMPDIR_LENGTH   9
//TCHAR tmpdir[TMPDIR_LENGTH + 1] = _T("/var/tmp/");                //  ����� ��������� ��������� ������ � ����� � �������,
                                                                    //  ��� ������������ ��������� ����������
#define TMPDIR_LENGTH   0
TCHAR tmpdir[TMPDIR_LENGTH + 1] = _T("");
#endif
//  ������������ ������� ��������� ������, ������ ���
//  C-������� _rmdir �������, ����� ����� ���� �����
#if !defined LINUX
//TCHAR rm[] = _T("rmdir /S /Q ");                      //  ����� ��� Windows
TCHAR rm[] = _T("rm -r -f ");                           //  ����� ��� Cygwin
#else
TCHAR rm[] = _T("rm -r -f -d ");                        //  ����� ��� Linux
//TCHAR rmdir[] = _T("rmdir ");
#endif

TCHAR http[]                    = _T("http://");
TCHAR www[]                     = _T("../www/");        //  ���� ��� �������� � ����� � ���������;
                                                        //  ������ ��������� ����������� / �� �����
TCHAR head[]                    = _T("��������� ������");
TCHAR head_error[]              = _T("������");
TCHAR home[]                    = _T("� ������");
TCHAR up[]                      = _T("�����");
TCHAR hint_folder[]             = _T("�����");
TCHAR hint_link[]               = _T("������");
TCHAR hint_error[]              = _T("������");
TCHAR hint_error_colon[]        = _T("������: ");
TCHAR hint_delete[]             = _T("��������");
TCHAR hint_edit[]               = _T("��������������");
TCHAR hint_add_link[]           = _T("������� ������");
TCHAR hint_add_folder[]         = _T("������� �����");
TCHAR err_change_folder[]       = _T("���������� ������� � �����: ");
TCHAR err_out_of_memory[]       = _T("������������ ������ ��� ����������");
TCHAR err_invalid_query[]       = _T("������������ ������: ");
TCHAR err_no_environment[]      = _T("����������� ��������� ���������: ");
TCHAR err_sys_utility[]         = _T("����� ������� %s ���������� � �������");
TCHAR err_imposs_read_url[]     = _T("���������� ��������� URL �� �����!");     //  ������������ ��������� � hint_error_colon

//  ������� ��� ���������� ���������� (����������� � url)
//  !!! ����� ������ ���������� ������ ��� ����� � ������, ��������� ����������
//  �� ���������� ����� url, � ������ ����� ����� ���������� ����� � ���� � 
//  ���������� ������ !!!
TCHAR cmd_edit[]                = _T(";edit=");
TCHAR cmd_edit_conf[]           = _T(";edit_conf=");
TCHAR cmd_add[]                 = _T(";add=");
TCHAR cmd_add_conf[]            = _T(";add_conf=");
TCHAR cmd_del[]                 = _T(";del=");
TCHAR cmd_del_conf[]            = _T(";del_conf=");
TCHAR cmd_edit_folder[]         = _T(";edit_folder=");
TCHAR cmd_edit_folder_conf[]    = _T(";edit_folder_conf=");
TCHAR cmd_add_folder[]          = _T(";add_folder=");
TCHAR cmd_add_folder_conf[]     = _T(";add_folder_conf=");
TCHAR cmd_del_folder[]          = _T(";del_folder=");
TCHAR cmd_del_folder_conf[]     = _T(";del_folder_conf=");

TCHAR ok[]                      = _T("Ok");
TCHAR cancel[]                  = _T("Cancel");

TCHAR folder_names[]            = _T("�����");
TCHAR link_names[]              = _T("������");

//  ����� ������ ��������
TCHAR pict_to_start_page[]      = _T("to_start_page.bmp");       
                                                        //  ������� � ������ �������� ������ ��������
TCHAR pict_to_upper_folder[]    = _T("to_upper_folder.bmp");       
                                                        //  ������� �� �������� �� ���� ������� �����
TCHAR pict_folder[]             = _T("folder.bmp");
TCHAR pict_delete_folder[]      = _T("delete_folder.bmp");
TCHAR pict_edit_folder[]        = _T("edit_folder.bmp");
TCHAR pict_delete_link[]        = _T("delete_link.bmp");
TCHAR pict_edit_link[]          = _T("edit_link.bmp");
TCHAR pict_add_folder[]         = _T("add_folder.bmp");
TCHAR pict_add_link[]           = _T("add_link.bmp");
TCHAR pict_link[]               = _T("link.bmp");
TCHAR pict_error[]              = _T("error.bmp");

//  ���� ������
#define CMD_CHANGE_FOLDER       0
#define CMD_EDIT                1
#define CMD_ADD                 2
#define CMD_DEL                 3                       //  �������� ������
#define CMD_EDIT_FOLDER         4
#define CMD_ADD_FOLDER          5
#define CMD_DEL_FOLDER          6                       //  �������� �����
#define CMD_DEL_CONF            7                       //  ������ �� �������� ������
#define CMD_DEL_FOLDER_CONF     8                       //  ������ �� �������� �����
#define CMD_EDIT_FOLDER_CONF    9                       //  ������ �� ��������� ����� �����
#define CMD_EDIT_CONF           10                      //  ������ �� ��������� ������
#define CMD_ADD_CONF            11
#define CMD_ADD_FOLDER_CONF     12
#define CMD_UNKNOWN             0xff

//  !!! ��������� ����� ��� ��������� ���� ������� - ������ �.�. ������ ������� !!!
TCHAR *query = 0;           //  �� ����������� ��������� 0
TCHAR *full_script_name = 0;//  �� ����������� ��������� 0
TCHAR *img_path = 0;        //  !!! �������� ����������� ������ / !!!
TCHAR *cwd = 0;             //  ���� � �������� ��������
                            //  ����������� ������ / �� ����������
                            //  �� ����������� ��������� 0
TCHAR *command = 0;         //  ��-�� �� ���������� � ������� ������� (������������� ������ � ������������� �������)

//  ���� ������
#define E_OUTOFMEMORY       1
#define E_INVALID_QUERY     2
#define E_CHANGE_FOLDER     3
#define E_NO_ENVIRONMENT    4
#define E_SYS_UTILITY       5
int error = 0;
int fatal_error = 0;
int first_folder = 1;
int first_link = 1;
//-------------------------------------------------------------------------------------------------

void print_info();
void change_folder();
void print_folders();
void do_change_folder();
void insert_add_buttons();
void remove_folder(TCHAR *folder);

TCHAR *read_str_cwd(TCHAR *url_file_name/*���� � ����� ���. cwd*/, TCHAR *str_name);

void invalid_query();
void out_of_memory();
void no_environment(TCHAR *env_str);

int get_query_command(unsigned char does_not_processed);
int hex_char_to_int(TCHAR c);

void print_html_head(TCHAR *title);
void print_html_tail();
//-------------------------------------------------------------------------------------------------
//  ����� % � ������� �������. ��������
//  '+' ���������� �� �������
void process_query(unsigned char delete_spaces)
{
    int query_len = _tcslen(query);
    TCHAR *temp = malloc((query_len + 1) * sizeof(TCHAR));
    if(temp)
    {
        TCHAR *ptr, *prev_ptr;
        ptr = query;
        prev_ptr = ptr;
        temp[0] = 0;

        //  ����� ������� ���������� ������ � ����� �� ������� ������,
        //  �� �� �������� ������� �� '+', � '+' �� % � ����� 
        //  (=> ������� ����� ������. '+'  � ' ', � ����� % � �������);
        //  ����� ������� ��������� �� ������ �� �������� ������ ��
        //  % � �����, � '+' �������� ��� ����
        //  (=> ����� ������ ������. %  � �������)

        //  '+' ���������� �� �������
        if(delete_spaces)
        {
            while(ptr = (TCHAR*)_tcschr(ptr, '+'))
            {
                *ptr = ' ';
                ptr++;
            }
        }

        ptr = query;
        //  �������������� ��������, �������� � ���. �����
        while(ptr = (TCHAR*)_tcschr(ptr, '%'))
        {
            if((int)(ptr - query) <= query_len - 3)
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
        _tcscat(temp, prev_ptr);            //  ���������� ������� ������

        query = temp;                       //  ����. ��-�� �� �������������,
                                            //  ��������� ������� ����� getenv
    }
    else
    {
        out_of_memory();
        query = 0;                          //  ����� �� ���� ������������� ���. ���������
    }
}
//-------------------------------------------------------------------------------------------------
//  ����� ������ �� ��������
void create_url_file(
    TCHAR *name,        //  ��� ����� ��� ����
    TCHAR *url)
{
#ifdef EXTENDED_URL_FILE
    //  � �������� ����� ����� ���. ��������� �������� ��������
    //  ��� ������������� ��������� �������� ���������� � ���.
    //  tmktemp ���������, ��� ��� �������� ����� �����������
    TCHAR temp[9 + EXT_LEN];

    //srand((unsigned)time(NULL));
    sprintf(temp, "%08x", /*(int)rand()*/(int)time(0));
    _tcscat(temp, ext);

    {
    FILE *f = _tfopen(temp, _T("w"));
    if(f)
    {
        _ftprintf(f, url_file_template, url, name);
        fclose(f);
    }
    }
#else
    TCHAR *full_file_name = malloc((_tcslen(name) + _tcslen(ext) + 1) * sizeof(TCHAR));
    if(full_file_name)
    {
        _tcscpy(full_file_name, name);
        _tcscat(full_file_name, ext);
        {
        FILE *f = _tfopen(full_file_name, _T("w"));
        if(f)
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

void print_html_head(TCHAR *title)
{
    _tprintf(_T("%s"), _T("<html>"));
    _tprintf(_T("<head><title> %s </title></head>\n"), title);
    _tprintf(_T("%s"), _T("<body><br>\n"));
}
//-------------------------------------------------------------------------------------------------

void print_html_tail()
{
    _tprintf(_T("%s"), _T("</body></html>\n"));
}
//-------------------------------------------------------------------------------------------------
//  ������� ���������� ������� �����
void print_folder_content()
{
    print_html_head(head);
    print_folders();
    insert_add_buttons();
    print_info();
    print_html_tail();
}
//-------------------------------------------------------------------------------------------------

void do_edit_conf()
{
    //  command ����� �� �.�. = 0
    //  link �������� ��� �����
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if(link)
    {
        link++;
        change_folder();
        if(!fatal_error)
        {
            TCHAR *url = read_str_cwd(link, str_url);
            //  name �������� �������� ��� ����������
            TCHAR *name;
#ifdef EXTENDED_URL_FILE
            name = read_str_cwd(link, str_name);
            if(!name)
#endif
            {// �������� � ����� �� ������ ��� �� �������������� -
             // ������� �������� �� ����� ����� ��� ����������
                TCHAR *dot;
                
                name = link;
                dot = (TCHAR*)_tcsrchr(link, '.');
                if(dot)
                {
                    *dot = 0;
                    name = malloc((_tcslen(name) + 1) * sizeof(TCHAR));
                    if(name)_tcscpy(name, link);
                    *dot = '.';
                }
            }
            if(name)
            {
                _tprintf(htm_edit, full_script_name, name, query, url, link);
                if(name != link)free(name);
            }
            else out_of_memory();
            if(url)free(url);
        }
    }
}
//-------------------------------------------------------------------------------------------------
//  �������������� ������
void do_edit()
{
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if(link)
    {
        link++;
        if(!_tcscmp(link, ok))
        {
            if(get_query_command(0) == CMD_EDIT)
            {// ������ ����� = ������ ��� ������
                TCHAR *old_name = (TCHAR*)_tcschr(command, '=');
                if(old_name)
                {
                    old_name++;
                    if(get_query_command(0) == CMD_EDIT)
                    {// ������ ����� = ����� ��� ������ (��� ����������)
                        TCHAR *new_name = (TCHAR*)_tcschr(command, '=');
                        if(new_name)
                        {
#ifdef EXTENDED_URL_FILE
                            TCHAR *and;
                            new_name++;
                            if(and = (TCHAR*)_tcsrchr(new_name, '&'))*and = 0;
#else
                            TCHAR *and;
                            TCHAR *dot;
                            TCHAR *new_name_ext;
                            new_name++;
                            new_name_ext = new_name;
                            //  �� ������ ����� ����� ������� ������ &
                            if(and = (TCHAR*)_tcsrchr(new_name, '&'))*and = 0;
                            //  ����� ��������� ����� ��� �� ������ �����������
                            if(dot = (TCHAR*)_tcsrchr(old_name, '.'))
                            {// ���� ����������
                                new_name_ext = malloc((_tcslen(new_name) + _tcslen(dot) + 1) * sizeof(TCHAR));
                                if(new_name_ext)
                                {
                                    _tcscpy(new_name_ext, new_name);
                                    _tcscat(new_name_ext, dot);
                                }
                                else out_of_memory();
                            }
#endif
                            if(!fatal_error)
                            {// ������� � ����� � �������������� �����
                                change_folder();
                                if(!fatal_error)
                                {
#ifdef EXTENDED_URL_FILE
                                    TCHAR *url = read_str_cwd(old_name, str_url);
                                    //  ������ ����� (!!! ��������� ���������� �� ����� �������� !!!)
                                    if(url)
                                    {
                                        create_url_file(new_name, url);
                                        free(url);
                                        _tremove(old_name);
                                    }
                                    else error = E_OUTOFMEMORY;
#else
                                    //  ��������������
                                    _trename(old_name, new_name_ext);
#endif
                                    print_folder_content();
                                }
#ifndef EXTENDED_URL_FILE
                                if(new_name_ext != new_name)free(new_name_ext);
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
        if(!_tcscmp(link, cancel))
        {// � ���� ���������� ������ ��� ������ ���������
            if(get_query_command(0) == CMD_EDIT)
            {// ������ ����� = ����� ��� ������
                change_folder();
                if(!fatal_error)print_folder_content();
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
    _tprintf(htm_add, full_script_name, query);
}
//-------------------------------------------------------------------------------------------------
//  ���������� ������
void do_add()
{
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if(link)
    {
        link++;
        if(!_tcscmp(link, ok))
        {
            if(get_query_command(0) == CMD_ADD)
            {// ������ ����� = url ����� ������
                TCHAR *url = (TCHAR*)_tcschr(command, '=');
                if(url)
                {
                    TCHAR *and;
                    url++;
                    //  �� ������ url ����� ������� ������ &
                    if(and = (TCHAR*)_tcsrchr(url, '&'))*and = 0;

                    if(get_query_command(0) == CMD_ADD)
                    {// ������ ����� = ��� ����� ������ (��� ����������)
                        TCHAR *name = (TCHAR*)_tcschr(command, '=');
                        if(name)
                        {
                            name++;
                            //  �� ����� ����� ������� ������ &
                            if(and = (TCHAR*)_tcsrchr(name, '&'))*and = 0;
                            // ������� � ����� � �������� ����� .url
                            change_folder();
                            if(!fatal_error)
                            {
                                create_url_file(name, url);
                                if(!fatal_error)print_folder_content();
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
        if(!_tcscmp(link, cancel))
        {
            if(get_query_command(0) == CMD_ADD)
            {// ������ ����� = url ����� ������
                if(get_query_command(0) == CMD_ADD)
                {// ������ ����� = ��� ����� ������
                    change_folder();
                    if(!fatal_error)print_folder_content();
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
    TCHAR *result = (TCHAR*)_tcschr(command, '=');
    if(result)
    {
        if(get_query_command(0) == CMD_DEL)
        {// ������ ����� = ��� �����
            if(!_tcsncmp(result + 1, ok, _tcslen(ok)))
            {
                change_folder();
                if(!fatal_error)
                {
                    result = (TCHAR*)_tcschr(command, '=');
                    if(result)
                    {
                        result++;
                        // �������� ������ - � result - ��� �����
                        _tremove(result);
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
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if(link)
    {
        link++;
        change_folder();
        if(!fatal_error)
        {
            TCHAR *url = read_str_cwd(link, str_url);
            TCHAR *name;
#ifdef EXTENDED_URL_FILE
            name = read_str_cwd(link, str_name);
            if(!name)name = link;
#else
            name = link;
#endif
            _tprintf(htm_del_conf, full_script_name, name, url, query, link, query, link);
            if(url)free(url);
            if(name && (name != link))free(name);
        }
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_edit_folder_conf()
{
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if(link)
    {
        link++;
        _tprintf(htm_edit_folder, full_script_name, link, query, link);
    }
}
//-------------------------------------------------------------------------------------------------
void do_edit_folder()
{
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if(link)
    {
        link++;
        if(!_tcscmp(link, ok))
        {
            if(get_query_command(0) == CMD_EDIT_FOLDER)
            {// ������ ����� = ������ ��� �����
                TCHAR *old_name = (TCHAR*)_tcschr(command, '=');
                if(old_name)
                {
                    old_name++;
                    if(get_query_command(0) == CMD_EDIT_FOLDER)
                    {// ������ ����� = ����� ��� �����
                        TCHAR *new_name = (TCHAR*)_tcschr(command, '=');
                        if(new_name)
                        {
                            TCHAR *and;
                            new_name++;
                            //  �� ������ ����� ����� ������� ������ &
                            if(and = (TCHAR*)_tcsrchr(new_name, '&'))*and = 0;

                            // ������� � ����� � �������������� �����
                            change_folder();
                            if(!fatal_error)
                            {// ��������������
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
        if(!_tcscmp(link, cancel))
        {// � ���� ���������� ������ ��� ������ ���������
            if(get_query_command(0) == CMD_EDIT_FOLDER)
            {// ������ ����� = ����� ��� ������
                change_folder();
                if(!fatal_error)print_folder_content();
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
    if(!fatal_error)
    {
        _tprintf(htm_add_folder, full_script_name, query);
    }
}
//-------------------------------------------------------------------------------------------------
void do_add_folder()
{
    //  command ����� �� �.�. = 0
    TCHAR *link = (TCHAR*)_tcschr(command, '=');
    if(link)
    {
        link++;
        if(!_tcscmp(link, ok))
        {
            if(get_query_command(0) == CMD_ADD_FOLDER)
            {// ������ ����� = ��� ����� �����
                TCHAR *name = (TCHAR*)_tcschr(command, '=');
                if(name)
                {
                    TCHAR *and;
                    name++;
                    //  �� ����� ����� ������� ������ &
                    if(and = (TCHAR*)_tcsrchr(name, '&'))*and = 0;
                    // ������� � ����� � �������� ����� .url
                    change_folder();
                    if(!fatal_error)
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
        if(!_tcscmp(link, cancel))
        {
            if(get_query_command(0) == CMD_ADD_FOLDER)
            {// ������ ����� = ��� ����� ������
                change_folder();
                if(!fatal_error)print_folder_content();
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
    TCHAR *result = (TCHAR*)_tcschr(command, '=');
    if(result)
    {
        if(!_tcsncmp(result + 1, ok, _tcslen(ok)))
        {
            if(get_query_command(0) == CMD_DEL_FOLDER)
            {// ������ ����� = ��� �����
                change_folder();
                if(!fatal_error)
                {
                    result = (TCHAR*)_tcschr(command, '=');
                    if(result)
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
    TCHAR *folder = (TCHAR*)_tcschr(command, '=');
    if(folder)
    {
        folder++;
        _tprintf(htm_folder_del_conf, full_script_name, folder, query, query, folder);
    }
    else invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_change_folder()
{
    change_folder();
    if(!fatal_error)
        print_folder_content();
}
//-------------------------------------------------------------------------------------------------

int recognize_command(TCHAR *command/*������� ��� ;*/)
{
    int result;
    if(!_tcsncmp(command, cmd_edit + 1, _tcslen(cmd_edit) - 1)){result = CMD_EDIT;}
    else
    if(!_tcsncmp(command, cmd_edit_conf + 1, _tcslen(cmd_edit_conf) - 1)){result = CMD_EDIT_CONF;}
    else
    if(!_tcsncmp(command, cmd_add + 1, _tcslen(cmd_add) - 1)){result = CMD_ADD;}
    else
    if(!_tcsncmp(command, cmd_add_conf + 1, _tcslen(cmd_add_conf) - 1)){result = CMD_ADD_CONF;}
    else
    if(!_tcsncmp(command, cmd_del_conf + 1, _tcslen(cmd_del_conf) - 1)){result = CMD_DEL_CONF;}
    else
    if(!_tcsncmp(command, cmd_del + 1, _tcslen(cmd_del) - 1)){result = CMD_DEL;}
    else
    if(!_tcsncmp(command, cmd_edit_folder + 1, _tcslen(cmd_edit_folder) - 1)){result = CMD_EDIT_FOLDER;}
    else
    if(!_tcsncmp(command, cmd_edit_folder_conf + 1, _tcslen(cmd_edit_folder_conf) - 1)){result = CMD_EDIT_FOLDER_CONF;}
    else
    if(!_tcsncmp(command, cmd_add_folder + 1, _tcslen(cmd_add_folder) - 1)){result = CMD_ADD_FOLDER;}
    else
    if(!_tcsncmp(command, cmd_add_folder_conf + 1, _tcslen(cmd_add_folder_conf) - 1)){result = CMD_ADD_FOLDER_CONF;}
    else
    if(!_tcsncmp(command, cmd_del_folder_conf + 1, _tcslen(cmd_del_folder_conf) - 1)){result = CMD_DEL_FOLDER_CONF;}
    else
    if(!_tcsncmp(command, cmd_del_folder + 1, _tcslen(cmd_del_folder) - 1)){result = CMD_DEL_FOLDER;}
    else {invalid_query(); result = CMD_UNKNOWN;}
    return result;
}
//-------------------------------------------------------------------------------------------------
//  !!! ������������ ������� ��������� �� ������ ������� !!!
int get_query_command(unsigned char does_not_processed)
{
    if(does_not_processed)
    {
        command = query + _tcslen(query) - 3;
        for(;command >= query; command--)
        {
            if((command[0] == '%') && (command[1] == '3') && 
                ((command[2] == 'B') || (command[2] == 'b')))
            {
                command += 3;
                //  ���� ���� %, �� ������ ������ �� ���������
                return recognize_command(command);
            }
        }
    }

    command = (TCHAR*)_tcsrchr(query, ';');
    if(!command)return CMD_CHANGE_FOLDER;
    else
    {
        if(!does_not_processed)*command = 0;
        command++;
        return recognize_command(command);
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

void print_info()
{
    TCHAR *par[N] = {
        _T("SERVER_SOFTWARE"), _T("SERVER_NAME"), _T("SCRIPT_NAME"), 
        _T("REQUEST_METHOD"), _T("QUERY_STRING") 
        };
    int x;
    _tprintf(_T("%s"), _T("<br><hr><br>\n"));
    for(x = 0; x < N; x++)
    {
        _tprintf(_T("%s = %s<br>\n"), par[x], _tgetenv(par[x]));
    }
    _tprintf(_T("HOME = %s<br>\n"), cwd);
}
//-------------------------------------------------------------------------------------------------

int hex_char_to_int(TCHAR c)
{
    switch(c)
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
    print_html_head(head_error);
    begin_error_box();
    printf("%s%s<BR>\n", err_no_environment, env_str);
    end_error_box();
    error = E_NO_ENVIRONMENT;
    fatal_error = 1;
    print_html_tail();
}
//-------------------------------------------------------------------------------------------------

void invalid_query()
{
    print_html_head(head_error);
    begin_error_box();
    printf("%s%s<BR>\n", err_invalid_query, query);
    end_error_box();
    error = E_INVALID_QUERY;
    fatal_error = 1;
    print_html_tail();
}
//-------------------------------------------------------------------------------------------------

void out_of_memory()
{
    print_html_head(head_error);
    begin_error_box();
    printf("%s<BR>\n", err_out_of_memory);
    end_error_box();
    error = E_OUTOFMEMORY;
    fatal_error = 1;
    print_html_tail();
}
//-------------------------------------------------------------------------------------------------
//  ������� � ��������� ����� (�� ��������� � �������� �������� �����)
void change_folder()
{
    //  ��������� ������ ��� ���������� ����� �����
    int query_len = _tcslen(query);
    TCHAR *temp = malloc((query_len + _tcslen(www) + 1) * sizeof(TCHAR));

    if(temp)
    {
        _tcscpy(temp, www);
        _tcscat(temp, query);
        //  ������� � ����������� �����
        if(-1 == _tchdir(temp))
        {
            print_html_head(head_error);
            begin_error_box();
            printf("%s%s<BR>\n", err_change_folder, temp);
            fatal_error = 1;
            end_error_box();
            error = E_CHANGE_FOLDER;
            print_html_tail();
        }

        free(temp);

        if(cwd)free(cwd);
        cwd = (TCHAR*)_tgetcwd(0, 0);
        if(!cwd)
            out_of_memory();
    }
    else 
        out_of_memory();
}
//-------------------------------------------------------------------------------------------------

TCHAR *read_str(TCHAR *url_file_name/*������ ���� � �����*/, TCHAR *str_name)
{
#ifndef _WINDOWS
    {
    unsigned int i;
    for(i = 0; i < _tcslen(url_file_name); i++)
    {
        if(url_file_name[i] == '\\')url_file_name[i] = '/';
    }
    }
#endif

    {
    FILE *url_file = _tfopen(url_file_name, _T("r"));
    
    if(url_file)
    {
        TCHAR *url = 0;
        TCHAR lineptr[MAX_LINE_LENGTH];
        //  _fgetts ��������� ������ 0-�
        while(_fgetts (lineptr, MAX_LINE_LENGTH, url_file))
        {
            //  ������� ����������� ������ �� ����������� �������� ����� ������
            int str_name_len = _tcslen(str_name);
            int i;
            for(i = _tcslen(lineptr) - 1; i >= 0; i--)
                if((lineptr[i] == '\x0a') || (lineptr[i] == '\x0d'))lineptr[i] = 0;

            if(!_tcsncmp(lineptr, str_name, str_name_len))
            {
                url = malloc((_tcslen(lineptr + str_name_len) + 1) * sizeof(TCHAR));
                if(url)
                {
                    _tcscpy(url, lineptr + str_name_len);
                }
                break;
            }
        }
        fclose(url_file);
        return url;
    }
    }
    return 0;
}
//-------------------------------------------------------------------------------------------------

TCHAR *read_str_cwd(TCHAR *url_file_name/*���� � ����� ���. cwd*/, TCHAR *str_name)
{
    TCHAR *path = malloc((_tcslen(cwd)      //  ���� � ���. �������� ��� ������. �����������
        + _tcslen(url_file_name)            //  ��� ����� � �����������
        + 2                                 //  ����������� � 0
        ) * sizeof(TCHAR));
    if(path)
    {
        TCHAR *url;
        _tcscpy(path, cwd);
        _tcscat(path, _T("\\"));
        _tcscat(path, url_file_name);

        url = read_str(path, str_name);
        free(path);
        return url;
    }
    else return 0;
}
//-------------------------------------------------------------------------------------------------

TCHAR *get_image_path(TCHAR *image_file)
{
    TCHAR *result;
    if(!img_path)return 0;
    result = (TCHAR*)malloc((_tcslen(image_file) + _tcslen(img_path) + 1) * sizeof(TCHAR));
    if(result)
    {
        _tcscpy(result, img_path);
        _tcscat(result, image_file);
    }
    return result;
}
//-------------------------------------------------------------------------------------------------

void insert_image(TCHAR *image_file, TCHAR *script_name, TCHAR *url, int size, TCHAR *alt)
{
    TCHAR *image_path = get_image_path(image_file);
    if(image_path)
    {
        if(script_name)
        {
            if(url)
                _tprintf(_T("\
                            <a href=\"%s?%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name, url, image_path, alt, size, size);
            else
                //  ���������� ������ �� ������� ������; script_name �������� ������ � �������
                _tprintf(_T("\
                            <a href=\"%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name, image_path, alt, size, size);
        }
        else
        {// ����������� ������ ��� ������
            if(url) //  � url ���������� ���������� � ������, ����������� ����� alt
                _tprintf(_T("\
                            <img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s%s\" width=\"%i\" height=\"%i\"></a>"), image_path, alt, url, size, size);
            else
                _tprintf(_T("\
                            <img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), image_path, alt, size, size);
        }
        free(image_path);
    }
}
//-------------------------------------------------------------------------------------------------

void insert_command_image(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, TCHAR* hint)
{
    TCHAR *command = malloc((_tcslen(cmd) + _tcslen(dir) + _tcslen(file) + 1) * sizeof(TCHAR));
    if(command)
    {
        _tcscpy(command, dir);
        _tcscat(command, cmd);
        _tcscat(command, file);
        insert_image(image_file, full_script_name, command, 16, hint);
        free(command);
    } 
    else error = E_OUTOFMEMORY;
}
//-------------------------------------------------------------------------------------------------

void exec_utility(TCHAR *folder, TCHAR *util)
{
    TCHAR *utility = malloc((_tcslen(util) + 
#ifdef LINUX
        _tcslen(cwd) + 1 +
#endif
        _tcslen(folder) + 3) * sizeof(TCHAR));
    if(utility)
    {
        _tcscpy(utility, util);
        _tcscat(utility, _T("\""));                 //  ������� ���������, ����� ��������� ����� � ���������
#ifdef LINUX
        _tcscat(utility, cwd);
        _tcscat(utility, _T("/"));
#endif
        _tcscat(utility, folder);
        _tcscat(utility, _T("\""));

        if(-1 == _tsystem(utility))
        {
            error = E_SYS_UTILITY;
            //  ������ �� ��������
        }
        free(utility);
    }
    else
    {
        error = E_OUTOFMEMORY;
        //  ������ �� ��������
    }
}
//-------------------------------------------------------------------------------------------------

void remove_folder(TCHAR *folder)
{
    exec_utility(folder, rm);
}
//-------------------------------------------------------------------------------------------------
//  ������� �����������
void print_row_tag()
{
    if(first_folder)
    {
        if(!first_link)
        {
            //  ��������� ��������� ������� � ������ ������� �������
            _tprintf(_T("%s"), _T("\n\
            </table>\n\
        </td>")); 
        }
        //  ��������� ������ ������� �������, ������� ��������� �������,
        //  ��������� ������ � ������ ��������� �������
        _tprintf(_T("%s"), _T("\n\
        <td valign=\"top\">\n\
            <table border=\"0\">\n\
                <tr>\n\
                    <td>\n"));
        first_folder = 0;
        first_link = 1;
    }
    else
    {
        _tprintf(_T("%s"), _T("\n\
                <tr>\n\
                    <td>\n"));
    }
}
//-------------------------------------------------------------------------------------------------
//  ����������� ������ � ������ ���������� �������
void close_inner_table_row()
{
    _tprintf(_T("%s"), _T("\n\
                    </td>\n\
                </tr>\n"));
}
//-------------------------------------------------------------------------------------------------
//  ��� ������ ��������� �������� ��� �������, �������
//  ����� ������� ������ ���������, ��������� ��� ��������
void print_folders()
{
    //  ��������� ����� ���������� �����
    //  !!! ����� ��������� ������ �.�. ����������� �� ��� ������, �����
    //  ����� ������������ �������� ��������� ����������� ������ ��������� !!!
    //  ��������� �������� ���������
    TCHAR temp[11 + TMPDIR_LENGTH];
    TCHAR utility_name[DIR_LENGTH + TMPDIR_LENGTH + 11];
    
    _tcscpy(temp, tmpdir);
    _tcscat(temp, _T("tempXXXXXX"));
    
    _tcscpy(utility_name, dir);
    _tcscat(utility_name, _tmktemp(temp));

    if(-1 == _tsystem(utility_name))
    {
        error = E_SYS_UTILITY;
        //  ������ �� �������� - ������ �� ���������
    }
    else
    {// ������ � ������ ���������� ������ ������
        FILE *f = _tfopen (temp, _T("r"));

        if(f)
        {
            size_t n;
            //TCHAR *lineptr = 0;
            //  getline ��� � cygwin
            //while(-1 != getline (&lineptr, &n, f))
            TCHAR lineptr[MAX_LINE_LENGTH];

            //  ��������� ������� �������
            printf(_T("\n\
<table border=\"1\">\n\
    <tr>\n\
        <th>%s\n\
        </th>\n\
        <th>%s [%s]\n\
        </th>\n\
    </tr>\n\
    <tr>\n"), folder_names, link_names, query);

            while(_fgetts (lineptr, MAX_LINE_LENGTH, f))
            {
                n = _tcslen(lineptr);
                if(n)
                {
                    //  ��������� ������ ����� url-������ ��� ����� �����
                    unsigned char folder = 0;

                    while((lineptr[n - 1] == '\x0a') || (lineptr[n - 1] == '\x0d')
#ifdef LINUX
			 || (lineptr[n - 1] == '*')
#endif		    
			)n--;
                    lineptr[n] = 0;         //  ������� �� ������ ��� ����. ����. => 
                                            //  ������ �������� ��� �����
                    if(lineptr[n - 1] != '/')
                    {
                        //  �������� ����������
                        if(!(
                            ((lineptr[n - 1] == 'l') || (lineptr[n - 1] == 'L')) && 
                            ((lineptr[n - 2] == 'r') || (lineptr[n - 2] == 'R')) && 
                            ((lineptr[n - 3] == 'u') || (lineptr[n - 3] == 'U')) &&
                            (lineptr[n - 4] == '.')
                            ))continue;
                        //  �������� �����, � �����������
                    }
                    else 
                    {// ��� �����
                        lineptr[n - 1] = 0;
                        folder = 1;
                    }


                    // ��� �������� ����� �� ����������� ������ "�����" � "������"
                    if(!_tcscmp(lineptr, _T(".")))
                    {// ������� � �������� ����� ��������
                        if(_tcslen(query))
                        {
                            print_row_tag();
                            insert_image(pict_to_start_page, full_script_name, 0/*url*/, 32, hint_folder);
                            _tprintf(_T("%s\n"), home/*��������*/);
                            close_inner_table_row();
                        }
                    }
                    else
                    if(!_tcscmp(lineptr, _T("..")))
                    {// ������� �� ���� ������� ����� ��� � �������� ����� ��������
                        if(_tcslen(query))
                        {
                            TCHAR *temp = malloc((_tcslen(query) + 1) * sizeof(TCHAR));
                            print_row_tag();

                            if(temp)
                            {
                                TCHAR *up_dir;
                                _tcscpy(temp, query);
                                up_dir = (TCHAR*)_tcsrchr (temp, '/');
                                if(up_dir) 
                                {
                                    *up_dir = 0;
                                    insert_image(pict_to_upper_folder, full_script_name, temp/*url*/, 32, hint_folder);
                                    _tprintf(_T("%s\n"), up/*��������*/);
                                    close_inner_table_row();
                                }
                                // ������� ����� �������� �������� - ������ �� �����������
                                /*
                                else
                                {
                                    insert_image(pict_to_upper_folder, full_script_name, 0, 32, hint_folder);
                                    _tprintf(_T("%s\n"), up);
                                    close_inner_table_row();
                                }
                                */
                                free(temp);
                            }
                            /*  ������� � �������� ����� ��� ����������� ����
                            else
                            {
                                _tprintf(_T("%s"), _T("Out of memory<BR>\n"));
                                // ������� � �������� ����� ��������
                                insert_image(pict_to_start_page, full_script_name, 0, 32, hint_folder);
                                _tprintf(_T("%s\n"), home);
                                close_inner_table_row();
                            }
                            */
                        }
                    }
                    else
                    //  ���� ������� ������ �� ����� ".", ".." ��� 0, ��
                    //  ������ ������� �������� ��� ������� �����, ������� �����
                    //  ��������� � ������ ��������� ������ ��� ����, �����
                    //  ��� ����������� ������� ��������� ������� � ������ �����
                    {
                        if(_tcscmp(query,_T(".")) && _tcscmp(query,_T("..")))
                        {
                            //  ����������� URL ������� ������ � ��������� ������
                            if(folder)
                            {
                                //  full_dir � ���������� �������� url ������������ url ������� 
                                //  � ����� ��� ����� - ������������ ��� �������� ������
                                TCHAR *full_dir = lineptr;
                                if(_tcslen(query))
                                {
                                    full_dir = malloc((_tcslen(query) + _tcslen(lineptr) + 2) * sizeof(TCHAR));
                                    if(full_dir)
                                    {
                                        _tcscpy(full_dir, query);
                                        _tcscat(full_dir, _T("/"));
                                        _tcscat(full_dir, lineptr);
                                    }
                                    /*
                                    else
                                    {// ������ �� ��������
                                    }
                                    */
                                }

                                print_row_tag();
                                if(full_dir)
                                    insert_image(pict_folder, full_script_name, full_dir/*url*/, 32, hint_folder);

                                //  ����������� ������ ���������� �������
                                _tprintf(_T("%s\n\
                    </td>\n\
                    <td>\n"), lineptr);
                                insert_command_image(cmd_del_folder_conf, query, lineptr, pict_delete_folder, hint_delete);
                                _tprintf(_T("%s"), _T("\n\
                    </td>\n\
                    <td>\n"));
                                insert_command_image(cmd_edit_folder_conf, query, lineptr, pict_edit_folder, hint_edit);
                                _tprintf(_T("%s"), _T("\n\
                    </td>\n\
                </tr>\n"));

                                if(full_dir != lineptr)free(full_dir);

                            }
                            else 
                            {
                                //  !!! ����� ����������� ���������� ���������������
                                //  �� lineptr, ����� ����������� �������� ������� !!!
                                //  �����������������
                                TCHAR *url = read_str_cwd(lineptr, str_url);
#ifdef EXTENDED_URL_FILE
                                TCHAR *name = read_str_cwd(lineptr, str_name);
#endif

                                if(first_link)
                                {
                                    if(!first_folder)
                                        //  ��������� ��������� ������� � ������ ������� �������
                                        _tprintf(_T("%s"), _T("\
            </table>\n\
        </td>\n")); 
                                    //  ��������� ������ ������� �������, ������� ��������� �������,
                                    //  ��������� ������ � ������ ��������� �������
                                    _tprintf(_T("%s"), _T("\n\
        <td valign=\"top\">\n\
            <table border=\"0\">\n\
                <tr>\n\
                    <td>\n"));
                                    first_link = 0;
                                    first_folder = 1;
                                }
                                else
                                {
                                    _tprintf(_T("%s"), _T("\n\
                <tr>\n\
                    <td>\n"));
                                }


                                if(url)
                                {
                                    insert_image(pict_link, url, 0, 32, hint_link);
                                    free(url);
                                }
                                else
                                    insert_image(pict_error, 0, err_imposs_read_url, 32, hint_error_colon);

#ifdef EXTENDED_URL_FILE
                                if(name)
                                {
                                    _tprintf(_T("%s\n\
                    </td>\n\
                    <td>\n"), name);
                                    free(name);
                                }
                                else
#endif
                                {// ������ �������� �� ����� ����� ��� ����������
                                    TCHAR *dot = (TCHAR*)_tcsrchr(lineptr, '.');
                                    if(dot)*dot = 0;
                                    _tprintf(_T("%s\n\
                    </td>\n\
                    <td>\n"), lineptr);
                                    if(dot)*dot = '.';  //  �������������� ����� �����
                                }
                                //  ������� ������
                                insert_command_image(cmd_del_conf, query, lineptr, pict_delete_link, hint_delete);
                                _tprintf(_T("\n\
                    </td>\n\
                    <td>\n"));
                                insert_command_image(cmd_edit_conf, query, lineptr, pict_edit_link, hint_edit);
                                _tprintf(_T("\n\
                    </td>\n\
                </tr>\n"));
                            }
                        }
                    }
                }
                //free(lineptr);
            }

            if(!first_folder || !first_link)
                //  ����������� ��������� ������� � ������ ������� �������
                printf(_T("%s"), _T("\
            </table>\n\
        </td>\n"));
            //  ����������� ������ ������� ������� � ������� �������
            printf(_T("%s"), _T("\
    </tr>\n\
</table>\n"));
            fclose(f);
        }
    }

    /*  ���� ����� �� ��������� �������� ������� 
        �������� ����������, ����� ����� ����� (����������
        �������� �������� ������ �� �������� ����������);
        ���� � �������������� POSIX ������������� ��� ������
    DIR *dp;
    struct dirent *ep;

    dp = opendir (_T("./"));
    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            _tprintf(_T("%s\n"), ep->d_name);
        }
        (void) closedir (dp);
    }
    else
        puts(_T("Couldn't open the directory."));
    */
    _tremove(temp);
}
//-------------------------------------------------------------------------------------------------

void insert_add_buttons()
{
    _tprintf(_T("%s"), _T("\n<p>\n"));
    insert_command_image(cmd_add_conf, query, _T(""), pict_add_link, hint_add_link);
    _tprintf(_T("%s"), _T("\n"));
    insert_command_image(cmd_add_folder_conf, query, _T(""), pict_add_folder, hint_add_folder);
    _tprintf(_T("%s"), _T("\n</p>\n"));
}
//-------------------------------------------------------------------------------------------------

#ifdef UNICODE
wmain( int argc, wchar_t *argv[ ], wchar_t *envp[ ] )
#else
int main(int argc, TCHAR *argv[])
#endif  //  UNICODE
{
    _tprintf(_T("%s"), _T("Content-type: text/html\n\n"));
    
    //  ��������� �������
    //  ��������� �������� ��������
    query = _tgetenv(_T("QUERY_STRING"));
    if(query)
    {
        int script_name_len = _tcslen(http);     //  ������ ����� ���� � �������
        TCHAR *server_name = _tgetenv(_T("SERVER_NAME"));
        TCHAR *script_name = _tgetenv(_T("SCRIPT_NAME"));

        if(!server_name)no_environment(_T("SERVER_NAME"));
        if(!script_name)no_environment(_T("SCRIPT_NAME"));
        if(!fatal_error)
        {
            full_script_name = 0;
            script_name_len += _tcslen(server_name);
            script_name_len += _tcslen(script_name);

            full_script_name = malloc((script_name_len + 1) * sizeof(TCHAR));
            if(full_script_name)
            {
                //  ��������� ��� ������� ��� �������� ������
                _tcscpy(full_script_name, http);
                _tcscat(full_script_name, server_name);
                _tcscat(full_script_name, script_name);

                img_path = malloc((_tcslen(server_name) + _tcslen(http) + 2) * sizeof(TCHAR));
                //  ��������� ���� � ������� ��� �������� ������ �� ��������
                if(img_path)
                {
                    _tcscpy(img_path, http);
                    _tcscat(img_path, server_name);
                    //  www - ����� ��-��������� ��� ���-����, �������
                    //  ��������� �� ���� �� �����
                    _tcscat(img_path, _T("/"));
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
                switch(get_query_command(1))
                {
                case(CMD_CHANGE_FOLDER): process_query(0); 
                    if(!fatal_error)do_change_folder(); 
                    break;

                case(CMD_EDIT): process_query(1); 
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_edit(); 
                    }
                    break;

                case(CMD_EDIT_CONF): process_query(0); 
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_edit_conf(); 
                    }
                    break;

                case(CMD_ADD): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_add(); 
                    }
                    break;

                case(CMD_ADD_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_add_conf(); 
                    }
                    break;

                case(CMD_DEL): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_del(); 
                    }
                    break;

                case(CMD_DEL_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_del_conf(); 
                    }
                    break;

                case(CMD_EDIT_FOLDER): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_edit_folder(); 
                    }
                    break;

                case(CMD_EDIT_FOLDER_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_edit_folder_conf(); 
                    }
                    break;

                case(CMD_ADD_FOLDER): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_add_folder(); 
                    }
                    break;

                case(CMD_ADD_FOLDER_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_add_folder_conf(); 
                    }
                    break;

                case(CMD_DEL_FOLDER): process_query(1);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_del_folder(); 
                    }
                    break;

                case(CMD_DEL_FOLDER_CONF): process_query(0);
                    if(!fatal_error) {
                        get_query_command(0);   //  ��������� ����� � ��������������� �������
                        do_del_folder_conf(); 
                    }
                    break;
                }// switch(get_query_command)
            }// if(cwd)
            else
                out_of_memory();
        }
        
        if(query)free(query);
        if(img_path)free(img_path);
        if(cwd)free(cwd);
        if(full_script_name)free(full_script_name);
    }// if(query)
    else 
        no_environment(_T("QUERY_STRING"));

    return 0;
}
//-------------------------------------------------------------------------------------------------