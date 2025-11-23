#include "App/stdafx.h"
#include "FilesystemDirectoryRepository.h"
#include "Features/BookmarkManagement/Models/Data.h"
#include "Features/BookmarkManagement/Models/FileList.h"
#include "Utils/DebugLog.h"
#include "Utils/Miscellaneous.h"
#include "Utils/Errors.h"
#include "Shared/Controllers/Commands.h"
#include <filesystem>

namespace Bookmarks
{

FilesystemDirectoryRepository::FilesystemDirectoryRepository()
{
    DEBUG_LOG_FUNC();
    _currentDirectory = get_current_working_directory();
    DEBUG_LOG("FilesystemDirectoryRepository initialized with directory: " << narrow(_currentDirectory));
    DEBUG_LOG_FUNC_EXIT();
}

FilesystemDirectoryRepository::~FilesystemDirectoryRepository()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG_FUNC_EXIT();
}

DirectoryOperationResult FilesystemDirectoryRepository::CreateDirectory(
    const std::wstring& name,
    const std::wstring& parentPath)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating directory: " << narrow(name));
    
    try {
        std::wstring targetParent = parentPath.empty() ? _currentDirectory : parentPath;
        
        // Change to target parent directory
        std::wstring originalDir = get_current_working_directory();
        if (!change_working_directory(targetParent)) {
            return DirectoryOperationResult::Failure(
                L"Failed to access parent directory: " + targetParent,
                L"Directory may not exist or insufficient permissions"
            );
        }
        
        try {
            // Use existing make_directory function
            make_directory(name);
            
            // Restore original directory
            change_working_directory(originalDir);
            
            // Verify creation
            std::wstring fullPath = concatenate_paths(targetParent, name);
            if (directory_exists(fullPath)) {
                DEBUG_LOG("Directory created successfully: " << narrow(fullPath));
                DEBUG_LOG_FUNC_EXIT();
                return DirectoryOperationResult::Success(L"Directory created successfully");
            } else {
                DEBUG_LOG("Directory creation verification failed");
                DEBUG_LOG_FUNC_EXIT();
                return DirectoryOperationResult::Failure(
                    L"Directory creation failed: " + name,
                    L"Directory was not created"
                );
            }
            
        } catch (...) {
            // Restore original directory on error
            change_working_directory(originalDir);
            throw;
        }
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in CreateDirectory: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to create directory: " + name,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in CreateDirectory");
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to create directory: " + name,
            L"Unknown error occurred"
        );
    }
}

DirectoryOperationResult FilesystemDirectoryRepository::CreateDirectoryPath(const std::wstring& path)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating directory path: " << narrow(path));
    
    try {
        std::wstring currentPath = _currentDirectory;
        
        // Split the path into components
        std::vector<std::wstring> pathComponents;
        std::wstring component;
        std::wstringstream pathStream(path);
        
        while (std::getline(pathStream, component, L'/')) {
            if (!component.empty()) {
                pathComponents.push_back(component);
            }
        }
        
        // Create each directory component
        for (const auto& comp : pathComponents) {
            std::wstring fullPath = concatenate_paths(currentPath, comp);
            
            if (!directory_exists(fullPath)) {
                auto result = CreateDirectory(comp, currentPath);
                if (!result.success) {
                    DEBUG_LOG("Failed to create path component: " << narrow(comp));
                    DEBUG_LOG_FUNC_EXIT();
                    return result;
                }
            }
            
            // Update current path for next component
            currentPath = fullPath;
        }
        
        DEBUG_LOG("Directory path created successfully: " << narrow(path));
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Success(L"Directory path created successfully");
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in CreateDirectoryPath: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to create directory path: " + path,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in CreateDirectoryPath");
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to create directory path: " + path,
            L"Unknown error occurred"
        );
    }
}

DirectoryInfoPtr FilesystemDirectoryRepository::GetDirectory(const std::wstring& name)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Getting directory info: " << narrow(name));
    
    try {
        std::wstring fullPath = concatenate_paths(_currentDirectory, name);
        
        if (!directory_exists(fullPath)) {
            DEBUG_LOG("Directory does not exist: " << narrow(fullPath));
            DEBUG_LOG_FUNC_EXIT();
            return nullptr;
        }
        
        // Get directory metadata
        size_t itemCount = 0;
        time_t modTime = 0;
        
        try {
            // Count items in directory
            for (const auto& entry : std::filesystem::directory_iterator(narrow(fullPath))) {
                itemCount++;
            }
            
            // Get last write time
            auto ftime = std::filesystem::last_write_time(narrow(fullPath));
            // TODO: Convert std::filesystem::file_time_type to time_t
            
        } catch (...) {
            // Ignore metadata errors
        }
        
        auto dirInfo = std::make_shared<DirectoryInfo>(name, fullPath, modTime, itemCount);
        
        DEBUG_LOG("Directory info loaded successfully");
        DEBUG_LOG_FUNC_EXIT();
        return dirInfo;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in GetDirectory: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return nullptr;
    } catch (...) {
        DEBUG_LOG("Unknown exception in GetDirectory");
        DEBUG_LOG_FUNC_EXIT();
        return nullptr;
    }
}

DirectoryOperationResult FilesystemDirectoryRepository::RenameDirectory(
    const std::wstring& oldName,
    const std::wstring& newName)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Renaming directory: " << narrow(oldName) << " -> " << narrow(newName));
    
    try {
        std::wstring oldPath = concatenate_paths(_currentDirectory, oldName);
        std::wstring newPath = concatenate_paths(_currentDirectory, newName);
        
        if (!directory_exists(oldPath)) {
            return DirectoryOperationResult::Failure(
                L"Source directory not found: " + oldName,
                L"Source directory does not exist"
            );
        }
        
        if (directory_exists(newPath)) {
            return DirectoryOperationResult::Failure(
                L"Target directory already exists: " + newName,
                L"Choose a different name"
            );
        }
        
        // Perform the rename
        std::filesystem::rename(narrow(oldPath), narrow(newPath));
        
        // Verify rename
        if (directory_exists(newPath) && !directory_exists(oldPath)) {
            DEBUG_LOG("Directory renamed successfully");
            DEBUG_LOG_FUNC_EXIT();
            return DirectoryOperationResult::Success(L"Directory renamed successfully");
        } else {
            DEBUG_LOG("Rename verification failed");
            DEBUG_LOG_FUNC_EXIT();
            return DirectoryOperationResult::Failure(
                L"Rename operation failed for: " + oldName,
                L"Directory not found at new location after rename"
            );
        }
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in RenameDirectory: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to rename directory: " + oldName,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in RenameDirectory");
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to rename directory: " + oldName,
            L"Unknown error occurred"
        );
    }
}

DirectoryOperationResult FilesystemDirectoryRepository::DeleteDirectory(const std::wstring& name)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Deleting directory: " << narrow(name));
    
    try {
        std::wstring fullPath = concatenate_paths(_currentDirectory, name);
        
        if (!directory_exists(fullPath)) {
            DEBUG_LOG("Directory does not exist, considering deletion successful");
            DEBUG_LOG_FUNC_EXIT();
            return DirectoryOperationResult::Success(L"Directory already removed");
        }
        
        // Use existing delete_directory function from Commands.cpp
        delete_directory(name);
        
        // Verify deletion
        if (directory_exists(fullPath)) {
            DEBUG_LOG("Directory still exists after deletion attempt");
            DEBUG_LOG_FUNC_EXIT();
            return DirectoryOperationResult::Failure(
                L"Failed to delete directory: " + name,
                L"Directory still exists after deletion"
            );
        }
        
        DEBUG_LOG("Directory deleted successfully");
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Success(L"Directory deleted successfully");
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in DeleteDirectory: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to delete directory: " + name,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in DeleteDirectory");
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to delete directory: " + name,
            L"Unknown error occurred"
        );
    }
}

DirectoryOperationResult FilesystemDirectoryRepository::MoveDirectory(
    const std::wstring& directoryName,
    const std::wstring& targetParentDirectory)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Moving directory: " << narrow(directoryName) << " to " << narrow(targetParentDirectory));
    
    try {
        std::wstring sourcePath = concatenate_paths(_currentDirectory, directoryName);
        std::wstring targetPath = concatenate_paths(targetParentDirectory, directoryName);
        
        if (!directory_exists(sourcePath)) {
            return DirectoryOperationResult::Failure(
                L"Source directory not found: " + directoryName,
                L"Source directory does not exist"
            );
        }
        
        if (!directory_exists(targetParentDirectory)) {
            return DirectoryOperationResult::Failure(
                L"Target parent directory does not exist: " + targetParentDirectory,
                L"Create target parent directory first"
            );
        }
        
        if (directory_exists(targetPath)) {
            return DirectoryOperationResult::Failure(
                L"Target directory already exists: " + targetPath,
                L"Choose a different location or name"
            );
        }
        
        // Perform the move
        std::filesystem::rename(narrow(sourcePath), narrow(targetPath));
        
        // Verify move
        bool success = directory_exists(targetPath) && !directory_exists(sourcePath);
        
        if (success) {
            DEBUG_LOG("Directory moved successfully");
            DEBUG_LOG_FUNC_EXIT();
            return DirectoryOperationResult::Success(L"Directory moved successfully");
        } else {
            DEBUG_LOG("Move verification failed");
            DEBUG_LOG_FUNC_EXIT();
            return DirectoryOperationResult::Failure(
                L"Move operation failed for: " + directoryName,
                L"Directory not found at target location after move"
            );
        }
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in MoveDirectory: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to move directory: " + directoryName,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in MoveDirectory");
        DEBUG_LOG_FUNC_EXIT();
        return DirectoryOperationResult::Failure(
            L"Failed to move directory: " + directoryName,
            L"Unknown error occurred"
        );
    }
}

DirectoryVector FilesystemDirectoryRepository::ListDirectories()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Listing directories in: " << narrow(_currentDirectory));
    
    DirectoryVector directories;
    
    try {
        // Change to working directory temporarily
        std::wstring originalDir = get_current_working_directory();
        if (!change_working_directory(_currentDirectory)) {
            DEBUG_LOG("Failed to change to working directory");
            DEBUG_LOG_FUNC_EXIT();
            return directories;
        }
        
        try {
            // Use existing Data class to get directory list
#ifdef _WINDOWS
            Data data(CP_OEMCP);
#else
            Data data;
#endif
            FileVector dirs = data.GetDirList();
            
            for (const auto& dir : dirs) {
                if (dir.IsFolder && dir.FileName != L"." && dir.FileName != L"..") {
                    auto dirInfo = std::make_shared<DirectoryInfo>(
                        dir.FileName,
                        concatenate_paths(_currentDirectory, dir.FileName),
                        dir.DateTime,
                        0 // Item count not available from FileVector
                    );
                    directories.push_back(dirInfo);
                }
            }
            
            // Restore original directory
            change_working_directory(originalDir);
            
        } catch (...) {
            // Restore original directory on error
            change_working_directory(originalDir);
            throw;
        }
        
        DEBUG_LOG("Found " << directories.size() << " directories");
        DEBUG_LOG_FUNC_EXIT();
        return directories;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in ListDirectories: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return directories;
    } catch (...) {
        DEBUG_LOG("Unknown exception in ListDirectories");
        DEBUG_LOG_FUNC_EXIT();
        return directories;
    }
}

DirectoryVector FilesystemDirectoryRepository::ListAllDirectoriesRecursive()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Listing all directories recursively from: " << narrow(_currentDirectory));
    
    DirectoryVector allDirectories;
    
    try {
        // Change to working directory temporarily
        std::wstring originalDir = get_current_working_directory();
        if (!change_working_directory(_currentDirectory)) {
            DEBUG_LOG("Failed to change to working directory");
            DEBUG_LOG_FUNC_EXIT();
            return allDirectories;
        }
        
        try {
            // Use existing Data class to get recursive directory list
#ifdef _WINDOWS
            Data data(CP_OEMCP);
#else
            Data data;
#endif
            auto treeNodes = data.GetAllFoldersRecursive();
            
            for (const auto& node : treeNodes) {
                if (node->isFolder && node->name != L"." && node->name != L"..") {
                    auto dirInfo = std::make_shared<DirectoryInfo>(
                        node->name,
                        node->path,
                        node->dateTime,
                        node->children.size()
                    );
                    allDirectories.push_back(dirInfo);
                }
            }
            
            // Restore original directory
            change_working_directory(originalDir);
            
        } catch (...) {
            // Restore original directory on error
            change_working_directory(originalDir);
            throw;
        }
        
        DEBUG_LOG("Found " << allDirectories.size() << " directories recursively");
        DEBUG_LOG_FUNC_EXIT();
        return allDirectories;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in ListAllDirectoriesRecursive: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return allDirectories;
    } catch (...) {
        DEBUG_LOG("Unknown exception in ListAllDirectoriesRecursive");
        DEBUG_LOG_FUNC_EXIT();
        return allDirectories;
    }
}

bool FilesystemDirectoryRepository::DirectoryExists(const std::wstring& name)
{
    std::wstring fullPath = concatenate_paths(_currentDirectory, name);
    return directory_exists(fullPath);
}

std::wstring FilesystemDirectoryRepository::GetCurrentDirectory()
{
    return _currentDirectory;
}

bool FilesystemDirectoryRepository::SetCurrentDirectory(const std::wstring& directory)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Setting current directory to: " << narrow(directory));
    
    if (directory_exists(directory)) {
        _currentDirectory = directory;
        DEBUG_LOG("Current directory updated successfully");
        DEBUG_LOG_FUNC_EXIT();
        return true;
    } else {
        DEBUG_LOG("Directory does not exist: " << narrow(directory));
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
}

std::wstring FilesystemDirectoryRepository::GetParentDirectory()
{
    std::filesystem::path currentPath(_currentDirectory);
    std::filesystem::path parentPath = currentPath.parent_path();
    return parentPath.wstring();
}

} // namespace Bookmarks