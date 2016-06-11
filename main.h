
#pragma once

#ifndef MAIN_H
#define MAIN_H

//#define LINUX     //  native
#define EXTENDED_URL_FILE

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <clocale>

#ifndef LINUX
#include <Windows.h>
#endif

#define TCHAR       char
#define _T(string)  string

#define _tfopen     fopen
#define _tmkdir     _mkdir
#define _tgetenv    getenv
#define _tgetcwd    _getcwd
#define _tsystem    system
#define _tchdir     _chdir
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
#define _tmktemp    _mktemp

#if defined _WINDOWS

#include <string.h>
#include <io.h>
#include <direct.h>
#include <time.h>

#endif

//  коды ошибок
#ifdef E_OUTOFMEMORY
#undef E_OUTOFMEMORY
#endif
#define E_OUTOFMEMORY       1
#define E_INVALID_QUERY     2
#define E_CHANGE_FOLDER     3
#define E_NO_ENVIRONMENT    4
#define E_SYS_UTILITY       5

#define MAX_LINE_LENGTH     1000                        //  максимально допустимая длина строки файла, содержащего имена файлов и папок

extern std::string ext;                             //  расширение файлов ссылок
extern std::string document_root;

extern int error;
extern int fatal_error;                             //  priznak vyvoda soobscheniya ob oschibke
extern int first_folder;
extern int first_link;

extern TCHAR str_url[];
extern TCHAR str_name[];
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
extern TCHAR cmd_del_folder_conf[];
extern TCHAR cmd_edit_folder_conf[];
extern TCHAR cmd_del_conf[];
extern TCHAR cmd_edit_conf[];
extern TCHAR *cwd;
extern TCHAR *read_str_cwd(TCHAR *url_file_name/*путь к файлу отн. cwd*/, TCHAR *str_name);

#endif  //  MAIN_H
