
#pragma once
#include "App/Globals.h"
#include "FileList.h"
#include <memory>

namespace Bookmarks
{
// Forward declaration
struct DirectoryTreeNode;
typedef std::shared_ptr<DirectoryTreeNode> DirectoryTreeNodePtr;
typedef std::vector<DirectoryTreeNodePtr> DirectoryTreeNodes;

// Represents a node in the directory tree (file or folder)
struct DirectoryTreeNode : public std::enable_shared_from_this<DirectoryTreeNode>
{
    std::wstring path;           // Full path from root
    std::wstring name;           // File/folder name
    bool isFolder;               // True if this is a directory
    std::wstring url;            // URL if this is a bookmark file
    std::wstring title;          // Display title if available
    time_t dateTime;             // Last modified time
    size_t size;                 // File size
    DirectoryTreeNodes children; // Child nodes (for folders)
    std::weak_ptr<DirectoryTreeNode> parent; // Parent node (weak to avoid cycles)
    
    DirectoryTreeNode(const std::wstring& nodePath, const std::wstring& nodeName, bool folder)
        : path(nodePath), name(nodeName), isFolder(folder), dateTime(0), size(0)
    {
    }
    
    // Add a child node
    void addChild(DirectoryTreeNodePtr child)
    {
        child->parent = shared_from_this();
        children.push_back(child);
    }
    
    // Get all bookmark files recursively
    void getBookmarksRecursive(std::vector<DirectoryTreeNodePtr>& bookmarks) const
    {
        if (!isFolder && !url.empty()) {
            bookmarks.push_back(std::const_pointer_cast<DirectoryTreeNode>(shared_from_this()));
        }
        
        for (const auto& child : children) {
            child->getBookmarksRecursive(bookmarks);
        }
    }
    
    // Get all folders recursively  
    void getFoldersRecursive(std::vector<DirectoryTreeNodePtr>& folders) const
    {
        if (isFolder) {
            folders.push_back(std::const_pointer_cast<DirectoryTreeNode>(shared_from_this()));
        }
        
        for (const auto& child : children) {
            child->getFoldersRecursive(folders);
        }
    }
};

class Data
{
private:
    FileList *_fileList;

public:
    FileVector GetFileList();
    FileVector GetDirList();
    
    // New methods for recursive directory tree operations
    DirectoryTreeNodePtr GetDirectoryTree();
    DirectoryTreeNodePtr GetDirectoryTree(const std::wstring& relativePath);
    std::vector<DirectoryTreeNodePtr> GetAllBookmarksRecursive();
    std::vector<DirectoryTreeNodePtr> GetAllFoldersRecursive();

    Data(
#ifdef _WINDOWS
        UINT codePage
#endif
    )
    {
        _fileList = new FileList(FileListReaderDir::GetFileReader(
#ifdef _WINDOWS
            codePage
#endif
            ));
    }

    ~Data()
    {
        if (_fileList)
            delete _fileList;
    }

private:
    // Helper method to build directory tree recursively
    DirectoryTreeNodePtr buildDirectoryTreeRecursive(const std::wstring& currentPath, const std::wstring& relativePath);
};
}  // namespace Bookmarks
