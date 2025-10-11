
#include "App/stdafx.h"

#include "Globals.h"
#include "Utils/DebugLog.h"

int error       = 0;
int fatal_error = 0;

// The pointer to query is modified during parsing.
TCHAR       *query = 0;     // Query without script path.
std::wstring full_script_name;
std::wstring images_root;
std::wstring server_name;
#ifdef _DEBUG
std::wstring server_port = L"4433";
#else
std::wstring server_port = L"4444";
#endif
std::wstring server_root;
std::wstring script_name;
std::wstring cwd;           // Current working directory.
TCHAR       *command  = 0;
TCHAR       *key      = 0;
TCHAR       *username = 0;
TCHAR       *password = 0;

#ifdef _WINDOWS
TCHAR path_separator = '\\';
#else
TCHAR path_separator = '/';
#endif
std::wstring document_root;

//  The directory for temporary session files.
std::wstring tmp_dir = _T("temp/links/");
