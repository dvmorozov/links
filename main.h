
#pragma once

//#define LINUX             //  Use Linux toolchain.
//#define _WINDOWS          //  Use Windows-specific things.
//#define USE_CYGWIN        //  Use Cygwin-specific things.
#define EXTENDED_URL_FILE
//#define USE_SCRIPT_NAME

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <clocale>
#include <locale>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <assert.h>

#ifdef _WINDOWS

#include <Windows.h>
#include <tchar.h>
#include <io.h>
#include <direct.h>

#elif LINUX

#define TCHAR wchar_t
#define _T(x) L ## x

#endif

#ifdef _TEST
#pragma warning( push )
#pragma warning( disable : 4005 )
extern TCHAR *test_str;
#define _tprintf(...) {swprintf(test_str, ##__VA_ARGS__); test_str += wcslen(test_str);}
#pragma warning( pop ) 
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

extern int error;
extern int fatal_error;                             //  priznak vyvoda soobscheniya ob oschibke

class two_msg_exception : public std::exception
{
public:
    std::wstring msg1;
    std::wstring msg2;

    two_msg_exception(std::wstring _msg1, std::wstring _msg2) : msg1(_msg1), msg2(_msg2) {}
};
