#pragma once
#include "App/Globals.h"
#include "IDirectoryRepository.h"

namespace Bookmarks
{

// Filesystem-based implementation of IDirectoryRepository
// Uses the current filesystem operations for directory management
class FilesystemDirectoryRepository : public IDirectoryRepository
{
public:
    FilesystemDirectoryRepository();
    virtual ~FilesystemDirectoryRepository();

    // === CRUD Operations ===
    virtual DirectoryOperationResult CreateDirectory(
        const std::wstring& name,
        const std::wstring& parentPath = L""
    ) override;
    
    virtual DirectoryOperationResult CreateDirectoryPath(
        const std::wstring& path
    ) override;

    virtual DirectoryInfoPtr GetDirectory(const std::wstring& name) override;
    
    virtual DirectoryOperationResult RenameDirectory(
        const std::wstring& oldName,
        const std::wstring& newName
    ) override;

    virtual DirectoryOperationResult DeleteDirectory(const std::wstring& name) override;

    // === Movement Operations ===
    virtual DirectoryOperationResult MoveDirectory(
        const std::wstring& directoryName,
        const std::wstring& targetParentDirectory
    ) override;

    // === Query Operations ===
    virtual DirectoryVector ListDirectories() override;
    virtual DirectoryVector ListAllDirectoriesRecursive() override;
    virtual bool DirectoryExists(const std::wstring& name) override;

    // === Context Operations ===
    virtual std::wstring GetCurrentDirectory() override;
    virtual bool SetCurrentDirectory(const std::wstring& directory) override;
    virtual std::wstring GetParentDirectory() override;

private:
    std::wstring _currentDirectory;
};

} // namespace Bookmarks