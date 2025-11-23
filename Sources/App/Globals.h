#pragma once

#include <string>

#ifdef _WINDOWS
const wchar_t PATH_DELIMITER = '\\';
#else
const wchar_t PATH_DELIMITER = '/';
#endif

//  TODO: Remove this define and preserve only functionality which it turns on.
//  Removing this define will break compatibility with the old format of the links file.
#define EXTENDED_URL_FILE
// #define USE_CYGWIN        //  Use Cygwin-specific things.
#define LINKS_LEGACY

#ifdef _WINDOWS
#include <Windows.h>
#include <cstdlib>
#include <direct.h>
#include <io.h>
#include <tchar.h>
#else
#define TCHAR     wchar_t
#define _T(x)     L##x
#define _MAX_PATH PATH_MAX
#include <fcntl.h>
#include <iconv.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

//  Error codes.
#ifdef E_OUTOFMEMORY
#undef E_OUTOFMEMORY
#endif

//  TODO: don't use error codes, use exceptions instead.
#define E_OUTOFMEMORY       1
#define E_INVALID_QUERY     2
#define E_CHANGE_FOLDER     3
#define E_NO_ENVIRONMENT    4
#define E_SYS_UTILITY       5
#define E_NO_PERMISSION     6
#define E_SYS_ERROR         7
#define E_FILE_DOESNT_EXIST 8

#define MAX_LINE_LENGTH 1000            //  Maximum length of a line in a file, including the null character.
#define MAX_USER_NAME   100

extern TCHAR        path_separator;
extern std::wstring document_root;

extern int error;
extern int fatal_error;                 //  Flag indicating whether to output an error message.

//  !!! Note that all global variables are initialized to zero !!!
extern TCHAR       *query;              //  Not used if 0.
extern std::wstring full_script_name;   //  Not used if 0.
extern std::wstring images_root;        //  Contains path to the image folder.
extern std::wstring server_name;        //  Server name.
extern std::wstring server_port;        //  Outer server port.
extern std::wstring server_root;
extern std::wstring script_name;        //  Path to CGI script.
extern std::wstring cwd;                //  Current working directory.
extern TCHAR *command;                  //  Pointer to the command in the query string 
                                        //  (used only with command execution).
extern TCHAR *key;                      //  Key value extracted from the query string.
extern TCHAR *username;
extern TCHAR *password;

class two_msg_exception : public std::exception
{
public:
    std::wstring msg1;
    std::wstring msg2;

    two_msg_exception(const std::wstring &_msg1, const std::wstring &_msg2)
        : msg1(_msg1), msg2(_msg2)
    {
    }
#ifdef __APPLE__
    virtual ~two_msg_exception() _NOEXCEPT {};
#endif
};

// Special exception for authentication failures that have already been handled
// (error displayed) and should not show additional error pages
class auth_handled_exception : public std::exception
{
public:
    auth_handled_exception() {}
#ifdef __APPLE__
    virtual ~auth_handled_exception() _NOEXCEPT {};
#endif
};

extern std::wstring cmd_dir;
extern std::wstring tmp_dir;
