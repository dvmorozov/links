#include "App/stdafx.h"

#include "FileListLegacy.h"
#include "Shared/Controllers/Commands.h"
#include "../Models/LinkFileReader.h"
#include "Utils/Miscellaneous.h"
#include "Shared/Repositories/RepositoryFactory.h"
#include "Utils/DebugLog.h"

namespace Bookmarks
{
FileListLegacy::FileListLegacy()
{
    _cssClass = L"window";
}

void FileListLegacy::SetRepositories(std::shared_ptr<IBookmarkRepository> bookmarkRepo, 
                                    std::shared_ptr<IDirectoryRepository> directoryRepo)
{
    _bookmarkRepo = bookmarkRepo;
    _directoryRepo = directoryRepo;
}

std::wstring FileListLegacy::GetImagePath(std::wstring imageFile)
{
    if (images_root.empty())
        return L"";
    return images_root + imageFile;
}

void FileListLegacy::InsertButton(const std::wstring &imageFile, const std::wstring &endpointUrl,
                                  const std::wstring &endpointParameters, int size,
                                  const std::wstring &altPrefix)
{
    const std::wstring imagePath = GetImagePath(imageFile);

    if (!imagePath.empty()) {
        if (!endpointUrl.empty()) {
            // A button with a link is inserted.
            // The endpointParameters is used to distinguish
            // between endpoint URL and external resource URL.
            if (!endpointParameters.empty()) {
                // Content of endpointParameters contains parameters to the endpoint.
                wprintf(L"<a href=\"%ls?%ls\">\n"
                        "<img src=\"%ls\" align=\"absmiddle\" border=\"0\" alt=\"%ls\" "
                        "width=\"%i\" height=\"%i\"></a>\n",
                        endpointUrl.c_str(), endpointParameters.c_str(), imagePath.c_str(),
                        altPrefix.c_str(), size, size);
            } else {
                // A bookmark button with a link to an external resource is inserted.
                // The endpointUrl parameter contains the URL of the external resource.
                // The altPrefix parameter contains the domain of the external resource.
                wprintf(L"<a href=\"%ls\">\n"
                        "<img src=\"%ls\" align=\"absmiddle\" border=\"0\" alt=\"%ls\" "
                        "width=\"%i\" height=\"%i\"></a>\n",
                        endpointUrl.c_str(), imagePath.c_str(), altPrefix.c_str(), size, size);
            }
        } else {  // A button without a link is inserted. For example, an error message.
            // The endpointParameters parameter is used to pass additional information.
            if (!endpointParameters.empty()) {
                // Content of endpointParameters is added to the alt prefix.
                wprintf(L"<a href=\"#\">\n"
                        "<img src=\"%ls\" align=\"absmiddle\" border=\"0\" alt=\"%ls%ls\" "
                        "width=\"%i\" height=\"%i\"></a>\n",
                        imagePath.c_str(), altPrefix.c_str(), endpointParameters.c_str(), size,
                        size);
            } else {
                wprintf(L"<a href=\"#\">\n"
                        "<img src=\"%ls\" align=\"absmiddle\" border=\"0\" alt=\"%ls\" "
                        "width=\"%i\" height=\"%i\"></a>\n",
                        imagePath.c_str(), altPrefix.c_str(), size, size);
            }
        }
    }
}

void FileListLegacy::InsertTableCommandButton(TCHAR *cmd, const TCHAR *dir,
                                              const TCHAR *fileOrButtonName, const TCHAR *imageFile,
                                              const TCHAR *altPrefix)
{
    // Internal div is used to fix the size of the button.
    wprintf(L"<td><div class=\"image_button\">\n");
    InsertCommandButton(cmd, dir, fileOrButtonName, imageFile, altPrefix);
    wprintf(L"</div></td>\n");
}

void FileListLegacy::InsertTableLinkButton(const std::wstring &imageFile,
                                           const std::wstring &endpointUrl,
                                           const std::wstring &endpointParameters, int size,
                                           const std::wstring &altPrefix)
{
    // Internal div is used to fix the size of the button.
    wprintf(L"<td><div class=\"image_button link_button\">\n");
    InsertButton(imageFile, endpointUrl, endpointParameters, 16, altPrefix);
    wprintf(L"</div></td>\n");
}

// TODO: Use std::wstring instead of TCHAR*.
void FileListLegacy::InsertCommandButton(TCHAR *cmd, const TCHAR *dir, const TCHAR *file,
                                         const TCHAR *imageFile, const TCHAR *altPrefix)
{
    InsertButton(imageFile, full_script_name, GetCommandUrl(cmd, dir, file), 16,
                 altPrefix ? altPrefix : L"");
}

// Calls repository pattern to read the list of directories.
std::vector<std::wstring> FileListLegacy::ReadFolders()
{
    std::vector<std::wstring> result;
    
    // Add navigation entries when in a subdirectory (when query is not empty)
    if (std::wcslen(query) > 0) {
        result.push_back(L".");   // Home button - go to root
        result.push_back(L"..");  // Up button - go to parent directory
    }
    
    // Use repository pattern - should always be available since it contains the same logic
    if (_directoryRepo) {
        auto directories = _directoryRepo->ListDirectories();
        for (const auto& dir : directories) {
            result.push_back(dir->name);
        }
        DEBUG_LOG("Successfully loaded " << directories.size() << " directories + " << (std::wcslen(query) > 0 ? 2 : 0) << " navigation entries using repository pattern");
    } else {
        DEBUG_LOG("Directory repository not available - this should not happen");
        // Repository should always be set, but handle gracefully
    }

    // Sorts the vector alphabetically, but keep navigation entries at the top
    if (std::wcslen(query) > 0) {
        // Sort only the actual directories (skip the first 2 navigation entries)
        std::sort(result.begin() + 2, result.end());
    } else {
        // Sort all entries when no navigation entries are present
        std::sort(result.begin(), result.end());
    }

    return result;
}

FileVector FileListLegacy::ReadFiles()
{
    FileVector files;
    
    // Use repository pattern - should always be available since it contains the same logic
    if (_bookmarkRepo) {
        auto bookmarks = _bookmarkRepo->ListBookmarks();
        // Convert repository BookmarkInfo to legacy File structure
        for (const auto& bookmark : bookmarks) {
            File file;
            file.FileName = bookmark->fileName;
            file.Name = bookmark->title.empty() ? bookmark->fileName : bookmark->title;
            file.Url = bookmark->url;
            file.IsFolder = false;
            files.push_back(file);
        }
        DEBUG_LOG("Successfully loaded " << files.size() << " bookmarks using repository pattern");
    } else {
        DEBUG_LOG("Bookmark repository not available - this should not happen");
        // Repository should always be set, but handle gracefully
    }

    // Sort the files vector by the FileName attribute in alphabetical order.
    std::sort(files.begin(), files.end(),
              [](const File &a, const File &b) { return a.Name < b.Name; });

    return files;
}

void FileListLegacy::InsertAddButtons()
{
    wprintf(_T("<div class=\"buttons\">\n"));
    InsertCommandButton(cmd_add_conf, query, L"", _T("add_link.bmp"), _hintAddLink.c_str());
    InsertCommandButton(cmd_add_directory_conf, query, L"", _T("add_folder.bmp"),
                        _hintAddFolder.c_str());
    InsertCommandButton(cmd_auto_folder, query, L"", _T("folder1.bmp"), L"Auto-organize with AI");
    wprintf(_T("\n</div>\n"));
}

void FileListLegacy::OpenInnerTable(const std::wstring &subclass)
{
    // Opens new cell for internal table.
    wprintf(L"\n<td class=\"inner %ls\" valign=\"top\">\n\
    <table cellspacing=\"10\" cellpadding=\"0\">\n\
<tbody>\n",
            subclass.c_str());
}

void FileListLegacy::CloseInnerTable()
{
    wprintf(L"</tbody>\n\
    </table>\n\
</td>\n");
}

void FileListLegacy::OpenInnerFileTableRow(std::wstring endpointParameters) { wprintf(L"<tr>\n"); }

void FileListLegacy::OpenInnerDirTableRow(std::wstring endpointParameters) { wprintf(L"<tr>\n"); }

// Closes the cell and row of the internal table.
void FileListLegacy::CloseInnerTableRow() { wprintf(L"</tr>"); }

void FileListLegacy::OpenOuterTable()
{
    // Creates outer table with 2 columns for folders and files.
    // Adds new row for internal tables.
    wprintf(_T("<div>\n\
<table class=\"outer\" cellspacing=\"0\" cellpadding=\"10\">\n\
    <tbody>\n\
        <tr>\n\
            <th>%ls\n\
            </th>\n\
            <th>%ls [%ls]\n\
            </th>\n\
        </tr>\n<tr>\n"),
            _hintDirs.c_str(), _hintLinks.c_str(), query);
}

void FileListLegacy::CloseOuterTable()
{
    // For the first closes internal table row.
    wprintf(_T("</tr>\n\
    </tbody>\n\
</table>\n\
</div>\n"));
}

void FileListLegacy::PrintDirList(std::vector<std::wstring> dirs)
{
    for (auto dir = dirs.begin(); dir != dirs.end(); ++dir) {
        if (dir->size()) {
            if (*dir == _T(".")) {  // Transition to the root folder of bookmarks.
                if (std::wcslen(query)) {
                    OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder,
                                                           L"" /* endpointParameters */,
                                                           ok /* what after command */));
                    InsertTableCommandButton(cmd_ch_folder, L"" /* endpointParameters */,
                                             ok /* what after command */,
                                             _T("to_start_page.bmp"), _hintFolder.c_str());
                    wprintf(_T("<td colspan='3'>%ls</td>"), _hintHome.c_str());
                    CloseInnerTableRow();
                }
            } else if (*dir ==
                       _T("..")) {  // Transition one level up or to the root folder of bookmarks.
                if (std::wcslen(query)) {
                    std::wstring upDir = query;
                    // Get the name of the upper folder.
                    auto slashPos = upDir.rfind(_T("/"));
                    upDir = slashPos != std::string::npos ? upDir.substr(0, slashPos) : upDir;

                    OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, upDir.c_str(),
                                                           ok /* what after command */));
                    // Transition to the top level of the directory (even if the top folder is the root).
                    InsertTableCommandButton(cmd_ch_folder, upDir.c_str(),
                                             ok /* what after command */,
                                             _T("to_upper_folder.bmp"), _hintFolder.c_str());
                    wprintf(_T("<td colspan='3'>%ls</td>"), _hintUp.c_str());

                    CloseInnerTableRow();
                    /* Transition to the root folder is already inserted above */
                }
            } else
            // If the current query is not ".", ".." or 0, then
            // the query string contains the name of the current folder, which needs to be
            // added to the names of the found files so that
            // the subsequent query transitions to the desired folder.
            {
                // dirPath contains the relative path to the directory for creating links.
                std::wstring dirPath = query;
                dirPath += _T("/");
                dirPath += *dir;
                OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, dirPath.c_str(), ok));

                // Creates button for changing directory.
                InsertTableCommandButton(cmd_ch_folder, dirPath.c_str(), ok, _T("folder.bmp"),
                                         _hintFolder.c_str());

                // Prints directory name.
                wprintf(_T("<td class=\"name\">%ls</td>"), dir->c_str());

                const std::wstring upperDirPath = query;
                // Creates button for deleting directory.
                InsertTableCommandButton(cmd_del_directory_conf, upperDirPath.c_str(), dir->c_str(),
                                         _T("delete_folder.bmp"), HintDelete.c_str());
                // Creates button for editing directory name.
                InsertTableCommandButton(cmd_edit_directory_conf, upperDirPath.c_str(),
                                         dir->c_str(), _T("edit_folder.bmp"), HintEdit.c_str());
                CloseInnerTableRow();
            }
        }
    }
}

void FileListLegacy::PrintFileList(const FileVector &files)
{
    for (const auto &file : files)
        PrintFileRow(file);
}

void FileListLegacy::PrintFolders()
{
    OpenInnerTable(L"left");
    PrintDirList(ReadFolders());
    CloseInnerTable();
}

void FileListLegacy::PrintFiles()
{
    OpenInnerTable(L"right");
    PrintFileList(ReadFiles());
    CloseInnerTable();
}

// Renders file name without extension.
std::wstring FileListLegacy::GetFileNameWithoutExt(std::wstring fileName) const
{
    auto         pointPos = fileName.rfind(_T("."));
    std::wstring fileNameWithoutExt =
        pointPos != std::string::npos ? fileName.substr(0, pointPos) : fileName;
    return fileNameWithoutExt;
}

std::wstring FileListLegacy::GetLinkName(const File &file) const
{
    std::wstring linkName = GetFileNameWithoutExt(file.FileName);
#ifdef EXTENDED_URL_FILE
    if (!file.Name.empty())
        linkName = file.Name;
    else
        linkName = extract_domain(file.Url);
#endif
    return linkName;
}

// Renders row with link.
void FileListLegacy::PrintFileRow(const File &file)
{
    OpenInnerFileTableRow();

    if (!file.Url.empty()) {  // Inserts link button. The domain is added to a hint.
        const std::wstring domainHint = _hintDomain + extract_domain(file.Url);
        // TODO: Rework this parameter manipulation to be clear.
        // The endpointParameters is used to distinguish between endpoint URL
        // and external resource URL. It must be empty to create a link to an
        // external resource.
        InsertTableLinkButton(L"link.bmp", file.Url, L"", 16, domainHint);
    } else
        // TODO: Just a simple icon here, not a link.
        InsertTableLinkButton(L"error.bmp", L"", _hintFileReadingFailed, 16, _hintError);


    // Renders link name.
    wprintf(_T("<td class=\"name\">%ls</td>\n"), GetLinkName(file).c_str());        

    // Inserts operation buttons.
    InsertTableCommandButton(cmd_delete_conf, query, file.FileName.c_str(), _T("delete_link.bmp"),
                             HintDelete.c_str());
    InsertTableCommandButton(cmd_edit_conf, query, file.FileName.c_str(), _T("edit_link.bmp"),
                             HintEdit.c_str());
    CloseInnerTableRow();
}

// When called, the page header has already been output, so
// need to do the work as much as possible.
void FileListLegacy::Render()
{
    PrintHead(_hintFavoriteLinks);
    OpenOuterTable();
    PrintFolders();
    PrintFiles();
    // Closes the row of the outer table and the outer table.
    CloseOuterTable();
    InsertAddButtons();

    PrintInfo();
    PrintTail();
}
}  // namespace Bookmarks
