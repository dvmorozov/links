
#pragma once

#ifdef _WINDOWS
#include <tchar.h>
#elif LINUX
#define TCHAR wchar_t
#endif
#include "..\main.h"

//  коды команд
#define CMD_CHANGE_FOLDER       0
#define CMD_EDIT                1
#define CMD_ADD                 2
#define CMD_DEL                 3                       //  удаление ссылки
#define CMD_EDIT_FOLDER         4
#define CMD_ADD_FOLDER          5
#define CMD_DEL_FOLDER          6                       //  удаление папки
#define CMD_DEL_CONF            7                       //  запрос на удаление ссылки
#define CMD_DEL_FOLDER_CONF     8                       //  запрос на удаление папки
#define CMD_EDIT_FOLDER_CONF    9                       //  запрос на изменение имени папки
#define CMD_EDIT_CONF           10                      //  запрос на изменение ссылки
#define CMD_ADD_CONF            11                      //  запрос на создание ссылки (выводит форму ввода ссылки)
#define CMD_ADD_FOLDER_CONF     12                      //  запрос на создание папки (выводит форму ввода имени папки)
#define CMD_LOG_IN_CONF         13                      //  вывод формы ввода имени пользователя и пароля
#define CMD_LOG_IN              14                      //  обработка формы ввода имени пользователя и пароля
#define CMD_KEY                 15                      //  klyuch zaprosa
#define CMD_UNKNOWN             0xff

typedef void(*check_log_in_result)();
extern check_log_in_result check_log_in_params();
extern std::wstring getKeyFileName();

extern void change_folder();
extern void do_change_folder();
extern void remove_folder(TCHAR *folder);

extern void invalid_query();
extern void out_of_memory();
extern void no_environment(const TCHAR *env_str);

extern int get_query_command(unsigned char does_not_processes /* priznak togo, chto zapros peredaetsya v pervonachal'nom vide -
                                                              zapret udaleniya komandy iz stroki */);
extern int hex_char_to_int(TCHAR c);
extern void process_query(unsigned char delete_spaces);
extern void prepare_query_buffer(int len);
extern void do_log_in_conf();
extern void do_log_in();
extern void get_key();
extern void do_edit();
extern void do_edit_conf();
extern void do_add();
extern void do_add_conf();
extern void do_del();
extern void do_del_conf();
extern void do_edit_folder();
extern void do_edit_folder_conf();
extern void do_add_folder();
extern void do_add_folder_conf();
extern void do_del_folder();
extern void do_del_folder_conf();

extern std::wstring DocumentRoot; 

extern TCHAR url_file_template[];
extern TCHAR *query;
extern TCHAR *img_path;
extern TCHAR cmd_key[];
extern TCHAR *key;
extern TCHAR *full_script_name;
extern TCHAR cmd_add_conf[];
extern TCHAR cmd_add_folder_conf[];
extern TCHAR cmd_ch_folder[];
extern TCHAR ok[];
extern TCHAR cancel[];
extern TCHAR cmd_edit[];
extern TCHAR cmd_add[];
extern TCHAR cmd_log_in[];
extern TCHAR cmd_del[];
extern TCHAR cmd_edit_folder[];
extern TCHAR cmd_add_folder[];
extern TCHAR cmd_del_folder[];
extern TCHAR cmd_del_folder_conf[];
extern TCHAR cmd_edit_folder_conf[];
extern TCHAR cmd_del_conf[];
extern TCHAR cmd_edit_conf[];
extern TCHAR *cwd;
extern TCHAR *command;  //  ук-ль на переданную в запросе команду
                        //  (освобождается вместе с освобождением запроса)
extern TCHAR *username;
extern TCHAR *password;
extern TCHAR www_sub[];

void print_exception(const char* what = nullptr);

void MakeFolder(std::wstring name);
std::wstring GetUserDirName();
std::wstring GetTmpDirName();
std::wstring GetFullDirName(std::wstring relDirName);
#ifdef USE_SCRIPT_NAME
int HandleQuery(TCHAR* encodedQuery, TCHAR* scriptName);
#else
int HandleQuery(TCHAR* encodedQuery);
#endif
