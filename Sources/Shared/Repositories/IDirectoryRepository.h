#pragma once
#include "App/Globals.h"
#include <vector>
#include <memory>

namespace Bookmarks
{

// Forward declarations  
struct DirectoryInfo;
typedef std::shared_ptr<DirectoryInfo> DirectoryInfoPtr;
typedef std::vector<DirectoryInfoPtr> DirectoryVector;

// Represents a directory with its metadata
struct DirectoryInfo
{
    std::wstring name;          // Directory name
    std::wstring path;          // Full path to the directory
    time_t dateTime;            // Last modified time
    size_t itemCount;           // Number of items in directory

    DirectoryInfo() : dateTime(0), itemCount(0) {}
    
    DirectoryInfo(const std::wstring& n, const std::wstring& p, time_t dt = 0, size_t count = 0)
        : name(n), path(p), dateTime(dt), itemCount(count) {}
};

// Result of a directory operation
struct DirectoryOperationResult
{
    bool success;
    std::wstring message;
    std::wstring errorDetails;

    DirectoryOperationResult(bool s = false, const std::wstring& msg = L"", const std::wstring& details = L"")
        : success(s), message(msg), errorDetails(details) {}
        
    static DirectoryOperationResult Success(const std::wstring& message = L"") 
    { 
        return DirectoryOperationResult(true, message); 
    }
    
    static DirectoryOperationResult Failure(const std::wstring& message, const std::wstring& details = L"") 
    { 
        return DirectoryOperationResult(false, message, details); 
    }
};

// Abstract interface for directory operations
class IDirectoryRepository
{
public:
    virtual ~IDirectoryRepository() = default;

    // === CRUD Operations ===
    
    // Create a new directory
    virtual DirectoryOperationResult CreateDirectory(
        const std::wstring& name,               // Directory name
        const std::wstring& parentPath = L""   // Parent directory (empty = current)
    ) = 0;
    
    // Create nested directory path (e.g., "Development/Tools")
    virtual DirectoryOperationResult CreateDirectoryPath(
        const std::wstring& path                // Full path to create
    ) = 0;

    // Read directory information
    virtual DirectoryInfoPtr GetDirectory(const std::wstring& name) = 0;
    
    // Rename directory
    virtual DirectoryOperationResult RenameDirectory(
        const std::wstring& oldName,
        const std::wstring& newName
    ) = 0;

    // Delete directory (and all contents)
    virtual DirectoryOperationResult DeleteDirectory(const std::wstring& name) = 0;

    // === Movement Operations ===
    
    // Move directory to different parent directory
    virtual DirectoryOperationResult MoveDirectory(
        const std::wstring& directoryName,
        const std::wstring& targetParentDirectory
    ) = 0;

    // === Query Operations ===
    
    // List all directories in current directory
    virtual DirectoryVector ListDirectories() = 0;
    
    // List all directories recursively from current directory
    virtual DirectoryVector ListAllDirectoriesRecursive() = 0;
    
    // Check if directory exists
    virtual bool DirectoryExists(const std::wstring& name) = 0;

    // === Context Operations ===
    
    // Get current working directory
    virtual std::wstring GetCurrentDirectory() = 0;
    
    // Change working directory (affects subsequent operations)
    virtual bool SetCurrentDirectory(const std::wstring& directory) = 0;
    
    // Get parent directory of current directory
    virtual std::wstring GetParentDirectory() = 0;
};

} // namespace Bookmarks