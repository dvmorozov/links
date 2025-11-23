#pragma once
#include "App/Globals.h"
#include "IBookmarkRepository.h"

namespace Bookmarks
{

// Filesystem-based implementation of IBookmarkRepository
// Uses the current filesystem operations for bookmark management
class FilesystemBookmarkRepository : public IBookmarkRepository
{
public:
    FilesystemBookmarkRepository();
    virtual ~FilesystemBookmarkRepository();

    // === CRUD Operations ===
    virtual BookmarkOperationResult CreateBookmark(
        const std::wstring& name,
        const std::wstring& url,
        const std::wstring& directory = L""
    ) override;

    virtual BookmarkInfoPtr GetBookmark(const std::wstring& fileName) override;
    
    virtual BookmarkOperationResult UpdateBookmark(
        const std::wstring& fileName,
        const std::wstring& newName,
        const std::wstring& newUrl
    ) override;

    virtual BookmarkOperationResult DeleteBookmark(const std::wstring& fileName) override;

    // === Movement Operations ===
    virtual BookmarkOperationResult MoveBookmark(
        const std::wstring& fileName,
        const std::wstring& targetDirectory
    ) override;

    // === Query Operations ===
    virtual BookmarkVector ListBookmarks() override;
    virtual BookmarkVector ListAllBookmarksRecursive() override;
    virtual bool BookmarkExists(const std::wstring& fileName) override;

    // === Context Operations ===
    virtual std::wstring GetCurrentDirectory() override;
    virtual bool SetCurrentDirectory(const std::wstring& directory) override;

private:
    std::wstring _currentDirectory;
};

} // namespace Bookmarks