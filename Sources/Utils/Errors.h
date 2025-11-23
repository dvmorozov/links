#pragma once
#include "App/Globals.h"
#include "Miscellaneous.h"

//  String constants are converted from UTF-8 to wide strings (UTF-16).
//  This is necessary for Windows build.
//  TODO: move string constants into separate "resource" file.
const std::wstring err_change_directory  = widen("Cannot change to directory: ");
const std::wstring err_file_doesnt_exist = widen("File does not exist: ");
const std::wstring err_out_of_memory     = widen("Insufficient memory to execute");
const std::wstring err_no_permission     = widen("Insufficient permissions to access ");
const std::wstring err_invalid_login     = widen("Invalid username or password");
const std::wstring err_invalid_query     = widen("Invalid query: ");
const std::wstring err_details           = widen("Details: ");
const std::wstring err_no_environment    = widen("Required environment is missing: ");
const std::wstring err_sys_utility       = widen("Utility %ls call failed with error");
const std::wstring err_system_exception  = widen("System error: ");
const std::wstring err_not_assigned      = widen("Value is not assigned.");

//  TODO: replace these functions by explicit throwing exceptions.
//  TODO: add "file", "line" parameters to all functions.
void invalid_query_fl(const char *file, int line);
void out_of_memory();
void file_doesnt_exist(const std::wstring &file_path);
void no_permission_fl(const std::wstring &file_path, const char *file, int line);
void no_environment(const TCHAR *env_str);
void system_error(const std::wstring &what);
void sysutility_error(const std::wstring &what, const std::wstring &command);

#define invalid_query()          invalid_query_fl(__FILE__, __LINE__)
#define no_permission(file_path) no_permission_fl(file_path, __FILE__, __LINE__)
