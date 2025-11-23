
#pragma once

#include "App/Globals.h"
#include "Shared/UI/Page.h"
#include "Utils/Miscellaneous.h"

// Repository Pattern Support
#include "Shared/Repositories/IBookmarkRepository.h"
#include "Shared/Repositories/IDirectoryRepository.h"

namespace Bookmarks
{
class FileListLegacy : public Page
{
private:
    //  TODO: double check if widen is actually necessary for proper encoding of
    //  HTML output.
    //  TODO: move texts to the resource file.
    const std::wstring _hintFolder            = widen("FOLDER");
    const std::wstring _hintHome              = widen("HOME");
    const std::wstring _hintDomain            = widen("Domain: ");
    const std::wstring _hintLinks             = widen("Links");
    const std::wstring _hintDirs              = widen("Folders");
    const std::wstring _hintAddLink           = widen("Create Link");
    const std::wstring _hintAddFolder         = widen("Create Folder");
    const std::wstring _hintError             = widen("Error: ");
    const std::wstring _hintFileReadingFailed = widen("Cannot read URL from file!");
    const std::wstring _hintUp                = widen("UP");
    const std::wstring _hintFavoriteLinks     = widen("Favorite Links");

    void                      PrintFolders();
    void                      PrintFiles();
    void                      PrintFileList(const FileVector &files);
    void                      PrintDirList(std::vector<std::wstring> dirs);
    std::vector<std::wstring> ReadFolders();
    FileVector                ReadFiles();
    void                      InsertAddButtons();

    // Repository Pattern - Repository instances for modernized data access
    std::shared_ptr<IBookmarkRepository> _bookmarkRepo;
    std::shared_ptr<IDirectoryRepository> _directoryRepo;

public:
    void SetRepositories(std::shared_ptr<IBookmarkRepository> bookmarkRepo, 
                        std::shared_ptr<IDirectoryRepository> directoryRepo);

protected:
    const std::wstring HintDelete = widen("Delete");
    const std::wstring HintEdit   = widen("Edit");

    virtual std::wstring GetImagePath(std::wstring imageFile);
    virtual void InsertTableCommandButton(TCHAR *cmd, const TCHAR *dir, const TCHAR *file,
                                          const TCHAR *imageFile, const TCHAR *altPrefix);
    virtual void InsertCommandButton(TCHAR *cmd, const TCHAR *dir, const TCHAR *file,
                                     const TCHAR *imageFile, const TCHAR *altPrefix);
    virtual void InsertTableLinkButton(const std::wstring &imageFile,
                                       const std::wstring &endpointUrl,
                                       const std::wstring &endpointParameters, int size,
                                       const std::wstring &altPrefix);
    virtual void InsertButton(const std::wstring &imageFile, const std::wstring &endpointUrl,
                              const std::wstring &endpointParameters, int size,
                              const std::wstring &altPrefix);

    virtual void OpenOuterTable();
    virtual void CloseOuterTable();

    virtual void OpenInnerTable(const std::wstring &subclass);
    virtual void CloseInnerTable();

    virtual void OpenInnerFileTableRow(std::wstring endpointParameters = L"#");
    virtual void OpenInnerDirTableRow(std::wstring endpointParameters = L"#");
    virtual void CloseInnerTableRow();

    //  Renders a row with a link.
    virtual void PrintFileRow(const File &file);

    std::wstring GetFileNameWithoutExt(std::wstring fileName) const;
    std::wstring GetLinkName(const File &file) const;

public:
    FileListLegacy();

    virtual void Render();
};
}  // namespace Bookmarks
