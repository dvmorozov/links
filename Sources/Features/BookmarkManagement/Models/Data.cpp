
#include "App/stdafx.h"

#include "Data.h"
#include "Shared/Controllers/Commands.h"
#include "LinkFileReader.h"
#include "Utils/DebugLog.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{
FileVector Data::GetFileList() 
{ 
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetFileList() called");
    FileVector files = _fileList->GetFileList();
    DEBUG_LOG("GetFileList() returned " << files.size() << " files");
    
    // Log first few files for debugging
    for (size_t i = 0; i < files.size() && i < 5; ++i) {
        const auto& file = files[i];
        DEBUG_LOG("File " << (i+1) << ": " << narrow(file.FileName) 
                 << " (IsFolder: " << (file.IsFolder ? "YES" : "NO")
                 << ", URL: " << (file.Url.empty() ? "EMPTY" : narrow(file.Url.substr(0, 50)))
                 << ")");
    }
    
    DEBUG_LOG_FUNC_EXIT();
    return files;
}

FileVector Data::GetDirList() 
{ 
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetDirList() called");
    FileVector dirs = _fileList->GetDirList();
    DEBUG_LOG("GetDirList() returned " << dirs.size() << " directories");
    DEBUG_LOG_FUNC_EXIT();
    return dirs;
}

DirectoryTreeNodePtr Data::GetDirectoryTree()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetDirectoryTree() called - building complete directory tree");
    
    DirectoryTreeNodePtr root = GetDirectoryTree(L"");
    
    DEBUG_LOG("GetDirectoryTree() completed");
    DEBUG_LOG_FUNC_EXIT();
    return root;
}

DirectoryTreeNodePtr Data::GetDirectoryTree(const std::wstring& relativePath)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetDirectoryTree() called for path: " << narrow(relativePath));
    
    DirectoryTreeNodePtr result = buildDirectoryTreeRecursive(L"", relativePath);
    
    DEBUG_LOG("GetDirectoryTree() completed for path: " << narrow(relativePath));
    DEBUG_LOG_FUNC_EXIT();
    return result;
}

std::vector<DirectoryTreeNodePtr> Data::GetAllBookmarksRecursive()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetAllBookmarksRecursive() called");
    
    std::vector<DirectoryTreeNodePtr> bookmarks;
    DirectoryTreeNodePtr root = GetDirectoryTree();
    
    if (root) {
        root->getBookmarksRecursive(bookmarks);
    }
    
    DEBUG_LOG("GetAllBookmarksRecursive() found " << bookmarks.size() << " bookmark files");
    DEBUG_LOG_FUNC_EXIT();
    return bookmarks;
}

std::vector<DirectoryTreeNodePtr> Data::GetAllFoldersRecursive()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("GetAllFoldersRecursive() called");
    
    std::vector<DirectoryTreeNodePtr> folders;
    DirectoryTreeNodePtr root = GetDirectoryTree();
    
    if (root) {
        root->getFoldersRecursive(folders);
    }
    
    DEBUG_LOG("GetAllFoldersRecursive() found " << folders.size() << " folders");
    DEBUG_LOG_FUNC_EXIT();
    return folders;
}

DirectoryTreeNodePtr Data::buildDirectoryTreeRecursive(const std::wstring& currentPath, const std::wstring& relativePath)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("buildDirectoryTreeRecursive() called for current: " << narrow(currentPath) << ", relative: " << narrow(relativePath));
    
    try {
        // Get current directory name for root node
        std::wstring currentDirName = relativePath.empty() ? L"." : relativePath;
        DirectoryTreeNodePtr rootNode = std::make_shared<DirectoryTreeNode>(currentPath, currentDirName, true);
        
        // Get files and directories in current location through abstraction layer
        // NOTE: This works on the current working directory context of the Data object
        FileVector files = GetFileList();
        FileVector dirs = GetDirList();
        
        DEBUG_LOG("Found " << files.size() << " files and " << dirs.size() << " directories");
        
        // Add files to tree
        for (const auto& file : files) {
            if (!file.IsFolder) {
                DirectoryTreeNodePtr fileNode = std::make_shared<DirectoryTreeNode>(
                    currentPath + L"/" + file.FileName, 
                    file.FileName, 
                    false
                );
                fileNode->url = file.Url;
                fileNode->title = file.Name;
                fileNode->dateTime = file.DateTime;
                fileNode->size = file.Size;
                
                rootNode->addChild(fileNode);
                
                if (!file.Url.empty()) {
                    DEBUG_LOG("Added bookmark file: " << narrow(file.FileName) << " with URL: " << narrow(file.Url.substr(0, 50)));
                }
            }
        }
        
        // Add subdirectories as nodes (non-recursive for now)
        // TODO: To implement full recursion, the abstraction layer needs to support
        // creating Data objects for specific subdirectory contexts without filesystem changes
        for (const auto& dir : dirs) {
            if (dir.IsFolder && dir.FileName != L"." && dir.FileName != L"..") {
                DEBUG_LOG("Processing subdirectory: " << narrow(dir.FileName));
                
                std::wstring newCurrentPath = currentPath.empty() ? dir.FileName : currentPath + L"/" + dir.FileName;
                DirectoryTreeNodePtr subDirNode = std::make_shared<DirectoryTreeNode>(newCurrentPath, dir.FileName, true);
                
                // Add the subdirectory node (structure preserved, but no recursive traversal)
                rootNode->addChild(subDirNode);
                
                DEBUG_LOG("Added subdirectory node: " << narrow(dir.FileName) << " (non-recursive for abstraction compliance)");
            }
        }
        
        DEBUG_LOG("buildDirectoryTreeRecursive() completed for " << narrow(relativePath));
        DEBUG_LOG_FUNC_EXIT();
        return rootNode;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in buildDirectoryTreeRecursive: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return nullptr;
    }
}
}  // namespace Bookmarks
