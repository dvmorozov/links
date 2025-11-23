#pragma once
#include "App/Globals.h"

// Command codes.
#define CMD_CHANGE_FOLDER    0
#define CMD_EDIT             1
#define CMD_ADD              2
#define CMD_DEL              3  // Delete link.
#define CMD_EDIT_FOLDER      4
#define CMD_ADD_FOLDER       5
#define CMD_DEL_FOLDER       6   // Delete folder.
#define CMD_DEL_CONF         7   // Request to delete link.
#define CMD_DEL_FOLDER_CONF  8   // Request to delete folder.
#define CMD_EDIT_FOLDER_CONF 9   // Request to change folder name.
#define CMD_EDIT_CONF        10  // Request to change link.
#define CMD_ADD_CONF         11  // Request to create link (displays link input form).
#define CMD_ADD_FOLDER_CONF  12  // Request to create folder (displays folder name input form).
#define CMD_LOG_IN_CONF      13  // Display username and password input form.
#define CMD_LOG_IN           14  // Process username and password input form.
#define CMD_KEY              15  // Request key.
#define CMD_AUTO_FOLDER      16  // Auto-organize bookmarks using AI.
#define CMD_UNKNOWN          0xff

extern void         check_log_in_params();
extern std::wstring get_key_file_path();
extern void         change_directory();
extern void         do_change_directory();
extern int  get_query_parameter(unsigned char does_not_processes /* Flag indicating that the query is passed in its initial form - 
                                                                  preventing deletion of the command from the string.*/);
extern void process_query(unsigned char delete_spaces);
extern void pepare_query_buffer(size_t queryLen);
extern void do_login_conf();
extern void do_login_conf_with_error(const std::wstring& errorMessage);
extern void do_login();
extern void get_key();
extern void do_edit();
extern void do_edit_conf();
extern void do_add_link();
extern void do_add_link_conf();
extern void do_delete_link();
extern void do_delete_link_conf();
extern void do_edit_directory();
extern void do_edit_directory_conf();
extern void do_add_directory();
extern void do_add_directory_conf();
extern void delete_directory(const std::wstring &directory_name);
extern void do_delete_directory();
extern void do_delete_directory_conf();
extern void do_auto_folder();

extern TCHAR  url_file_template[];
extern TCHAR  cmd_key[];
extern TCHAR *key;
extern TCHAR  cmd_add_conf[];
extern TCHAR  cmd_add_directory_conf[];
extern TCHAR  cmd_ch_folder[];
extern TCHAR  ok[];
extern TCHAR  cancel[];
extern TCHAR  cmd_edit[];
extern TCHAR  cmd_add[];
extern TCHAR  cmd_log_in[];
extern TCHAR  cmd_del[];
extern TCHAR  cmd_edit_directory[];
extern TCHAR  cmd_add_directory[];
extern TCHAR  cmd_del_directory[];
extern TCHAR  cmd_del_directory_conf[];
extern TCHAR  cmd_auto_folder[];
extern TCHAR  cmd_edit_directory_conf[];
extern TCHAR  cmd_delete_conf[];
extern TCHAR  cmd_edit_conf[];
extern TCHAR  www_sub[];

void         make_directory(const std::wstring &name);
std::wstring get_user_directory_name();
std::wstring get_temp_directory_path();
std::wstring get_full_directory_path(const std::wstring &relDirName);
int          handle_query(const TCHAR *encodedQuery);
void         print_exception(const char *what = nullptr);
