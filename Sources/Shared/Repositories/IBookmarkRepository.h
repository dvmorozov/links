#pragma once
#include "App/Globals.h"
#include <vector>
#include <memory>

namespace Bookmarks
{

// Forward declarations
struct BookmarkInfo;
typedef std::shared_ptr<BookmarkInfo> BookmarkInfoPtr;
typedef std::vector<BookmarkInfoPtr> BookmarkVector;

// Represents a bookmark with its metadata
struct BookmarkInfo
{
    std::wstring fileName;      // File name (including extension)
    std::wstring title;         // Display title
    std::wstring url;           // Target URL
    std::wstring path;          // Full path to the bookmark file
    time_t dateTime;            // Last modified time
    size_t size;                // File size

    BookmarkInfo() : dateTime(0), size(0) {}
    
    BookmarkInfo(const std::wstring& name, const std::wstring& t, const std::wstring& u, 
                 const std::wstring& p, time_t dt = 0, size_t sz = 0)
        : fileName(name), title(t), url(u), path(p), dateTime(dt), size(sz) {}
};

// Result of a bookmark operation
struct BookmarkOperationResult
{
    bool success;
    std::wstring message;
    std::wstring errorDetails;

    BookmarkOperationResult(bool s = false, const std::wstring& msg = L"", const std::wstring& details = L"")
        : success(s), message(msg), errorDetails(details) {}
        
    static BookmarkOperationResult Success(const std::wstring& message = L"") 
    { 
        return BookmarkOperationResult(true, message); 
    }
    
    static BookmarkOperationResult Failure(const std::wstring& message, const std::wstring& details = L"") 
    { 
        return BookmarkOperationResult(false, message, details); 
    }
};

// Abstract interface for bookmark operations
class IBookmarkRepository
{
public:
    virtual ~IBookmarkRepository() = default;

    // === CRUD Operations ===
    
    // Create a new bookmark
    virtual BookmarkOperationResult CreateBookmark(
        const std::wstring& name,           // Display name for the bookmark
        const std::wstring& url,            // Target URL
        const std::wstring& directory = L"" // Target directory (empty = current)
    ) = 0;

    // Read bookmark information
    virtual BookmarkInfoPtr GetBookmark(const std::wstring& fileName) = 0;
    
    // Update existing bookmark
    virtual BookmarkOperationResult UpdateBookmark(
        const std::wstring& fileName,
        const std::wstring& newName,
        const std::wstring& newUrl
    ) = 0;

    // Delete bookmark
    virtual BookmarkOperationResult DeleteBookmark(const std::wstring& fileName) = 0;

    // === Movement Operations ===
    
    // Move bookmark to different directory
    virtual BookmarkOperationResult MoveBookmark(
        const std::wstring& fileName,
        const std::wstring& targetDirectory
    ) = 0;

    // === Query Operations ===
    
    // List all bookmarks in current directory
    virtual BookmarkVector ListBookmarks() = 0;
    
    // List all bookmarks recursively from current directory
    virtual BookmarkVector ListAllBookmarksRecursive() = 0;
    
    // Check if bookmark exists
    virtual bool BookmarkExists(const std::wstring& fileName) = 0;

    // === Context Operations ===
    
    // Get current working directory
    virtual std::wstring GetCurrentDirectory() = 0;
    
    // Change working directory (affects subsequent operations)
    virtual bool SetCurrentDirectory(const std::wstring& directory) = 0;
};

} // namespace Bookmarks