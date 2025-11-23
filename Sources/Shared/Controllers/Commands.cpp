#include "App/stdafx.h"

#include "Commands.h"
#include "Utils/DebugLog.h"
#include "Shared/Models/Config.h"
#include "Shared/Models/ConfigFactory.h"
#include "Features/BookmarkManagement/Models/Data.h"
#include "Features/BookmarkManagement/Models/LinkFileReader.h"
#include "Features/AIAutoOrganization/Models/OllamaService.h"
#include "Features/AIAutoOrganization/Models/BookmarkReorganizer.h"
#include "Utils/Errors.h"
#include "Utils/Miscellaneous.h"
#include "Features/BookmarkManagement/Views/AddDirectory.h"
#include "Features/BookmarkManagement/Views/AddLink.h"
#include "Features/AIAutoOrganization/Views/AutoFolderStatus.h"

// Repository Pattern Integration
#include "Shared/Repositories/RepositoryFactory.h"
#include "Shared/Repositories/IBookmarkRepository.h"
#include "Shared/Repositories/IDirectoryRepository.h"

#include <fstream>
#include <ctime>
#include <iomanip>
#include <typeinfo>
#include <unistd.h>
#include <filesystem>

#include <fstream>  // For Docker container detection
#include "Features/BookmarkManagement/Views/DeleteDirectory.h"
#include "Features/BookmarkManagement/Views/DeleteLink.h"
#include "Features/BookmarkManagement/Views/EditDirectory.h"
#include "Features/BookmarkManagement/Views/EditLink.h"
#include "Shared/UI/ErrorBox.h"
#include "Features/BookmarkManagement/Views/FileListBootstrap.h"
#include "Features/Authentication/Views/Login.h"

// Commands for execution by the program (inserted into the URL).
// Identical sets of commands are needed for folders and files, since the extension
// is not transmitted through the URL, and a folder and a file with the same name can be inside the folder.
TCHAR cmd_edit[]                = _T(";edit=");
TCHAR cmd_edit_conf[]           = _T(";edit_conf=");
TCHAR cmd_add[]                 = _T(";add=");
TCHAR cmd_log_in[]              = _T(";log_in=");
TCHAR cmd_add_conf[]            = _T(";add_conf=");
TCHAR cmd_del[]                 = _T(";del=");
TCHAR cmd_delete_conf[]         = _T(";del_conf=");
TCHAR cmd_ch_folder[]           = _T(";ch_folder=");
TCHAR cmd_edit_directory[]      = _T(";edit_folder=");
TCHAR cmd_edit_directory_conf[] = _T(";edit_folder_conf=");
TCHAR cmd_add_directory[]       = _T(";add_folder=");
TCHAR cmd_add_directory_conf[]  = _T(";add_folder_conf=");
TCHAR cmd_del_directory[]       = _T(";del_folder=");
TCHAR cmd_del_directory_conf[]  = _T(";del_folder_conf=");
TCHAR cmd_auto_folder[]         = _T(";auto_folder=");
TCHAR cmd_key[]                 = _T(";key=");

TCHAR ok[]         = _T("Ok");
TCHAR cancel[]     = _T("Cancel");
TCHAR pict_error[] = _T("error.bmp");

// Repository Pattern - Global instances for command processing
namespace {
    std::shared_ptr<Bookmarks::IBookmarkRepository> g_bookmarkRepo;
    std::shared_ptr<Bookmarks::IDirectoryRepository> g_directoryRepo;
    
    // Initialize repositories on first use
    void InitializeRepositories() {
        if (!g_bookmarkRepo) {
            auto factory = std::make_shared<Bookmarks::RepositoryFactory>();
            g_bookmarkRepo = factory->CreateBookmarkRepository(Bookmarks::RepositoryFactory::RepositoryType::Filesystem);
            g_directoryRepo = factory->CreateDirectoryRepository(Bookmarks::RepositoryFactory::RepositoryType::Filesystem);
        }
    }
}

// #define USE_CYGWIN

//-------------------------------------------------------------------------------------------------

void process_query(unsigned char delete_spaces)
{
    size_t query_len = std::wcslen(query);
    TCHAR *temp      = (TCHAR *)malloc((query_len + 1) * sizeof(TCHAR));
    TCHAR *temp2     = (TCHAR *)malloc((query_len + 1) * sizeof(TCHAR));
    if (temp && temp2) {
        decode_url(temp, query, delete_spaces);
        decode_html_entities_utf8(temp2, temp);
        decode_unicode_escapes(query, temp2);
        free(temp);
        free(temp2);
    } else
        out_of_memory();
}
//-------------------------------------------------------------------------------------------------

// Outputs the contents of the current folder.
void print_directory_content()
{
#ifdef LINKS_LEGACY
    Bookmarks::FileListLegacy p;
    // Set repositories for the file list
    InitializeRepositories();
    p.SetRepositories(g_bookmarkRepo, g_directoryRepo);
#else
    Bookmarks::FileListBootstrap p;
#endif
    p.Render();
}
//-------------------------------------------------------------------------------------------------

void do_edit_conf()
{
    // Command should not be 0 here.
    // Link contains the file name.
    TCHAR *link_file_name = (TCHAR *)wcschr(command, '=');
    if (link_file_name) {
        link_file_name++;
        // The directory name is taken from the URL displaying the current page.
        change_directory();
        if (!fatal_error) {
            Bookmarks::LinkFileReader fr(cwd);
            std::wstring              url = fr.GetParamFromCurDirFile(link_file_name, param_url);
            // Name contains the file name without extension.
            std::wstring name;
#ifdef EXTENDED_URL_FILE
            name = fr.GetParamFromCurDirFile(link_file_name, param_name);
            if (name.empty())
#endif
            {  // The URL name in the file is not specified or not supported -
                // Make a name from the file name without extension.
                TCHAR *dot;

                name = link_file_name;
                dot  = (TCHAR *)wcsrchr(link_file_name, '.');
                if (dot) {
                    *dot = 0;
                    name = link_file_name;
                    *dot = '.';
                }
            } else {
                Bookmarks::EditLink p(name, url, link_file_name);
                p.Render();
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------
// Link editing.
void do_edit()
{
    // Command should not be 0 here.
    TCHAR *link = (TCHAR *)wcschr(command, '=');
    if (link) {
        link++;
        if (!wcscmp(link, ok)) {
            if (get_query_parameter(0) == CMD_EDIT) {  // Now after = the old link name.
                TCHAR *old_name = (TCHAR *)wcschr(command, '=');
                if (old_name) {
                    old_name++;
                    if (get_query_parameter(0) ==
                        CMD_EDIT) {  // Now after - the new link name (without extension).
                        TCHAR *new_name = (TCHAR *)wcschr(command, '=');
                        if (new_name) {
#ifdef EXTENDED_URL_FILE
                            TCHAR *and_;
                            new_name++;
                            if ((and_ = (TCHAR *)wcsrchr(new_name, '&')))
                                *and_ = 0;
#else
                            TCHAR *and;
                            TCHAR *dot;
                            TCHAR *new_name_ext;
                            new_name++;
                            new_name_ext = new_name;
                            // The & symbol must be removed from the new name.
                            if (and = (TCHAR *)wcsrchr(new_name, '&'))
                                *and = 0;
                            // The new name must be combined with the old extension.
                            if (dot = (TCHAR *)wcsrchr(old_name, '.')) {  // There is an extension.
                                new_name_ext = malloc((std::wcslen(new_name) + wcslen(dot) + 1) *
                                                      sizeof(TCHAR));
                                if (new_name_ext) {
                                    wcscpy(new_name_ext, new_name);
                                    wcscat(new_name_ext, dot);
                                } else
                                    out_of_memory();
                            }
#endif
                            if (!fatal_error) {  // Transition to the folder and renaming the file.
                                change_directory();
                                if (!fatal_error) {
#ifdef EXTENDED_URL_FILE
                                    Bookmarks::LinkFileReader fr(cwd);
                                    std::wstring              url =
                                        fr.GetParamFromCurDirFile(old_name, param_url);
                                    // File replacement (the rest of the information from the file is lost).
                                    create_url_file(new_name, url.c_str());
                                    std::filesystem::remove(std::filesystem::absolute(old_name));
#else
                                    // Renaming.
                                    std::filesystem::rename(
                                        std::filesystem::absolute(old_name),
                                        std::filesystem::absolute(new_name_ext));
#endif
                                    print_directory_content();
                                }
#ifndef EXTENDED_URL_FILE
                                if (new_name_ext != new_name)
                                    free(new_name_ext);
#endif
                            }
                        } else
                            invalid_query();
                    } else
                        invalid_query();
                } else
                    invalid_query();
            } else
                invalid_query();
        } else if (!wcscmp(link, cancel)) {  // In this subcommand the old link name is shortened.
            if (get_query_parameter(0) == CMD_EDIT) {  // Now after = the new link name.
                change_directory();
                print_directory_content();
            } else
                invalid_query();
        } else
            invalid_query();
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_add_link_conf()
{
    Bookmarks::AddLink p;
    p.Render();
}
//-------------------------------------------------------------------------------------------------

void do_login_conf()
{
    Bookmarks::Login p;
    p.Render();
}
//-------------------------------------------------------------------------------------------------

void do_login_conf_with_error(const std::wstring& errorMessage)
{
    Bookmarks::Login p(errorMessage);
    p.Render();
}
//-------------------------------------------------------------------------------------------------

// Returns a function which is used to render output.
void check_log_in_params()
{
    if (!query)
        invalid_query();

    // Command should not be 0 here.
    // The command is located after '='.
    TCHAR *link = (TCHAR *)wcsrchr(query, '=');

    if (link) {
        link++;
        if (!wcscmp(link, ok)) {  // The next part of the string is extracted - password.
            // TODO: Use another parameter name for password.
            if (get_query_parameter(0) == CMD_LOG_IN) {  // Now after = password.
                if ((password = (TCHAR *)wcsrchr(query, '='))) {
                    TCHAR *and_;
                    password++;
                    // The & symbol must be removed from the password.
                    if ((and_ = (TCHAR *)wcschr(password, '&')))
                        *and_ = 0;
                    // Removes the remainder of the password parameter.
                    if (get_query_parameter(0) ==
                        CMD_LOG_IN) {  // Now after = username.
                        if ((username = (TCHAR *)wcsrchr(query, '='))) {
                            username++;
                            // The & symbol must be removed from the username.
                            if ((and_ = (TCHAR *)wcschr(username, '&')))
                                *and_ = 0;
                            // Removes the remainder of the username parameter.
                            if (get_query_parameter(0) == CMD_LOG_IN) {
                                // Check username and password.
                                std::unique_ptr<Bookmarks::Config> config(
                                    Bookmarks::ConfigFactory::GetConfig());
                                if (!wcscmp(username, config->GetValue(_T("UserName")).c_str()) &&
                                    !wcscmp(password, config->GetValue(_T("Password")).c_str()))
                                    return;

                                // Display login form with error message and stop processing
                                do_login_conf_with_error(err_invalid_login);
                                throw auth_handled_exception();
                            } else
                                return invalid_query();
                        } else
                            return invalid_query();
                    } else
                        return invalid_query();
                } else
                    return invalid_query();
            } else
                return invalid_query();
        } else
            return do_login_conf();
    } else
        return invalid_query();
}
//-------------------------------------------------------------------------------------------------
// Returns the session key file name.
std::wstring get_key_file_path()
{
    // Creating key value.
    time_t t;
    time(&t);
    // Must be static because it is referenced by the "key" variable.
    static TCHAR k[11];
    std::swprintf(k, sizeof(k) / sizeof(*k), _T("%i"), (int)t);
    key = k;
    return get_temp_directory_path() + k;
}
//-------------------------------------------------------------------------------------------------
// Checking username and password and login.
void do_login()
{
    check_log_in_params();
    // Null means that login just has been performed.
    // New keyfile must be created.
    std::wstring key_file_path = get_key_file_path();
    FILE        *f;
    // Checking file existence.
#ifdef _WINDOWS
    f = _wfopen(key_file_path.c_str(), L"r");
#else
    f = std::fopen(narrow(key_file_path).c_str(), "r");
#endif
    if (f)
        do_login_conf();  // Repeated input is required -
                          // Another copy of the script has already
                          // created the file.
    else {
        // The key file is created.
        std::wstring directory_path = get_directory_name(key_file_path);
        if (!directory_exists(directory_path)) {
            try {
                std::filesystem::create_directories(directory_path);
            } catch (const std::exception &e) {
#ifdef _WINDOWS
                // TODO: Use UTF-8 (default)?
                const std::wstring message = widen(e.what(), CP_ACP);
#else
                const std::wstring message = widen(e.what());
#endif
                system_error(message);
            }
        }

#ifdef _WINDOWS
        f = _wfopen(key_file_path.c_str(), L"w");
#else
        f = std::fopen(narrow(key_file_path).c_str(), "w");
#endif
        if (!f)
            no_permission(key_file_path);

        if (std::wcslen(username) > MAX_USER_NAME - 1) {
            fclose(f);
            std::filesystem::remove(key_file_path);
            out_of_memory();
        } else {
            fwprintf(f, _T("%ls\r\n"), username);
            fclose(f);
            do_change_directory();
        }
    }
}
//-------------------------------------------------------------------------------------------------
// Adding a link.
void do_add_link()
{
    // Command should not be 0 here.
    TCHAR *link = (TCHAR *)wcschr(command, '=');
    if (link) {
        link++;
        if (!wcscmp(link, ok)) {
            DEBUG_LOG("Link is 'ok', checking first get_query_parameter(0)");
            if (get_query_parameter(0) == CMD_ADD) {  // Now after = url of the new link.
                DEBUG_LOG("First get_query_parameter(0) == CMD_ADD, looking for URL parameter");
                TCHAR *url = (TCHAR *)wcschr(command, '=');
                if (url) {
                    url++;
                    // Remove trailing & symbol that appears as parameter separator
                    size_t len = wcslen(url);
                    if (len > 0 && url[len-1] == '&') {
                        url[len-1] = '\0';
                    }
                    DEBUG_LOG("Parsed URL: " << narrow(url));

                    if (get_query_parameter(0) ==
                        CMD_ADD) {  // Now after = name of the new link (without extension).
                        DEBUG_LOG("Looking for name parameter in command: " << narrow(command));
                        TCHAR *name = (TCHAR *)wcschr(command, '=');
                        if (name) {
                            name++;
                            // Remove trailing & symbol that appears as parameter separator  
                            size_t len = wcslen(name);
                            if (len > 0 && name[len-1] == '&') {
                                name[len-1] = '\0';
                            }
                            DEBUG_LOG("Parsed name: " << narrow(name));
                            // Transition to the folder and creating a .url file.
                            change_directory();
                            if (!fatal_error) {
                                // Use repository pattern for bookmark creation
                                InitializeRepositories();
                                auto result = g_bookmarkRepo->CreateBookmark(name, url, L"");
                                if (result.success) {
                                    print_directory_content();
                                } else {
                                    DEBUG_LOG("Repository creation failed: " << narrow(result.message));
                                    // Error handling - repository should not fail since it contains the same logic
                                    invalid_query();
                                }
                            }
                        } else {
                            DEBUG_LOG("Name parameter not found - command does not contain '='");
                            invalid_query();
                        }
                    } else {
                        DEBUG_LOG("Second get_query_parameter(0) != CMD_ADD");
                        invalid_query();
                    }
                } else {
                    DEBUG_LOG("URL parameter not found - command does not contain '='");
                    invalid_query();
                }
            } else
                invalid_query();
        } else if (!wcscmp(link, cancel)) {
            if (get_query_parameter(0) == CMD_ADD) {      // Now after = url of the new link.
                if (get_query_parameter(0) == CMD_ADD) {  // Now after = name of the new link.
                    change_directory();
                    print_directory_content();
                } else
                    invalid_query();
            } else
                invalid_query();
        } else
            invalid_query();
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------
// Deleting a link.
void do_delete_link()
{
    // Recognizing the result.
    TCHAR *result = (TCHAR *)wcschr(command, '=');
    if (result) {
        if (get_query_parameter(0) == CMD_DEL) {  // Now after = folder name.
            if (!wcsncmp(result + 1, ok, std::wcslen(ok))) {
                change_directory();
                if (!fatal_error) {
                    result = (TCHAR *)wcschr(command, '=');
                    if (result) {
                        result++;
                        // Deleting the link - result contains the file name.
                        std::filesystem::remove(result);
                        print_directory_content();
                    }
                }
            } else {  // Displaying the contents of the current folder.
                do_change_directory();
            }
        } else
            invalid_query();
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------
// Request to delete a link.
void do_delete_link_conf()
{
    // Command should not be 0 here.
    // Need to search to the right of command.
    TCHAR *link_file_name = (TCHAR *)wcschr(command, '=');
    if (link_file_name) {
        link_file_name++;
        change_directory();
        if (!fatal_error) {
            Bookmarks::LinkFileReader fr(cwd);
            std::wstring              url = fr.GetParamFromCurDirFile(link_file_name, param_url);
            std::wstring              name;
#ifdef EXTENDED_URL_FILE
            name = fr.GetParamFromCurDirFile(link_file_name, param_name);
            if (name.empty())
                name = link_file_name;
#else
            name = link;
#endif
            Bookmarks::DeleteLink p(name, url, link_file_name);
            p.Render();
        } else
            invalid_query();
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_edit_directory_conf()
{
    TCHAR *directory_name = (TCHAR *)wcschr(command, '=');
    if (directory_name) {
        directory_name++;
        Bookmarks::EditDirectory p(directory_name);
        p.Render();
    }
}
//-------------------------------------------------------------------------------------------------

void do_edit_directory()
{
    // Command should not be 0 here.
    TCHAR *link = (TCHAR *)wcschr(command, '=');
    if (link) {
        link++;
        if (!wcscmp(link, ok)) {
            if (get_query_parameter(0) == CMD_EDIT_FOLDER) {  // Now after = old folder name.
                TCHAR *old_name = (TCHAR *)wcschr(command, '=');
                if (old_name) {
                    old_name++;
                    if (get_query_parameter(0) ==
                        CMD_EDIT_FOLDER) {  // Now after = new folder name.
                        TCHAR *new_name = (TCHAR *)wcschr(command, '=');
                        if (new_name) {
                            TCHAR *and_;
                            new_name++;
                            // The & symbol must be removed from the new name.
                            if ((and_ = (TCHAR *)wcsrchr(new_name, '&')))
                                *and_ = 0;

                            // Transition to the folder and renaming the file.
                            change_directory();
                            if (!fatal_error) {  // Renaming.
                                std::filesystem::rename(std::filesystem::absolute(old_name),
                                                        std::filesystem::absolute(new_name));
                                print_directory_content();
                            } else
                                invalid_query();
                        } else
                            invalid_query();
                    } else
                        invalid_query();
                } else
                    invalid_query();
            } else
                invalid_query();
        } else if (!wcscmp(link, cancel)) {  // In this subcommand the old link name is shortened.
            if (get_query_parameter(0) == CMD_EDIT_FOLDER) {  // Now after = new link name.
                change_directory();
                print_directory_content();
            } else
                invalid_query();
        } else
            invalid_query();
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_add_directory_conf()
{
    change_directory();
    if (!fatal_error) {
        Bookmarks::AddDirectory p;
        p.Render();
    }
}
//-------------------------------------------------------------------------------------------------

void do_add_directory()
{
    // Command should not be 0 here.
    TCHAR *link = (TCHAR *)wcschr(command, '=');
    if (link) {
        link++;
        if (!wcscmp(link, ok)) {
            if (get_query_parameter(0) == CMD_ADD_FOLDER) {  // Now after = new folder name.
                TCHAR *name = (TCHAR *)wcschr(command, '=');
                if (name) {
                    TCHAR *and_;
                    name++;
                    // The & symbol must be removed from the name.
                    if ((and_ = (TCHAR *)wcsrchr(name, '&')))
                        *and_ = 0;
                    // Transition to the folder and creating a directory.
                    change_directory();
                    if (!fatal_error) {
                        // Use repository pattern for directory creation
                        InitializeRepositories();
                        auto result = g_directoryRepo->CreateDirectory(name, L"");
                        if (result.success) {
                            print_directory_content();
                        } else {
                            DEBUG_LOG("Repository directory creation failed: " << narrow(result.message));
                            // Error handling - repository should not fail since it contains the same logic
                            invalid_query();
                        }
                    } else
                        invalid_query();
                } else
                    invalid_query();
            } else
                invalid_query();
        } else if (!wcscmp(link, cancel)) {
            if (get_query_parameter(0) == CMD_ADD_FOLDER) {  // Now after = new link name.
                change_directory();
                print_directory_content();
            } else
                invalid_query();
        } else
            invalid_query();
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------

void delete_directory(const std::wstring &directory_name)
{
    std::filesystem::path absolute_path = std::filesystem::absolute(directory_name);
    if (!std::filesystem::is_directory(absolute_path)) {
        throw two_msg_exception(L"Invalid directory: ", absolute_path.wstring());
    }

    try {
        std::filesystem::remove_all(absolute_path);
    } catch (const std::exception &e) {
        throw two_msg_exception(L"Failed to delete directory: " + directory_name, widen(e.what()));
    }
}
//-------------------------------------------------------------------------------------------------
// Deleting a folder.
void do_delete_directory()
{
    // Parsing the command.
    TCHAR *result = (TCHAR *)wcschr(command, '=');
    if (result) {
        if (!wcsncmp(result + 1, ok, std::wcslen(ok))) {
            if (get_query_parameter(0) == CMD_DEL_FOLDER) {  // Now after = folder name.
                change_directory();
                if (!fatal_error) {
                    // Getting directory name.
                    result = (TCHAR *)wcschr(command, '=');
                    if (result) {
                        result++;
                        // Use repository pattern for directory deletion
                        InitializeRepositories();
                        auto deleteResult = g_directoryRepo->DeleteDirectory(result);
                        if (deleteResult.success) {
                            print_directory_content();
                        } else {
                            DEBUG_LOG("Repository directory deletion failed: " << narrow(deleteResult.message));
                            // Error handling - repository should not fail since it contains the same logic
                            invalid_query();
                        }
                    }
                } else
                    invalid_query();

            } else
                invalid_query();
        } else {  // Displaying the contents of the current folder.
            do_change_directory();
        }
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------
// Request to delete a folder.
void do_delete_directory_conf()
{
    // Command should not be 0 here.
    TCHAR *directory_name = (TCHAR *)wcschr(command, '=');
    if (directory_name) {
        directory_name++;
        Bookmarks::DeleteDirectory p(directory_name);
        p.Render();
    } else
        invalid_query();
}
//-------------------------------------------------------------------------------------------------

void do_change_directory()
{
    change_directory();
    print_directory_content();
}
//-------------------------------------------------------------------------------------------------

void do_auto_folder()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Starting auto-organization process");
    
    // Auto-organize bookmarks using AI with comprehensive logging
    try {
        
        // Check username is set
        DEBUG_LOG("Checking authentication, username pointer: " << (username ? "NOT NULL" : "NULL"));
        if (!username) {
            DEBUG_LOG("ERROR: Username is not set!");
            DEBUG_LOG("Redirecting to login page due to missing authentication");
            // Redirect to login page instead of showing error
            do_login_conf();
            return;
        }
        if (username[0] == 0) {
            DEBUG_LOG("ERROR: Username is empty string!");
            DEBUG_LOG("Redirecting to login page due to empty username");
            // Redirect to login page instead of showing error
            do_login_conf();
            return;
        }
        DEBUG_LOG("Username: '" << narrow(username) << "'");
        
        // CRITICAL: Navigate to the current browsing directory (using same logic as change_directory())
        DEBUG_LOG("Navigating to current browsing directory using change_directory() logic");
        std::wstring userDir = get_user_directory_name();
        DEBUG_LOG("Base user directory: '" << narrow(userDir) << "'");
        
        // Ensure user directory exists
        if (!directory_exists(userDir)) {
            DEBUG_LOG("User directory doesn't exist, creating it");
            try {
                std::filesystem::create_directories(userDir);
                DEBUG_LOG("User directory created successfully");
            } catch (const std::exception &e) {
                DEBUG_LOG("Failed to create user directory: " << e.what());
#ifdef _WINDOWS
                const std::wstring message = widen(e.what(), CP_ACP);
#else
                const std::wstring message = widen(e.what());
#endif
                Bookmarks::AutoFolderStatus status(L"Failed to create user directory: " + message, false, L"", (query && query[0] != 0) ? query : L"");
                status.Render();
                return;
            }
        }
        
        // Build the target directory path with subdirectory (if any)
        std::wstring targetDir = userDir;
        if (query && query[0] != 0) {
            DEBUG_LOG("Query contains directory path: '" << narrow(query) << "'");
            targetDir = concatenate_paths(targetDir, query);
        } else {
            DEBUG_LOG("No subdirectory specified, using user root directory");
        }
        
#ifdef _WINDOWS
        std::replace(targetDir.begin(), targetDir.end(), '/', '\\');
#endif
        DEBUG_LOG("Target directory (with subdirectory): '" << narrow(targetDir) << "'");
        
        // Check if directory exists before attempting to change
        try {
            if (!std::filesystem::exists(targetDir)) {
                DEBUG_LOG("ERROR: Target directory does not exist!");
                Bookmarks::AutoFolderStatus status(L"Target directory does not exist: " + targetDir, false, L"", (query && query[0] != 0) ? query : L"");
                status.Render();
                return;
            }
            
            if (!std::filesystem::is_directory(targetDir)) {
                DEBUG_LOG("ERROR: Target path is not a directory!");
                Bookmarks::AutoFolderStatus status(L"Target path is not a directory: " + targetDir, false, L"", (query && query[0] != 0) ? query : L"");
                status.Render();
                return;
            }
            
            DEBUG_LOG("Directory exists and is valid, attempting to change");
        } catch (const std::filesystem::filesystem_error& e) {
            DEBUG_LOG("Filesystem error checking directory: " << e.what());
            Bookmarks::AutoFolderStatus status(L"Filesystem error: " + widen(e.what()), false, L"", (query && query[0] != 0) ? query : L"");
            status.Render();
            return;
        }
        
        if (!change_working_directory(targetDir)) {
            DEBUG_LOG("ERROR: Failed to change to target directory!");
            Bookmarks::AutoFolderStatus status(L"Failed to access bookmark directory", false, L"", (query && query[0] != 0) ? query : L"");
            status.Render();
            return;
        }
        
        // CRITICAL: Update global cwd variable after changing directory
        cwd = get_current_working_directory();
        DEBUG_LOG("Changed working directory to: " << narrow(cwd));
        DEBUG_LOG("Updated global cwd variable: " << narrow(cwd));
        
        // Debug environment variables
        std::wstring documentRoot = get_wide_environment_variable(_T("DOCUMENT_ROOT"));
        DEBUG_LOG("DOCUMENT_ROOT: '" << narrow(documentRoot) << "'");
        
        // Create Data object with error handling
        DEBUG_LOG("Creating Data object");
        DEBUG_LOG("Current working directory before Data creation: " << narrow(get_current_working_directory()));
#ifdef _WINDOWS
        Bookmarks::Data data(1251);  // Windows-1251 code page
        DEBUG_LOG("Using Windows-1251 encoding");
#else
        Bookmarks::Data data;  // Default constructor
        DEBUG_LOG("Using default encoding");
#endif
        DEBUG_LOG("Data object created successfully");
        DEBUG_LOG("Current working directory after Data creation: " << narrow(get_current_working_directory()));
        
        // Use local Ollama installation with environment detection
        std::string ollamaUrl = "http://localhost:11435";
        
        // Check if running in Docker container (simple detection)
        std::ifstream dockerenv("/.dockerenv");
        if (dockerenv.good()) {
            ollamaUrl = "http://host.docker.internal:11435";
            DEBUG_LOG("Docker container detected, using: " << ollamaUrl);
        } else {
            DEBUG_LOG("Native environment detected, using: " << ollamaUrl);
        }
        dockerenv.close();
        
        // Initialize AI service with logging
        DEBUG_LOG("Initializing Ollama service");
        DEBUG_LOG("Using URL: '" << ollamaUrl << "'");
        DEBUG_LOG("URL length: " << ollamaUrl.length());
        
        DEBUG_LOG("About to create OllamaService with URL: '" << ollamaUrl << "'");
        
        Bookmarks::OllamaService aiService(ollamaUrl, "llama3.2:latest", 120);
        DEBUG_LOG("Ollama service created successfully");
        
        // Test AI service availability
        DEBUG_LOG("Testing AI service availability");
        bool aiAvailable = false;
        try {
            aiAvailable = aiService.isAvailable();
            DEBUG_LOG("AI service availability check completed: " << (aiAvailable ? "YES" : "NO"));
        } catch (const std::exception& e) {
            DEBUG_LOG("Exception during AI availability check: " << e.what());
            throw;
        }
        
        // Initialize reorganizer
        DEBUG_LOG("Initializing bookmark reorganizer");
        Bookmarks::BookmarkReorganizer reorganizer(&data, &aiService);
        
        // Set repositories for the reorganizer
        InitializeRepositories();
        reorganizer.SetRepositories(g_bookmarkRepo, g_directoryRepo);
        DEBUG_LOG("Bookmark reorganizer created and repositories set successfully");
        
        // Check if reorganization is possible with detailed logging
        DEBUG_LOG("Checking reorganization prerequisites");
        bool canReorg = false;
        try {
            canReorg = reorganizer.canReorganize();
            DEBUG_LOG("Prerequisites check completed: " << (canReorg ? "YES" : "NO"));
        } catch (const std::exception& e) {
            DEBUG_LOG("Exception during prerequisites check: " << e.what());
            throw;
        }
        
        if (!canReorg) {
            std::wstring statusMsg = reorganizer.getLastStatusMessage();
            DEBUG_LOG("Reorganization failed - " << narrow(statusMsg));
            
            // Show error page with status message
            std::wstring currentDirectory = (query && query[0] != 0) ? query : L"";
            Bookmarks::AutoFolderStatus status(statusMsg, false, L"", currentDirectory);
            status.Render();
            return;
        }
        
        DEBUG_LOG("Prerequisites passed, starting reorganization");
        bool success = reorganizer.reorganizeBookmarks();
        
        std::wstring statusMsg = reorganizer.getLastStatusMessage();
        DEBUG_LOG("Reorganization completed: " << (success ? "SUCCESS" : "FAILED"));
        DEBUG_LOG("Status: " << narrow(statusMsg));
        
        // Show status page with result and operation summary if successful
        std::wstring currentDirectory = (query && query[0] != 0) ? query : L"";
        DEBUG_LOG("Current directory for redirect: '" << narrow(currentDirectory) << "'");
        
        if (success) {
            std::wstring operationSummary = reorganizer.getOperationSummary();
            DEBUG_LOG("Generated operation summary with " << operationSummary.length() << " characters");
            Bookmarks::AutoFolderStatus status(statusMsg, success, operationSummary, currentDirectory);
            status.Render();
        } else {
            Bookmarks::AutoFolderStatus status(statusMsg, success, L"", currentDirectory);
            status.Render();
        }
        
    } catch (const std::exception& e) {
        std::string errorStr = e.what();
        std::wstring errorMsg = L"Auto-organization error: " + widen(errorStr);
        DEBUG_LOG("Exception caught: " << errorStr);
        DEBUG_LOG("Exception type: std::exception");
        DEBUG_LOG("Full exception details: " << typeid(e).name() << " - " << errorStr);
        
        std::wstring currentDirectory = (query && query[0] != 0) ? query : L"";
        Bookmarks::AutoFolderStatus status(errorMsg, false, L"", currentDirectory);
        status.Render();
    } catch (...) {
        DEBUG_LOG("Unknown exception caught");
        std::wstring errorMsg = L"Auto-organization error: Unknown exception occurred";
        
        std::wstring currentDirectory = (query && query[0] != 0) ? query : L"";
        Bookmarks::AutoFolderStatus status(errorMsg, false, L"", currentDirectory);
        status.Render();
    }
    
    DEBUG_LOG_FUNC_EXIT();
}
//-------------------------------------------------------------------------------------------------

int get_command_code(TCHAR *command /*the command without ;*/)
{
    int result;
    if (!wcsncmp(command, cmd_edit + 1, std::wcslen(cmd_edit) - 1)) {
        result = CMD_EDIT;
    } else if (!wcsncmp(command, cmd_edit_conf + 1, std::wcslen(cmd_edit_conf) - 1)) {
        result = CMD_EDIT_CONF;
    } else if (!wcsncmp(command, cmd_add + 1, std::wcslen(cmd_add) - 1)) {
        result = CMD_ADD;
    } else if (!wcsncmp(command, cmd_log_in + 1, std::wcslen(cmd_log_in) - 1)) {
        result = CMD_LOG_IN;
    } else if (!wcsncmp(command, cmd_add_conf + 1, std::wcslen(cmd_add_conf) - 1)) {
        result = CMD_ADD_CONF;
    } else if (!wcsncmp(command, cmd_delete_conf + 1, std::wcslen(cmd_delete_conf) - 1)) {
        result = CMD_DEL_CONF;
    } else if (!wcsncmp(command, cmd_del + 1, std::wcslen(cmd_del) - 1)) {
        result = CMD_DEL;
    } else if (!wcsncmp(command, cmd_edit_directory + 1, std::wcslen(cmd_edit_directory) - 1)) {
        result = CMD_EDIT_FOLDER;
    } else if (!wcsncmp(command, cmd_edit_directory_conf + 1,
                        std::wcslen(cmd_edit_directory_conf) - 1)) {
        result = CMD_EDIT_FOLDER_CONF;
    } else if (!wcsncmp(command, cmd_add_directory + 1, std::wcslen(cmd_add_directory) - 1)) {
        result = CMD_ADD_FOLDER;
    } else if (!wcsncmp(command, cmd_add_directory_conf + 1,
                        std::wcslen(cmd_add_directory_conf) - 1)) {
        result = CMD_ADD_FOLDER_CONF;
    } else if (!wcsncmp(command, cmd_del_directory_conf + 1,
                        std::wcslen(cmd_del_directory_conf) - 1)) {
        result = CMD_DEL_FOLDER_CONF;
    } else if (!wcsncmp(command, cmd_ch_folder + 1, std::wcslen(cmd_ch_folder) - 1)) {
        result = CMD_CHANGE_FOLDER;
    } else if (!wcsncmp(command, cmd_del_directory + 1, std::wcslen(cmd_del_directory) - 1)) {
        result = CMD_DEL_FOLDER;
    } else if (!wcsncmp(command, cmd_auto_folder + 1, std::wcslen(cmd_auto_folder) - 1)) {
        result = CMD_AUTO_FOLDER;
    } else if (!wcsncmp(command, cmd_key + 1, std::wcslen(cmd_key) - 1)) {
        result = CMD_KEY;
    } else {
        invalid_query();
        result = CMD_UNKNOWN;
    }

    return result;
}
//-------------------------------------------------------------------------------------------------
// The parsed command is removed from the query string.
int get_query_parameter(unsigned char preserve_parameter /* flag that the request is transmitted in its original form -
                                                            prohibition of command removal from the string */)
{
    // Searching ';' in HTML-encoded query.
    // TODO: What is "-3"?
    command = query + std::wcslen(query) - 3;
    for (; command >= query; command--) {
        // Skips HTML-encoded ';'.
        if ((command[0] == '%') && (command[1] == '3') &&
            ((command[2] == 'B') || (command[2] == 'b'))) {
            if (!preserve_parameter)
                // Command part is cut off from the query.
                (*command) = 0;

            command += 3;
            // If there is %, then the request was not processed.
            return get_command_code(command);
        }
    }

    command = (TCHAR *)wcsrchr(query, ';');
    if (!command)
        return CMD_LOG_IN_CONF;
    else {
        if (!preserve_parameter)
            // Command part is cut off from the query.
            (*command) = 0;

        command++;
        return get_command_code(command);
    }

    // Command not found. Return to the login page.
    return CMD_LOG_IN_CONF;
}
//-------------------------------------------------------------------------------------------------

std::wstring get_full_directory_path(const std::wstring &relative_directory_path)
{
    std::wstring result = concatenate_paths(document_root, relative_directory_path);

#ifdef _WINDOWS
    std::replace(result.begin(), result.end(), '/', '\\');
#endif
    return result;
}
//-------------------------------------------------------------------------------------------------
// Returns path to user content folder.
std::wstring get_user_directory_name()
{
    std::wstring result = get_full_directory_path(_T("users/links/"));
    result += username;

#ifdef _WINDOWS
    std::replace(result.begin(), result.end(), '/', '\\');
#endif
    return result;
}
//-------------------------------------------------------------------------------------------------

std::wstring get_temp_directory_path() { return get_full_directory_path(tmp_dir); }
//-------------------------------------------------------------------------------------------------
// Transition to the required folder (relative to the specified root folder).
void change_directory()
{
    if (!username) {
        DEBUG_LOG("No username set, redirecting to login");
        do_login_conf();
        throw auth_handled_exception();
    } else {
        // A folder name string is created.
        std::wstring user_directory_path = get_user_directory_name();
        
        // Ensure user directory exists, create if necessary
        DEBUG_LOG("Checking if user directory exists: '" << narrow(user_directory_path) << "'");
        if (!directory_exists(user_directory_path)) {
            DEBUG_LOG("User directory doesn't exist, creating it");
            try {
                std::filesystem::create_directories(user_directory_path);
                DEBUG_LOG("User directory created successfully");
            } catch (const std::exception &e) {
                DEBUG_LOG("Failed to create user directory: " << e.what());
#ifdef _WINDOWS
                const std::wstring message = widen(e.what(), CP_ACP);
#else
                const std::wstring message = widen(e.what());
#endif
                system_error(message);
            }
        } else {
            DEBUG_LOG("User directory already exists");
        }
        
        std::wstring dest_directory_path = user_directory_path;
        // Query must contain now the relative directory path.
        dest_directory_path = concatenate_paths(dest_directory_path, query);
#ifdef _WINDOWS
        std::replace(dest_directory_path.begin(), dest_directory_path.end(), '/', '\\');
#endif
        // Changes current directory.
        if (!change_working_directory(dest_directory_path)) {
            fatal_error = 1;
            error       = E_CHANGE_FOLDER;
            throw two_msg_exception(err_change_directory, dest_directory_path.c_str());
        }

        cwd = get_current_working_directory();
    }
}
//-------------------------------------------------------------------------------------------------

void get_key()
{
    TCHAR *saved_command = command;         // Saving.
    if (CMD_KEY == get_query_parameter(0))  // Removing the key from the request.
    {
        FILE *f;

        key = (TCHAR *)wcsrchr(command, '=');
        key++;
        
        // Check if key is empty - if so, redirect to login page
        if (!key || !*key || std::wcslen(key) == 0) {
            DEBUG_LOG("Empty key detected, redirecting to login");
            command = saved_command;
            do_login_conf();
            throw auth_handled_exception();
        }
        
        // Reading session key file.
        std::wstring key_file_path = get_temp_directory_path();
        key_file_path += key;

        if (!std::filesystem::exists(key_file_path)) {
            DEBUG_LOG("Session key file doesn't exist, redirecting to login: " << narrow(key_file_path));
            command = saved_command;
            do_login_conf();
            throw auth_handled_exception();
        }

#ifdef _WINDOWS
        f = _wfopen(key_file_path.c_str(), _T("r"));
#else
        f = std::fopen(narrow(key_file_path).c_str(), "r");
#endif
        if (!f)
            no_permission(key_file_path);

        try {
            username = (TCHAR *)malloc(MAX_USER_NAME);
            if (username) {
                int count = 0;
                while (1) {
                    int c = fgetc(f);
                    if ((c == '\r') || (c == '\n') || (count == MAX_USER_NAME - 1) || (c == EOF))
                        break;
                    username[count++] = (char)c;
                }
                username[count] = 0;
                // TODO: Reading and checking the key validity time.
            } else
                out_of_memory();
        } catch (std::exception &) {
            fclose(f);
            throw;
        }

        fclose(f);
    } else
        invalid_query();
    command = saved_command;
}
//-------------------------------------------------------------------------------------------------

void make_directory(const std::wstring &dirName)
{
    // TODO: Check returning value.
#ifdef _WINDOWS
    // Need encoding in 1251 (CP_ACP corresponds to it, encoding can be specified directly by a numeric value).
    char dirNameMultibyte[MAX_LINE_LENGTH];
    WideCharToMultiByte(CP_ACP, 0, dirName.c_str(), -1, dirNameMultibyte, sizeof(dirNameMultibyte),
                        NULL, NULL);
    // The function works with "narrow" characters.
    _mkdir(dirNameMultibyte);
#else
    mkdir(narrow(dirName).c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
}
//-------------------------------------------------------------------------------------------------

void pepare_query_buffer(size_t queryLen)
{
    size_t bufSize = (queryLen + 1) * sizeof(TCHAR);
    query          = (TCHAR *)malloc(bufSize);
    if (query) {
        // Fill by terminating zeros.
        memset(query, 0, bufSize);
    }
}
//-------------------------------------------------------------------------------------------------

#define MAX_DOMAIN 100
int handle_query(const TCHAR *encodedQuery)
{
    pepare_query_buffer(std::wcslen(encodedQuery));
    wcscpy(query, encodedQuery);

#ifdef _WINDOWS
    // Should be unsed only for debug purposes under IIS.
    // TODO: Configure IIS to use HTTPS.
    std::wstring http = L"http://";
#else
    // Set up for Apache.
    std::wstring http = L"https://";
#endif

    document_root = get_wide_environment_variable(_T("DOCUMENT_ROOT"));
    if (document_root.empty()) {
        no_environment(_T("DOCUMENT_ROOT"));
        return 1;
    }
    // Instead of an environment variable, use the domain name from the config file to correctly set links when the server is running through SSH.
    // evernote:///view/14501366/s132/44d4835c-cdac-40e1-acff-2fe610f865c8/44d4835c-cdac-40e1-acff-2fe610f865c8/
    // On Apache the environment variable is provided.
    server_name = get_wide_environment_variable(_T("SERVER_NAME"));
    if (server_name.empty()) {
        no_environment(_T("SERVER_NAME"));
        return 1;
    }

    script_name = get_wide_environment_variable(_T("SCRIPT_NAME"));
    if (script_name.empty()) {
        no_environment(_T("SCRIPT_NAME"));
        return 1;
    }

    if (!fatal_error) {
        // The script name is prepared for creating links.
        full_script_name = http + server_name + L":" + server_port + script_name;
        // TODO: Move this into configuration.
#ifdef _WINDOWS
        // This is for current IIS configuration.
        const std::wstring image_dir = L"/legacy-images/";
        server_root                  = substring_before_last(full_script_name, L'/');
#else
        // This is for Apache configuration.
        const std::wstring image_dir = L"/links/legacy-images/";
        server_root                  = http + server_name + L":" + server_port;
#endif
        images_root = server_root + image_dir;
    }

    // The body of the page is formed here.
    if (!fatal_error) {
        cwd = get_current_working_directory();
        // Prohibition of inserting 0 is necessary so that
        // process_query can process the entire request;
        // A request by pressing a button can be distinguished from
        // transition by link by setting the post method
        // (now get is used everywhere), or by the command code
        // (certain commands are associated with pressing the button);
        // However, this requires searching for the command
        // on the unprocessed request string;
        // If the request is received as a result of following the link,
        // then you need to call process_query(0).
        switch (get_query_parameter(1)) {
        default:
        case (CMD_LOG_IN_CONF):
            process_query(0);
            if (!fatal_error)
                do_login_conf();
            break;

        case (CMD_LOG_IN):
            process_query(1);
            if (!fatal_error) {
                do_login();
            }
            break;

        case (CMD_CHANGE_FOLDER):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_change_directory();
            }
            break;

        case (CMD_EDIT):
            process_query(1);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_edit();
            }
            break;

        case (CMD_EDIT_CONF):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_edit_conf();
            }
            break;

        case (CMD_ADD):
            process_query(1);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_add_link();
            }
            break;

        case (CMD_ADD_CONF):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_add_link_conf();
            }
            break;

        case (CMD_DEL):
            process_query(1);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_delete_link();
            }
            break;

        case (CMD_DEL_CONF):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_delete_link_conf();
            }
            break;

        case (CMD_EDIT_FOLDER):
            process_query(1);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_edit_directory();
            }
            break;

        case (CMD_EDIT_FOLDER_CONF):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_edit_directory_conf();
            }
            break;

        case (CMD_ADD_FOLDER):
            process_query(1);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_add_directory();
            }
            break;

        case (CMD_ADD_FOLDER_CONF):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_add_directory_conf();
            }
            break;

        case (CMD_DEL_FOLDER):
            process_query(1);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_delete_directory();
            }
            break;

        case (CMD_DEL_FOLDER_CONF):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_delete_directory_conf();
            }
            break;

        case (CMD_AUTO_FOLDER):
            process_query(0);
            if (!fatal_error) {
                get_query_parameter(0);  // Removing the command from the request.
                get_key();
                do_auto_folder();
            }
            break;
        }  // switch(get_query_command)
    }

    if (query) {
        free(query);  // After process_query can be freed.
        query = 0;
    }

    return 0;
}
