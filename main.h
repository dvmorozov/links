
#pragma once

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

#ifndef _TEST
#define _tprintf(...) wprintf(##__VA_ARGS__);
#else
extern TCHAR *test_str;
#define _tprintf(...) {swprintf(test_str, ##__VA_ARGS__); test_str += wcslen(test_str);}
#endif

#if defined _WINDOWS

#include <string.h>
#include <io.h>
#include <direct.h>
#include <time.h>

#endif

//  ���� ������
#ifdef E_OUTOFMEMORY
#undef E_OUTOFMEMORY
#endif
#define E_OUTOFMEMORY       1
#define E_INVALID_QUERY     2
#define E_CHANGE_FOLDER     3
#define E_NO_ENVIRONMENT    4
#define E_SYS_UTILITY       5

#define MAX_LINE_LENGTH     1000                    //  ����������� ���������� ����� ������ �����, ����������� ����� ������ � �����

extern int error;
extern int fatal_error;                             //  priznak vyvoda soobscheniya ob oschibke
