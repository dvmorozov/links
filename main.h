
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

#define TCHAR       wchar_t
#define _T(string)  L##string

#define _tfopen     _wfopen
#define _tmkdir     _wmkdir
#define _tgetenv    _wgetenv
#define _tgetcwd    _wgetcwd
#define _tsystem    _wsystem
#define _tchdir     _wchdir
#define _tcslen     wcslen
#define _tcschr     wcschr
#define _tcscat     wcscat
#define _tcscpy     wcscpy
#define _ftprintf   fwprintf
#define _tprintf    wprintf
#define _tsprintf   swprintf
#define _tcsrchr    wcschr
#define _tcscmp     wcscmp
#define _trename    _wrename
#define _tcsncmp    wcsncmp
#define _tremove    _wremove
#define _fgetts     fgetws
#define _tmktemp    _wmktemp

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

#define MAX_LINE_LENGTH     1000                    //  максимально допустимая длина строки файла, содержащего имена файлов и папок

extern std::wstring ext;                            //  расширение файлов ссылок
extern std::wstring document_root;

extern int error;
extern int fatal_error;                             //  priznak vyvoda soobscheniya ob oschibke

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
extern TCHAR *read_str_cwd(TCHAR *url_file_name/*путь к файлу отн. cwd*/, TCHAR *str_name);

#endif  //  MAIN_H
