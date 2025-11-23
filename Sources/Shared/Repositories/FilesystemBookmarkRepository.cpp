#include "App/stdafx.h"
#include "FilesystemBookmarkRepository.h"
#include "Utils/DebugLog.h"
#include "Utils/Miscellaneous.h"
#include "Utils/Errors.h"
#include "Features/BookmarkManagement/Models/LinkFileReader.h"
#include "Features/BookmarkManagement/Models/Data.h"
#include "Features/BookmarkManagement/Models/FileList.h"
#include <filesystem>

namespace Bookmarks
{

FilesystemBookmarkRepository::FilesystemBookmarkRepository()
{
    DEBUG_LOG_FUNC();
    _currentDirectory = get_current_working_directory();
    DEBUG_LOG("FilesystemBookmarkRepository initialized with directory: " << narrow(_currentDirectory));
    DEBUG_LOG_FUNC_EXIT();
}

FilesystemBookmarkRepository::~FilesystemBookmarkRepository()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG_FUNC_EXIT();
}

BookmarkOperationResult FilesystemBookmarkRepository::CreateBookmark(
    const std::wstring& name,
    const std::wstring& url,
    const std::wstring& directory)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating bookmark: " << narrow(name) << " -> " << narrow(url));
    
    try {
        std::wstring targetDir = directory.empty() ? _currentDirectory : directory;
        
        // Change to target directory if different from current
        std::wstring originalDir = get_current_working_directory();
        if (targetDir != originalDir) {
            if (!change_working_directory(targetDir)) {
                return BookmarkOperationResult::Failure(
                    L"Failed to access target directory: " + targetDir,
                    L"Directory may not exist or insufficient permissions"
                );
            }
        }
        
        // Create the URL file using existing utility function
        std::wstring filePath = create_url_file(name.c_str(), url.c_str());
        
        // Restore original directory
        if (targetDir != originalDir) {
            change_working_directory(originalDir);
        }
        
        // Verify file was created
        std::wstring fullPath = concatenate_paths(targetDir, filePath);
        if (std::filesystem::exists(narrow(fullPath))) {
            DEBUG_LOG("Bookmark created successfully: " << narrow(fullPath));
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Success(L"Bookmark created successfully");
        } else {
            DEBUG_LOG("Bookmark creation verification failed");
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Failure(L"Bookmark creation failed", L"File was not created");
        }
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in CreateBookmark: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to create bookmark: " + name,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in CreateBookmark");
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to create bookmark: " + name,
            L"Unknown error occurred"
        );
    }
}

BookmarkInfoPtr FilesystemBookmarkRepository::GetBookmark(const std::wstring& fileName)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Getting bookmark: " << narrow(fileName));
    
    try {
        std::wstring fullPath = concatenate_paths(_currentDirectory, fileName);
        
        if (!std::filesystem::exists(narrow(fullPath))) {
            DEBUG_LOG("Bookmark file does not exist: " << narrow(fullPath));
            DEBUG_LOG_FUNC_EXIT();
            return nullptr;
        }
        
        // Use LinkFileReader to extract bookmark information
        LinkFileReader reader(_currentDirectory);
        std::wstring url = reader.GetParamFromCurDirFile(fileName, param_url);
        std::wstring title = reader.GetParamFromCurDirFile(fileName, param_name);
        
        // If no title in file, use filename without extension as title
        if (title.empty()) {
            title = fileName;
            auto dotPos = title.rfind(L'.');
            if (dotPos != std::wstring::npos) {
                title = title.substr(0, dotPos);
            }
        }
        
        // Get file metadata
        auto fileStatus = std::filesystem::status(narrow(fullPath));
        auto ftime = std::filesystem::last_write_time(narrow(fullPath));
        auto size = std::filesystem::file_size(narrow(fullPath));
        
        // Convert file time to time_t (simplified)
        time_t modTime = 0; // TODO: Convert std::filesystem::file_time_type to time_t
        
        auto bookmark = std::make_shared<BookmarkInfo>(
            fileName, title, url, fullPath, modTime, size
        );
        
        DEBUG_LOG("Bookmark loaded successfully: " << narrow(title));
        DEBUG_LOG_FUNC_EXIT();
        return bookmark;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in GetBookmark: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return nullptr;
    } catch (...) {
        DEBUG_LOG("Unknown exception in GetBookmark");
        DEBUG_LOG_FUNC_EXIT();
        return nullptr;
    }
}

BookmarkOperationResult FilesystemBookmarkRepository::UpdateBookmark(
    const std::wstring& fileName,
    const std::wstring& newName,
    const std::wstring& newUrl)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Updating bookmark: " << narrow(fileName) << " -> " << narrow(newName));
    
    try {
        std::wstring oldPath = concatenate_paths(_currentDirectory, fileName);
        
        if (!std::filesystem::exists(narrow(oldPath))) {
            return BookmarkOperationResult::Failure(
                L"Bookmark not found: " + fileName,
                L"Source file does not exist"
            );
        }
        
        // Change to working directory
        std::wstring originalDir = get_current_working_directory();
        if (!change_working_directory(_currentDirectory)) {
            return BookmarkOperationResult::Failure(
                L"Failed to access bookmark directory",
                L"Cannot change to working directory"
            );
        }
        
        try {
#ifdef EXTENDED_URL_FILE
            // For extended URL files, create new file and delete old one
            std::wstring newFilePath = create_url_file(newName.c_str(), newUrl.c_str());
            std::filesystem::remove(std::filesystem::absolute(fileName));
#else
            // For simple files, rename if name changed
            if (fileName != newName) {
                std::wstring newFileName = newName + ext_url; // Add .url extension
                std::filesystem::rename(
                    std::filesystem::absolute(fileName),
                    std::filesystem::absolute(newFileName)
                );
            }
#endif
            // Restore original directory
            change_working_directory(originalDir);
            
            DEBUG_LOG("Bookmark updated successfully");
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Success(L"Bookmark updated successfully");
            
        } catch (...) {
            // Restore original directory on error
            change_working_directory(originalDir);
            throw;
        }
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in UpdateBookmark: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to update bookmark: " + fileName,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in UpdateBookmark");
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to update bookmark: " + fileName,
            L"Unknown error occurred"
        );
    }
}

BookmarkOperationResult FilesystemBookmarkRepository::DeleteBookmark(const std::wstring& fileName)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Deleting bookmark: " << narrow(fileName));
    
    try {
        std::wstring fullPath = concatenate_paths(_currentDirectory, fileName);
        
        if (!std::filesystem::exists(narrow(fullPath))) {
            DEBUG_LOG("Bookmark file does not exist, considering deletion successful");
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Success(L"Bookmark already removed");
        }
        
        std::filesystem::remove(narrow(fullPath));
        
        // Verify deletion
        if (std::filesystem::exists(narrow(fullPath))) {
            DEBUG_LOG("File still exists after deletion attempt");
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Failure(
                L"Failed to delete bookmark: " + fileName,
                L"File still exists after deletion"
            );
        }
        
        DEBUG_LOG("Bookmark deleted successfully");
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Success(L"Bookmark deleted successfully");
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in DeleteBookmark: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to delete bookmark: " + fileName,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in DeleteBookmark");
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to delete bookmark: " + fileName,
            L"Unknown error occurred"
        );
    }
}

BookmarkOperationResult FilesystemBookmarkRepository::MoveBookmark(
    const std::wstring& fileName,
    const std::wstring& targetDirectory)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Moving bookmark: " << narrow(fileName) << " to " << narrow(targetDirectory));
    
    try {
        std::wstring sourcePath = concatenate_paths(_currentDirectory, fileName);
        std::wstring targetPath = concatenate_paths(targetDirectory, fileName);
        
        if (!std::filesystem::exists(narrow(sourcePath))) {
            return BookmarkOperationResult::Failure(
                L"Source bookmark not found: " + fileName,
                L"Source file does not exist"
            );
        }
        
        // Ensure target directory exists
        if (!directory_exists(targetDirectory)) {
            return BookmarkOperationResult::Failure(
                L"Target directory does not exist: " + targetDirectory,
                L"Create target directory first"
            );
        }
        
        // Check if target file already exists
        if (std::filesystem::exists(narrow(targetPath))) {
            DEBUG_LOG("Target file already exists, considering move successful");
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Success(L"Bookmark already at target location");
        }
        
        // Perform the move
        std::filesystem::rename(narrow(sourcePath), narrow(targetPath));
        
        // Verify move
        bool success = std::filesystem::exists(narrow(targetPath)) && 
                      !std::filesystem::exists(narrow(sourcePath));
        
        if (success) {
            DEBUG_LOG("Bookmark moved successfully");
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Success(L"Bookmark moved successfully");
        } else {
            DEBUG_LOG("Move verification failed");
            DEBUG_LOG_FUNC_EXIT();
            return BookmarkOperationResult::Failure(
                L"Move operation failed for: " + fileName,
                L"File not found at target location after move"
            );
        }
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in MoveBookmark: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to move bookmark: " + fileName,
            widen(e.what())
        );
    } catch (...) {
        DEBUG_LOG("Unknown exception in MoveBookmark");
        DEBUG_LOG_FUNC_EXIT();
        return BookmarkOperationResult::Failure(
            L"Failed to move bookmark: " + fileName,
            L"Unknown error occurred"
        );
    }
}

BookmarkVector FilesystemBookmarkRepository::ListBookmarks()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Listing bookmarks in: " << narrow(_currentDirectory));
    
    BookmarkVector bookmarks;
    
    try {
        // Change to working directory temporarily
        std::wstring originalDir = get_current_working_directory();
        if (!change_working_directory(_currentDirectory)) {
            DEBUG_LOG("Failed to change to working directory");
            DEBUG_LOG_FUNC_EXIT();
            return bookmarks;
        }
        
        try {
            // Use existing Data class to get file list
#ifdef _WINDOWS
            Data data(CP_OEMCP);
#else
            Data data;
#endif
            FileVector files = data.GetFileList();
            
            for (const auto& file : files) {
                if (!file.IsFolder && !file.Url.empty()) {
                    auto bookmark = std::make_shared<BookmarkInfo>(
                        file.FileName,
                        file.Name.empty() ? file.FileName : file.Name,
                        file.Url,
                        concatenate_paths(_currentDirectory, file.FileName),
                        file.DateTime,
                        file.Size
                    );
                    bookmarks.push_back(bookmark);
                }
            }
            
            // Restore original directory
            change_working_directory(originalDir);
            
        } catch (...) {
            // Restore original directory on error
            change_working_directory(originalDir);
            throw;
        }
        
        DEBUG_LOG("Found " << bookmarks.size() << " bookmarks");
        DEBUG_LOG_FUNC_EXIT();
        return bookmarks;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in ListBookmarks: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return bookmarks;
    } catch (...) {
        DEBUG_LOG("Unknown exception in ListBookmarks");
        DEBUG_LOG_FUNC_EXIT();
        return bookmarks;
    }
}

BookmarkVector FilesystemBookmarkRepository::ListAllBookmarksRecursive()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Listing all bookmarks recursively from: " << narrow(_currentDirectory));
    
    BookmarkVector allBookmarks;
    
    try {
        // Change to working directory temporarily
        std::wstring originalDir = get_current_working_directory();
        if (!change_working_directory(_currentDirectory)) {
            DEBUG_LOG("Failed to change to working directory");
            DEBUG_LOG_FUNC_EXIT();
            return allBookmarks;
        }
        
        try {
            // Use existing Data class to get recursive bookmark list
#ifdef _WINDOWS
            Bookmarks::Data data(CP_OEMCP);
#else
            Bookmarks::Data data;
#endif
            auto treeNodes = data.GetAllBookmarksRecursive();
            
            for (const auto& node : treeNodes) {
                if (!node->url.empty()) {
                    auto bookmark = std::make_shared<BookmarkInfo>(
                        node->name,
                        node->title.empty() ? node->name : node->title,
                        node->url,
                        node->path,
                        node->dateTime,
                        node->size
                    );
                    allBookmarks.push_back(bookmark);
                }
            }
            
            // Restore original directory
            change_working_directory(originalDir);
            
        } catch (...) {
            // Restore original directory on error
            change_working_directory(originalDir);
            throw;
        }
        
        DEBUG_LOG("Found " << allBookmarks.size() << " bookmarks recursively");
        DEBUG_LOG_FUNC_EXIT();
        return allBookmarks;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in ListAllBookmarksRecursive: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return allBookmarks;
    } catch (...) {
        DEBUG_LOG("Unknown exception in ListAllBookmarksRecursive");
        DEBUG_LOG_FUNC_EXIT();
        return allBookmarks;
    }
}

bool FilesystemBookmarkRepository::BookmarkExists(const std::wstring& fileName)
{
    std::wstring fullPath = concatenate_paths(_currentDirectory, fileName);
    return std::filesystem::exists(narrow(fullPath)) && 
           std::filesystem::is_regular_file(narrow(fullPath));
}

std::wstring FilesystemBookmarkRepository::GetCurrentDirectory()
{
    return _currentDirectory;
}

bool FilesystemBookmarkRepository::SetCurrentDirectory(const std::wstring& directory)
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

} // namespace Bookmarks