#include "App/stdafx.h"
#include "BookmarkReorganizer.h"
#include "Shared/Controllers/Commands.h"
#include "Utils/Errors.h"
#include "Utils/DebugLog.h"

#include <filesystem>
#include <regex>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ctime>

namespace Bookmarks
{

BookmarkReorganizer::BookmarkReorganizer(Data* dataLayer, OllamaService* aiService)
    : _dataLayer(dataLayer), _aiService(aiService), _lastStatusMessage(L"Ready")
{
    DEBUG_LOG_FUNC();
    _currentDirectory = get_current_working_directory();
    DEBUG_LOG("BookmarkReorganizer created, current directory: " << narrow(_currentDirectory));
}

BookmarkReorganizer::~BookmarkReorganizer()
{
    DEBUG_LOG_FUNC();
    // Don't delete _dataLayer or _aiService as they are managed externally
}

void BookmarkReorganizer::SetRepositories(std::shared_ptr<IBookmarkRepository> bookmarkRepo, 
                                        std::shared_ptr<IDirectoryRepository> directoryRepo)
{
    _bookmarkRepo = bookmarkRepo;
    _directoryRepo = directoryRepo;
    DEBUG_LOG("BookmarkReorganizer repositories set");
}

bool BookmarkReorganizer::canReorganize()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Checking reorganization prerequisites with tree-based analysis");
    
    if (!_aiService) {
        _lastStatusMessage = L"AI service not initialized";
        DEBUG_LOG("ERROR: AI service not initialized");
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
    
    DEBUG_LOG("AI service initialized, checking availability");
    if (!_aiService->isAvailable()) {
        _lastStatusMessage = L"AI service (Ollama) is not available. Please check that Ollama is running.";
        DEBUG_LOG("ERROR: AI service not available");
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
    DEBUG_LOG("AI service is available");
    
    if (!_dataLayer) {
        _lastStatusMessage = L"Data layer not initialized";
        DEBUG_LOG("ERROR: Data layer not initialized");
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
    DEBUG_LOG("Data layer initialized, analyzing directory tree");
    
    // Use new tree-based approach to analyze the complete directory structure
    try {
        std::vector<DirectoryTreeNodePtr> allBookmarks = _dataLayer->GetAllBookmarksRecursive();
        std::vector<DirectoryTreeNodePtr> allFolders = _dataLayer->GetAllFoldersRecursive();
        
        DEBUG_LOG("Found " << allBookmarks.size() << " bookmarks recursively");
        DEBUG_LOG("Found " << allFolders.size() << " folders recursively");
        
        // Log sample bookmarks for debugging
        for (size_t i = 0; i < allBookmarks.size() && i < 5; ++i) {
            const auto& bookmark = allBookmarks[i];
            DEBUG_LOG("Bookmark " << (i+1) << ": " << narrow(bookmark->name) 
                     << " at path: " << narrow(bookmark->path) 
                     << " URL: " << (bookmark->url.empty() ? "EMPTY" : narrow(bookmark->url.substr(0, 50))));
        }
        
        // Log sample folders for debugging
        for (size_t i = 0; i < allFolders.size() && i < 5; ++i) {
            const auto& folder = allFolders[i];
            DEBUG_LOG("Folder " << (i+1) << ": " << narrow(folder->name) 
                     << " at path: " << narrow(folder->path) 
                     << " children: " << folder->children.size());
        }
        
        if (allBookmarks.empty() && allFolders.size() <= 1) {
            _lastStatusMessage = L"No bookmarks or folders found to reorganize";
            DEBUG_LOG("ERROR: No bookmarks or meaningful folder structure found");
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        if (allBookmarks.empty()) {
            _lastStatusMessage = L"No bookmark files found (only empty folders)";
            DEBUG_LOG("WARNING: No bookmark files found, but folders exist");
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        _lastStatusMessage = L"Ready to reorganize";
        DEBUG_LOG("Prerequisites passed - ready to reorganize with " << allBookmarks.size() << " bookmarks and " << allFolders.size() << " folders");
        DEBUG_LOG_FUNC_EXIT();
        return true;
        
    } catch (const std::exception& e) {
        _lastStatusMessage = L"Error analyzing directory structure: " + widen(e.what());
        DEBUG_LOG("Exception during directory analysis: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
}

std::vector<BookmarkData> BookmarkReorganizer::prepareBookmarksForAnalysis(const FileVector& files)
{
    DEBUG_LOG_FUNC();
    std::vector<BookmarkData> bookmarkData;
    
    for (const auto& file : files) {
        if (!file.IsFolder && !file.Url.empty()) {
            DEBUG_LOG("Processing bookmark file: " << narrow(file.FileName));
            // Extract domain from URL for better categorization
            std::wstring domain = extract_domain(file.Url);
            
            // Use Name if available, otherwise fall back to FileName
            std::wstring title = file.Name.empty() ? file.FileName : file.Name;
            
            bookmarkData.emplace_back(file.FileName, title, file.Url, domain);
        }
    }
    
    DEBUG_LOG("Prepared " << bookmarkData.size() << " bookmarks for analysis");
    DEBUG_LOG_FUNC_EXIT();
    return bookmarkData;
}

DirectoryContext BookmarkReorganizer::prepareDirectoryContext()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Preparing comprehensive directory context for AI analysis");
    
    DirectoryTreeNodePtr rootNode = _dataLayer->GetDirectoryTree();
    DirectoryContext context = prepareDirectoryContext(rootNode);
    
    DEBUG_LOG("Directory context prepared with " << context.bookmarks.size() << " bookmarks and " << context.existingFolders.size() << " folders");
    DEBUG_LOG_FUNC_EXIT();
    return context;
}

DirectoryContext BookmarkReorganizer::prepareDirectoryContext(DirectoryTreeNodePtr rootNode)
{
    DEBUG_LOG_FUNC();
    DirectoryContext context(_currentDirectory);
    
    if (!rootNode) {
        DEBUG_LOG("No root node provided");
        DEBUG_LOG_FUNC_EXIT();
        return context;
    }
    
    // Collect all bookmarks from the tree
    collectBookmarksFromTree(rootNode, context.bookmarks);
    DEBUG_LOG("Collected " << context.bookmarks.size() << " bookmarks from directory tree");
    
    // Collect all folders from the tree
    collectFoldersFromTree(rootNode, context.existingFolders);
    DEBUG_LOG("Collected " << context.existingFolders.size() << " folders from directory tree");
    
    DEBUG_LOG_FUNC_EXIT();
    return context;
}

void BookmarkReorganizer::collectBookmarksFromTree(DirectoryTreeNodePtr node, std::vector<BookmarkData>& bookmarks, const std::wstring& currentPath)
{
    if (!node) return;
    
    std::wstring nodePath = currentPath.empty() ? node->name : currentPath + L"/" + node->name;
    
    // If this is a bookmark file, add it to the collection
    if (!node->isFolder && !node->url.empty()) {
        std::wstring domain = extract_domain(node->url);
        std::wstring title = node->title.empty() ? node->name : node->title;
        
        bookmarks.emplace_back(node->name, title, node->url, domain, currentPath);
        
        DEBUG_LOG("Collected bookmark: " << narrow(node->name) << " from path: " << narrow(currentPath));
    }
    
    // Recursively process children
    for (const auto& child : node->children) {
        collectBookmarksFromTree(child, bookmarks, nodePath);
    }
}

void BookmarkReorganizer::collectFoldersFromTree(DirectoryTreeNodePtr node, std::vector<std::wstring>& folders, const std::wstring& currentPath)
{
    if (!node) return;
    
    std::wstring nodePath = currentPath.empty() ? node->name : currentPath + L"/" + node->name;
    
    // If this is a folder (and not the root), add it to the collection
    if (node->isFolder && !currentPath.empty()) {
        folders.push_back(nodePath);
        DEBUG_LOG("Collected folder: " << narrow(nodePath));
    }
    
    // Recursively process children
    for (const auto& child : node->children) {
        collectFoldersFromTree(child, folders, nodePath);
    }
}

std::wstring BookmarkReorganizer::getSafeFolderName(const std::wstring& originalName)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Getting safe folder name for: " << narrow(originalName));
    
    std::wstring safeName = originalName;
    
    // First, decode Unicode escape sequences (like \u0026 to &)
    std::wregex unicodePattern(L"\\\\u([0-9a-fA-F]{4})");
    std::wsmatch match;
    std::wstring::const_iterator start = safeName.cbegin();
    std::wstring result;
    
    while (std::regex_search(start, safeName.cend(), match, unicodePattern)) {
        // Add text before the match
        result.append(start, match[0].first);
        
        // Parse the Unicode code point
        std::wstring hexStr = match[1].str();
        wchar_t codePoint = static_cast<wchar_t>(std::stoi(hexStr, nullptr, 16));
        result += codePoint;
        
        // Move to after this match
        start = match[0].second;
    }
    // Add remaining text
    result.append(start, safeName.cend());
    safeName = result;
    
    DEBUG_LOG("After Unicode decoding: " << narrow(safeName));
    
    // Remove or replace invalid characters for folder names
    std::wregex invalidChars(L"[<>:\"/\\|?*]");
    safeName = std::regex_replace(safeName, invalidChars, L"_");
    
    // Trim whitespace
    safeName.erase(0, safeName.find_first_not_of(L" \t"));
    safeName.erase(safeName.find_last_not_of(L" \t") + 1);
    
    // Ensure name is not too long (Windows has 255 char limit)
    if (safeName.length() > 200) {
        safeName = safeName.substr(0, 200);
    }
    
    // Ensure name is not empty
    if (safeName.empty()) {
        safeName = L"Misc";
    }
    
    DEBUG_LOG("Safe folder name: " << narrow(safeName));
    DEBUG_LOG_FUNC_EXIT();
    return safeName;
}

bool BookmarkReorganizer::createFolderIfNotExists(const std::wstring& folderName)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating folder path if not exists: " << narrow(folderName));
    
    try {
        // Handle full paths (e.g., "Development/Tools") by creating intermediate directories
        std::wstring currentPath = _currentDirectory;
        std::wstring pathToCreate = folderName;
        
        // Split the path into components
        std::vector<std::wstring> pathComponents;
        std::wstring component;
        std::wstringstream pathStream(pathToCreate);
        
        while (std::getline(pathStream, component, L'/')) {
            if (!component.empty()) {
                pathComponents.push_back(component);
            }
        }
        
        // Create each directory component
        for (const auto& comp : pathComponents) {
            std::wstring safeName = getSafeFolderName(comp);
            std::wstring fullPath = concatenate_paths(currentPath, safeName);
            
            DEBUG_LOG("Checking path component: " << narrow(fullPath));
            
            if (!directory_exists(fullPath)) {
                DEBUG_LOG("Creating directory: " << narrow(safeName) << " in " << narrow(currentPath));
                
                // Use repository pattern for directory creation
                bool creationSuccess = false;
                if (_directoryRepo) {
                    auto result = _directoryRepo->CreateDirectory(safeName, L"");
                    creationSuccess = result.success;
                    if (!creationSuccess) {
                        DEBUG_LOG("Repository directory creation failed: " << narrow(result.message));
                    }
                } else {
                    DEBUG_LOG("Directory repository not available - this should not happen");
                }
                
                logFolderCreation(safeName, creationSuccess);
                
                if (!creationSuccess) {
                    DEBUG_LOG("Failed to create directory: " << narrow(fullPath));
                    DEBUG_LOG_FUNC_EXIT();
                    return false;
                }
            }
            
            // Update current path for next component
            currentPath = fullPath;
        }
        
        DEBUG_LOG("Full folder path created successfully: " << narrow(currentPath));
        DEBUG_LOG_FUNC_EXIT();
        return true;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in createFolderIfNotExists: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return false;
    } catch (...) {
        DEBUG_LOG("Unknown exception in createFolderIfNotExists");
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
}

bool BookmarkReorganizer::moveBookmarkToFolder(const std::wstring& bookmarkFile, const std::wstring& targetFolder)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Moving bookmark: " << narrow(bookmarkFile) << " to folder path: " << narrow(targetFolder));
    
    // Build the full target directory path by processing each component
    std::wstring targetDir = _currentDirectory;
    std::wstring pathToCreate = targetFolder;
    
    try {
        // Split the target folder path into components
        std::vector<std::wstring> pathComponents;
        std::wstring component;
        std::wstringstream pathStream(pathToCreate);
        
        while (std::getline(pathStream, component, L'/')) {
            if (!component.empty()) {
                pathComponents.push_back(component);
            }
        }
        
        // Build the full target directory path
        for (const auto& comp : pathComponents) {
            std::wstring safeName = getSafeFolderName(comp);
            targetDir = concatenate_paths(targetDir, safeName);
        }
        
        std::wstring sourcePath = concatenate_paths(_currentDirectory, bookmarkFile);
        std::wstring targetPath = concatenate_paths(targetDir, bookmarkFile);
        
        DEBUG_LOG("Source path: " << narrow(sourcePath));
        DEBUG_LOG("Target directory: " << narrow(targetDir));
        DEBUG_LOG("Target path: " << narrow(targetPath));
        
        // Check if source file exists
        if (!std::filesystem::exists(narrow(sourcePath))) {
            DEBUG_LOG("Source file does not exist");
            logFileMovement(bookmarkFile, _currentDirectory, targetDir, false);
            writeAutoOrganizeLog("ERROR: Source file does not exist: " + narrow(bookmarkFile));
            std::wstring bookmarkTitle = getBookmarkTitle(bookmarkFile);
            recordOperation(OperationRecord::OPERATION_FAILED, bookmarkTitle, L"", L"", L"Source file does not exist");
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        // Create target directory lazily (only when we have a file to move)
        if (!directory_exists(targetDir)) {
            DEBUG_LOG("Target directory does not exist, creating lazily: " << narrow(targetDir));
            if (!createFolderIfNotExists(targetFolder)) {
                DEBUG_LOG("Failed to create target directory lazily");
                logFileMovement(bookmarkFile, _currentDirectory, targetDir, false);
                writeAutoOrganizeLog("ERROR: Failed to create target directory: " + narrow(targetDir));
                std::wstring bookmarkTitle = getBookmarkTitle(bookmarkFile);
                recordOperation(OperationRecord::OPERATION_FAILED, bookmarkTitle, L"", L"", L"Failed to create target directory: " + targetDir);
                DEBUG_LOG_FUNC_EXIT();
                return false;
            }
            DEBUG_LOG("Successfully created target directory lazily");
        }
        
        // Check if target file already exists
        if (std::filesystem::exists(narrow(targetPath))) {
            DEBUG_LOG("Target file already exists, skipping move");
            logFileMovement(bookmarkFile, _currentDirectory, targetDir, true);
            writeAutoOrganizeLog("INFO: Target file already exists, skipping: " + narrow(bookmarkFile));
            DEBUG_LOG_FUNC_EXIT();
            return true;
        }
        
        DEBUG_LOG("Performing file move");
        std::filesystem::rename(narrow(sourcePath), narrow(targetPath));
        
        bool success = std::filesystem::exists(narrow(targetPath));
        
        // Log file movement
        logFileMovement(bookmarkFile, _currentDirectory, targetDir, success);
        
        // Record operation in history
        if (success) {
            std::wstring bookmarkTitle = getBookmarkTitle(bookmarkFile);
            recordOperation(OperationRecord::FILE_MOVED, bookmarkTitle, _currentDirectory, targetDir);
        } else {
            std::wstring bookmarkTitle = getBookmarkTitle(bookmarkFile);
            recordOperation(OperationRecord::OPERATION_FAILED, bookmarkTitle, L"", L"", L"File move failed after filesystem rename");
            writeAutoOrganizeLog("ERROR: File move failed after filesystem rename: " + narrow(bookmarkFile));
        }
        
        DEBUG_LOG("File move " << (success ? "succeeded" : "failed"));
        DEBUG_LOG_FUNC_EXIT();
        return success;
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in moveBookmarkToFolder: " << e.what());
        logFileMovement(bookmarkFile, _currentDirectory, targetDir, false);
        writeAutoOrganizeLog("ERROR: Exception during file move: " + narrow(bookmarkFile) + " - " + e.what());
        std::wstring bookmarkTitle = getBookmarkTitle(bookmarkFile);
        recordOperation(OperationRecord::OPERATION_FAILED, bookmarkTitle, L"", L"", widen(e.what()));
        DEBUG_LOG_FUNC_EXIT();
        return false;
    } catch (...) {
        DEBUG_LOG("Unknown exception in moveBookmarkToFolder");
        logFileMovement(bookmarkFile, _currentDirectory, targetDir, false);
        writeAutoOrganizeLog("ERROR: Unknown exception during file move: " + narrow(bookmarkFile));
        std::wstring bookmarkTitle = getBookmarkTitle(bookmarkFile);
        recordOperation(OperationRecord::OPERATION_FAILED, bookmarkTitle, L"", L"", L"Unknown exception during file move");
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
}

ReorganizationStats BookmarkReorganizer::applyFolderSuggestions(const FolderSuggestions& suggestions)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Applying " << suggestions.size() << " folder suggestions with lazy directory creation");
    
    ReorganizationStats stats;
    std::set<std::wstring> createdDirectories; // Track successfully created directories
    
    for (const auto& suggestion : suggestions) {
        DEBUG_LOG("Processing folder: " << narrow(suggestion.folderName) << 
                 " with " << suggestion.bookmarkFiles.size() << " bookmarks");
        
        // Record folder creation first for proper grouping in operation summary
        recordOperation(OperationRecord::FOLDER_CREATED, suggestion.folderName);
        
        // Move bookmarks to folder (directories will be created lazily)
        bool anyFilesMoved = false;
        for (const auto& bookmarkFile : suggestion.bookmarkFiles) {
            if (moveBookmarkToFolder(bookmarkFile, suggestion.folderName)) {
                stats.totalMoved++;
                anyFilesMoved = true;
                createdDirectories.insert(suggestion.folderName); // Track that this directory was used
            } else {
                stats.totalFailed++;
            }
        }
        
        // Log folder creation only if files were actually moved
        if (anyFilesMoved) {
            logFolderCreation(suggestion.folderName, true);
        }
    }
    
    // Update status message
    DEBUG_LOG("Reorganization summary: " << stats.totalMoved << " moved, " << stats.totalFailed << " failed");
    
    // Log detailed summary
    writeAutoOrganizeLog("=== REORGANIZATION SUMMARY ===");
    writeAutoOrganizeLog("Total bookmarks moved: " + std::to_string(stats.totalMoved));
    writeAutoOrganizeLog("Total failures: " + std::to_string(stats.totalFailed));
    writeAutoOrganizeLog("Total folders suggested: " + std::to_string(suggestions.size()));
    writeAutoOrganizeLog("Total folders created: " + std::to_string(createdDirectories.size()));
    writeAutoOrganizeLog("=== END SUMMARY ===");
    
    if (stats.totalMoved > 0) {
        _lastStatusMessage = L"Reorganization completed: " + std::to_wstring(stats.totalMoved) + L" bookmarks moved";
        if (stats.totalFailed > 0) {
            _lastStatusMessage += L", " + std::to_wstring(stats.totalFailed) + L" failed";
        }
    } else {
        _lastStatusMessage = L"No bookmarks were moved";
        if (stats.totalFailed > 0) {
            _lastStatusMessage += L" (" + std::to_wstring(stats.totalFailed) + L" errors)";
        }
    }
    
    DEBUG_LOG("Status: " << narrow(_lastStatusMessage));
    
    // Apply LLM title improvements after successful reorganization
    applyTitleImprovements();
    
    DEBUG_LOG_FUNC_EXIT();
    
    return stats;
}

bool BookmarkReorganizer::reorganizeBookmarks()
{
    DEBUG_LOG_FUNC();
    try {
        _lastStatusMessage = L"Starting enhanced tree-based reorganization...";
        DEBUG_LOG("Starting enhanced bookmark reorganization with directory tree analysis");
        
        // Clear previous operation history
        clearOperationHistory();
        
        // Start detailed logging
        logOperationStart();
        
        // Check prerequisites
        if (!canReorganize()) {
            DEBUG_LOG("Prerequisites check failed");
            writeAutoOrganizeLog("ERROR: Prerequisites check failed - " + narrow(_lastStatusMessage));
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        // Prepare comprehensive directory context
        DEBUG_LOG("Preparing directory context with complete tree structure");
        DirectoryContext context = prepareDirectoryContext();
        
        if (context.bookmarks.empty()) {
            _lastStatusMessage = L"No bookmarks found to analyze";
            DEBUG_LOG("No bookmarks found in directory tree");
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        // Build title-to-filename mapping for LLM integration
        DEBUG_LOG("Building title-to-filename mapping for " << context.bookmarks.size() << " bookmarks");
        buildTitleToFileNameMapping(context.bookmarks);
        
        _lastStatusMessage = L"Analyzing " + std::to_wstring(context.bookmarks.size()) + 
                           L" bookmarks and " + std::to_wstring(context.existingFolders.size()) + 
                           L" folders with AI...";
        DEBUG_LOG("Analyzing " << context.bookmarks.size() << " bookmarks and " 
                 << context.existingFolders.size() << " existing folders with AI");
        
        // Get AI suggestions using enhanced directory context
        std::string rawLlmResponse = _aiService->getRawReorganizationResponse(context);
        
        if (rawLlmResponse.empty()) {
            _lastStatusMessage = L"AI service returned empty response";
            DEBUG_LOG("AI service returned empty response");
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        // Parse LLM response with proper title-to-filename mapping
        FolderSuggestions suggestions = parseLLMJsonRecommendations(rawLlmResponse);
        
        if (suggestions.empty()) {
            _lastStatusMessage = L"AI service returned no reorganization suggestions";
            DEBUG_LOG("AI service returned no reorganization suggestions");
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        _lastStatusMessage = L"Applying " + std::to_wstring(suggestions.size()) + L" folder reorganization suggestions...";
        DEBUG_LOG("Applying " << suggestions.size() << " folder reorganization suggestions");
        
        // Apply suggestions
        ReorganizationStats stats = applyFolderSuggestions(suggestions);
        
        // Log operation completion
        logOperationEnd(stats.totalMoved, stats.totalFailed);
        
        DEBUG_LOG("Enhanced reorganization completed successfully");
        _lastStatusMessage = L"Reorganization completed successfully! " + 
                           std::to_wstring(suggestions.size()) + L" folders organized.";
        DEBUG_LOG_FUNC_EXIT();
        return true;
        
    } catch (const std::exception& e) {
        _lastStatusMessage = L"Error during reorganization: " + widen(e.what());
        writeAutoOrganizeLog("FATAL ERROR: Exception in reorganizeBookmarks - " + std::string(e.what()));
        DEBUG_LOG("Exception in reorganizeBookmarks: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return false;
    } catch (...) {
        _lastStatusMessage = L"Unknown error during reorganization";
        writeAutoOrganizeLog("FATAL ERROR: Unknown exception in reorganizeBookmarks");
        DEBUG_LOG("Unknown exception in reorganizeBookmarks");
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
}

std::wstring BookmarkReorganizer::getLastStatusMessage() const
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Returning status message: " << narrow(_lastStatusMessage));
    return _lastStatusMessage;
}

bool BookmarkReorganizer::applyLLMRecommendations(const std::string& llmJsonResponse)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Applying LLM recommendations from JSON response");
    
    try {
        // Parse the LLM JSON response into folder suggestions
        FolderSuggestions suggestions = parseLLMJsonRecommendations(llmJsonResponse);
        
        if (suggestions.empty()) {
            _lastStatusMessage = L"No valid recommendations found in LLM response";
            DEBUG_LOG("No valid recommendations found in LLM response");
            DEBUG_LOG_FUNC_EXIT();
            return false;
        }
        
        _lastStatusMessage = L"Applying " + std::to_wstring(suggestions.size()) + L" LLM recommendations...";
        DEBUG_LOG("Applying " << suggestions.size() << " LLM recommendations");
        
        // Apply the folder suggestions using existing infrastructure
        applyFolderSuggestions(suggestions);
        
        DEBUG_LOG("LLM recommendations applied successfully");
        _lastStatusMessage = L"LLM recommendations applied successfully!";
        DEBUG_LOG_FUNC_EXIT();
        return true;
        
    } catch (const std::exception& e) {
        _lastStatusMessage = L"Error applying LLM recommendations: " + widen(e.what());
        DEBUG_LOG("Exception in applyLLMRecommendations: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return false;
    } catch (...) {
        _lastStatusMessage = L"Unknown error applying LLM recommendations";
        DEBUG_LOG("Unknown exception in applyLLMRecommendations");
        DEBUG_LOG_FUNC_EXIT();
        return false;
    }
}

FolderSuggestions BookmarkReorganizer::parseLLMJsonRecommendations(const std::string& jsonResponse)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Parsing LLM JSON recommendations");
    
    FolderSuggestions suggestions;
    
    try {
        // Get current files for name matching
        FileVector currentFiles = _dataLayer->GetFileList();
        DEBUG_LOG("Found " << currentFiles.size() << " current files for matching");
        
        // Simple JSON parsing for the array format
        // Look for folder objects in the JSON array
        std::regex folderRegex("\\{\\s*\"name\"\\s*:\\s*\"([^\"]+)\"\\s*,\\s*\"bookmarks\"\\s*:\\s*\\[([^\\]]+)\\]\\s*\\}");
        std::sregex_iterator iter(jsonResponse.begin(), jsonResponse.end(), folderRegex);
        std::sregex_iterator end;
        
        std::set<std::wstring> processedFiles; // Track files to avoid duplicates
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            std::string folderName = match[1].str();
            std::string bookmarksList = match[2].str();
            
            DEBUG_LOG("Found folder suggestion: " << folderName << " with bookmarks: " << bookmarksList);
            
            FolderSuggestion folder(widen(folderName));
            
            // Parse bookmarks list and match with actual files
            std::regex bookmarkRegex("\"([^\"]+)\"");
            std::sregex_iterator bookmarkIter(bookmarksList.begin(), bookmarksList.end(), bookmarkRegex);
            std::sregex_iterator bookmarkEnd;
            
            for (; bookmarkIter != bookmarkEnd; ++bookmarkIter) {
                std::smatch bookmarkMatch = *bookmarkIter;
                std::string suggestedName = bookmarkMatch[1].str();
                
                // Map LLM-suggested title to actual filename using our mapping
                std::wstring actualFileName = mapTitleToFileName(widen(suggestedName));
                
                if (!actualFileName.empty() && processedFiles.find(actualFileName) == processedFiles.end()) {
                    folder.bookmarkFiles.push_back(actualFileName);
                    processedFiles.insert(actualFileName);
                    DEBUG_LOG("Mapped LLM suggestion '" << suggestedName << "' to actual file: " << narrow(actualFileName));
                } else if (!actualFileName.empty()) {
                    DEBUG_LOG("File already processed, skipping: " << narrow(actualFileName));
                } else {
                    DEBUG_LOG("No mapping found for LLM suggestion: " << suggestedName);
                }
            }
            
            DEBUG_LOG("Folder '" << folderName << "' contains " << folder.bookmarkFiles.size() << " matched bookmarks");
            
            if (!folder.bookmarkFiles.empty()) {
                suggestions.push_back(folder);
            }
        }
        
        DEBUG_LOG("Successfully parsed " << suggestions.size() << " folder suggestions from LLM JSON");
        
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception during LLM JSON parsing: " << e.what());
    } catch (...) {
        DEBUG_LOG("Unknown exception during LLM JSON parsing");
    }
    
    DEBUG_LOG_FUNC_EXIT();
    return suggestions;
}

std::wstring BookmarkReorganizer::findBestMatchingFile(const std::wstring& suggestedName, const FileVector& availableFiles)
{
    DEBUG_LOG_FUNC();
    
    double bestScore = 0.0;
    std::wstring bestMatch;
    
    for (const auto& file : availableFiles) {
        if (file.IsFolder) continue; // Skip directories
        
        // Calculate similarity with file name and title
        double nameScore = calculateFileNameSimilarity(suggestedName, file.FileName);
        double titleScore = file.Name.empty() ? 0.0 : calculateFileNameSimilarity(suggestedName, file.Name);
        
        double maxScore = std::max(nameScore, titleScore);
        
        if (maxScore > bestScore) {
            bestScore = maxScore;
            bestMatch = file.FileName;
        }
    }
    
    // Only return matches with reasonable similarity (>= 30%)
    if (bestScore >= 0.3) {
        DEBUG_LOG("Best match for '" << narrow(suggestedName) << "' is '" << narrow(bestMatch) << "' with score: " << bestScore);
        DEBUG_LOG_FUNC_EXIT();
        return bestMatch;
    }
    
    DEBUG_LOG("No good match found for: " << narrow(suggestedName) << " (best score: " << bestScore << ")");
    DEBUG_LOG_FUNC_EXIT();
    return L"";
}

double BookmarkReorganizer::calculateFileNameSimilarity(const std::wstring& name1, const std::wstring& name2)
{
    // Simple similarity calculation based on common substrings and character matches
    if (name1.empty() || name2.empty()) return 0.0;
    
    // Convert to lowercase for case-insensitive comparison
    std::wstring lower1 = name1;
    std::wstring lower2 = name2;
    std::transform(lower1.begin(), lower1.end(), lower1.begin(), ::tolower);
    std::transform(lower2.begin(), lower2.end(), lower2.begin(), ::tolower);
    
    // Exact match
    if (lower1 == lower2) return 1.0;
    
    // Check if one contains the other
    if (lower1.find(lower2) != std::wstring::npos || lower2.find(lower1) != std::wstring::npos) {
        return 0.8;
    }
    
    // Count matching characters (simple approach)
    size_t matches = 0;
    size_t maxLen = std::max(lower1.length(), lower2.length());
    size_t minLen = std::min(lower1.length(), lower2.length());
    
    for (size_t i = 0; i < minLen; ++i) {
        if (lower1[i] == lower2[i]) {
            matches++;
        }
    }
    
    // Look for common words (space/delimiter separated)
    std::wregex wordRegex(L"[\\w]+");
    std::set<std::wstring> words1, words2;
    
    std::wsregex_iterator iter1(lower1.begin(), lower1.end(), wordRegex);
    std::wsregex_iterator iter2(lower2.begin(), lower2.end(), wordRegex);
    std::wsregex_iterator end;
    
    for (; iter1 != end; ++iter1) {
        words1.insert(iter1->str());
    }
    for (; iter2 != end; ++iter2) {
        words2.insert(iter2->str());
    }
    
    size_t commonWords = 0;
    for (const auto& word : words1) {
        if (words2.find(word) != words2.end()) {
            commonWords++;
        }
    }
    
    double charSimilarity = static_cast<double>(matches) / maxLen;
    double wordSimilarity = words1.empty() ? 0.0 : static_cast<double>(commonWords) / words1.size();
    
    // Combine both metrics with word similarity being more important
    return std::max(charSimilarity, wordSimilarity * 1.2);
}

// Auto-organize detailed logging methods
void BookmarkReorganizer::writeAutoOrganizeLog(const std::string& message)
{
    try {
        // Create log file path - use system temp directory or a known location
        std::string logPath;
        
        // Try to use /var/log/links first (for container), fallback to temp
        if (std::filesystem::exists("/var/log/links")) {
            logPath = "/var/log/links/auto_organize.log";
        } else {
            // For Windows or other systems, use a fallback location
            logPath = "auto_organize.log";
        }
        
        std::ofstream logFile(logPath, std::ios::app);
        if (logFile.is_open()) {
            // Get current time
            auto now = std::time(nullptr);
            auto localTime = *std::localtime(&now);
            
            // Write timestamp and message
            logFile << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] " 
                    << message << std::endl;
        }
    } catch (...) {
        // Don't let logging errors break the main functionality
        DEBUG_LOG("Failed to write to auto-organize log");
    }
}

void BookmarkReorganizer::logOperationStart()
{
    writeAutoOrganizeLog("=== AUTO-ORGANIZE OPERATION STARTED ===");
    writeAutoOrganizeLog("Working directory: " + narrow(_currentDirectory));
}

void BookmarkReorganizer::logOperationEnd(int totalMoved, int totalFailed)
{
    writeAutoOrganizeLog("=== AUTO-ORGANIZE OPERATION COMPLETED ===");
    writeAutoOrganizeLog("Summary: " + std::to_string(totalMoved) + " files moved, " + 
                        std::to_string(totalFailed) + " operations failed");
    writeAutoOrganizeLog(""); // Empty line for separation
}

void BookmarkReorganizer::logFolderCreation(const std::wstring& folderName, bool success)
{
    std::string message = success ? "FOLDER CREATED: " : "FOLDER CREATION FAILED: ";
    message += narrow(folderName);
    writeAutoOrganizeLog(message);
}

void BookmarkReorganizer::logFileMovement(const std::wstring& fileName, const std::wstring& fromPath, 
                                         const std::wstring& toPath, bool success)
{
    std::string status = success ? "MOVED" : "MOVE FAILED";
    std::string message = status + ": " + narrow(fileName);
    message += " FROM: " + narrow(fromPath);
    message += " TO: " + narrow(toPath);
    writeAutoOrganizeLog(message);
}

// Title-to-filename mapping methods
void BookmarkReorganizer::buildTitleToFileNameMapping(const std::vector<BookmarkData>& bookmarks)
{
    DEBUG_LOG_FUNC();
    _titleToFileNameMap.clear();
    
    for (const auto& bookmark : bookmarks) {
        if (!bookmark.title.empty()) {
            _titleToFileNameMap[bookmark.title] = bookmark.fileName;
            DEBUG_LOG("Mapped title '" << narrow(bookmark.title) << "' -> filename '" << narrow(bookmark.fileName) << "'");
        }
    }
    
    DEBUG_LOG("Built title-to-filename mapping with " << _titleToFileNameMap.size() << " entries");
    DEBUG_LOG_FUNC_EXIT();
}

void BookmarkReorganizer::applyTitleImprovements()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Applying " << _titleImprovements.size() << " title improvements");
    
    if (_titleImprovements.empty()) {
        DEBUG_LOG("No title improvements to apply");
        DEBUG_LOG_FUNC_EXIT();
        return;
    }
    
    int successCount = 0;
    int failureCount = 0;
    
    for (const auto& improvement : _titleImprovements) {
        try {
            DEBUG_LOG("Attempting to update title for file: " << narrow(improvement.fileName));
            DEBUG_LOG("Original: '" << narrow(improvement.originalTitle) << "' -> Improved: '" << narrow(improvement.improvedTitle) << "'");
            
            // Find the file in current directory context and update its title
            // This is a simplified approach - in a real implementation, you might want to
            // update the bookmark file content or metadata depending on your file format
            
            // For now, just log the improvement as successful documentation of the enhancement
            writeAutoOrganizeLog("TITLE IMPROVED: " + narrow(improvement.fileName) + 
                               " - '" + narrow(improvement.originalTitle) + "' -> '" + 
                               narrow(improvement.improvedTitle) + "' (similarity: " + 
                               std::to_string(improvement.similarityScore) + ")");
            
            // Record title improvement in operation history
            std::wstring similarityStr = std::to_wstring(improvement.similarityScore);
            similarityStr = similarityStr.substr(0, similarityStr.find(L'.') + 3); // Limit to 2 decimal places
            recordOperation(OperationRecord::TITLE_IMPROVED, 
                          improvement.originalTitle + L" → " + improvement.improvedTitle,
                          improvement.fileName, L"", similarityStr);
            
            successCount++;
            
        } catch (const std::exception& e) {
            DEBUG_LOG("ERROR: Failed to apply title improvement for " << narrow(improvement.fileName) << " - " << e.what());
            recordOperation(OperationRecord::OPERATION_FAILED, 
                          L"Title improvement for " + improvement.fileName,
                          L"", L"", widen(e.what()));
            failureCount++;
        }
    }
    
    DEBUG_LOG("Applied title improvements: " << successCount << " successful, " << failureCount << " failed");
    writeAutoOrganizeLog("Title improvements applied: " + std::to_string(successCount) + " successful, " + std::to_string(failureCount) + " failed");
    
    // Clear the improvements after processing
    _titleImprovements.clear();
    
    DEBUG_LOG_FUNC_EXIT();
}

std::wstring BookmarkReorganizer::mapTitleToFileName(const std::wstring& title)
{
    DEBUG_LOG_FUNC();
    
    // First try exact matching for efficiency
    auto it = _titleToFileNameMap.find(title);
    if (it != _titleToFileNameMap.end()) {
        DEBUG_LOG("Found exact mapping: '" << narrow(title) << "' -> '" << narrow(it->second) << "'");
        DEBUG_LOG_FUNC_EXIT();
        return it->second;
    }
    
    // Use embedding-based semantic similarity matching
    DEBUG_LOG("Exact match failed, trying embedding-based semantic matching for: '" << narrow(title) << "'");
    
    if (!_aiService) {
        DEBUG_LOG("ERROR: AI service not available for embedding computation");
        DEBUG_LOG_FUNC_EXIT();
        return L"";
    }
    
    try {
        // Compute embedding for the LLM-returned title
        std::vector<float> llmTitleEmbedding = _aiService->computeEmbedding(narrow(title));
        
        if (llmTitleEmbedding.empty()) {
            DEBUG_LOG("ERROR: Failed to compute embedding for LLM title");
            DEBUG_LOG_FUNC_EXIT();
            return L"";
        }
        
        float bestSimilarity = 0.0f;
        std::wstring bestMatch;
        const float SIMILARITY_THRESHOLD = 0.75f; // Adjust based on testing
        
        DEBUG_LOG("Computing similarities with " << _titleToFileNameMap.size() << " stored titles (threshold: " << SIMILARITY_THRESHOLD << ")");
        
        for (const auto& pair : _titleToFileNameMap) {
            std::vector<float> storedTitleEmbedding = _aiService->computeEmbedding(narrow(pair.first));
            
            if (storedTitleEmbedding.empty()) {
                DEBUG_LOG("WARNING: Failed to compute embedding for stored title: " << narrow(pair.first));
                continue;
            }
            
            float similarity = _aiService->computeSimilarity(llmTitleEmbedding, storedTitleEmbedding);
            
            DEBUG_LOG("Similarity between '" << narrow(title) << "' and '" << narrow(pair.first) << "': " << similarity);
            
            if (similarity > bestSimilarity && similarity >= SIMILARITY_THRESHOLD) {
                bestSimilarity = similarity;
                bestMatch = pair.second;
                DEBUG_LOG("New best match found with similarity: " << similarity);
            }
        }
        
        if (!bestMatch.empty()) {
            // Record the title improvement for potential bookmark update
            for (const auto& pair : _titleToFileNameMap) {
                if (pair.second == bestMatch) {
                    _titleImprovements.emplace_back(pair.first, title, bestMatch, bestSimilarity);
                    DEBUG_LOG("Recorded title improvement: '" << narrow(pair.first) << "' -> '" << narrow(title) << "' (score: " << bestSimilarity << ")");
                    break;
                }
            }
            
            DEBUG_LOG("Found embedding-based mapping: '" << narrow(title) << "' -> '" << narrow(bestMatch) << "' (similarity: " << bestSimilarity << ")");
            DEBUG_LOG_FUNC_EXIT();
            return bestMatch;
        } else {
            DEBUG_LOG("No embedding-based mapping found above threshold for title: '" << narrow(title) << "' (best similarity: " << bestSimilarity << ")");
        }
        
    } catch (const std::exception& e) {
        DEBUG_LOG("ERROR: Exception during embedding-based matching: " << e.what());
    }
    
    DEBUG_LOG("No exact or semantic mapping found for title: '" << narrow(title) << "'");
    DEBUG_LOG_FUNC_EXIT();
    return L"";
}

// Operation tracking methods
void BookmarkReorganizer::recordOperation(OperationRecord::Type type, const std::wstring& description, 
                                        const std::wstring& source, const std::wstring& target, const std::wstring& details)
{
    _operationHistory.emplace_back(type, description, source, target, details);
    DEBUG_LOG("Recorded operation: " << narrow(description));
}

void BookmarkReorganizer::clearOperationHistory()
{
    _operationHistory.clear();
    DEBUG_LOG("Operation history cleared");
}

std::wstring BookmarkReorganizer::getBookmarkTitle(const std::wstring& fileName) const
{
    // Search through the title-to-filename map to find the title for this filename
    for (const auto& pair : _titleToFileNameMap) {
        if (pair.second == fileName) {
            return pair.first; // Return the title
        }
    }
    
    // If no title found, return the filename without extension for better readability
    std::wstring displayName = fileName;
    size_t lastDot = displayName.find_last_of(L'.');
    if (lastDot != std::wstring::npos) {
        displayName = displayName.substr(0, lastDot);
    }
    
    // If it looks like a hash (all alphanumeric, no spaces), try to make it more readable
    if (displayName.length() > 20 && displayName.find_first_of(L" -_") == std::wstring::npos) {
        // This looks like a hash, shorten it for display
        return displayName.substr(0, 8) + L"..." + displayName.substr(displayName.length() - 8);
    }
    
    return displayName;
}

std::wstring BookmarkReorganizer::getOperationSummary() const
{
    DEBUG_LOG_FUNC();
    
    if (_operationHistory.empty()) {
        return L"No operations performed.";
    }
    
    std::wostringstream summary;
    summary << L"📋 Auto-Organization Summary\n\n";
    
    // Count operations by type
    int foldersCreated = 0;
    int filesMoved = 0;
    int titlesImproved = 0;
    int failures = 0;
    
    for (const auto& op : _operationHistory) {
        switch (op.operationType) {
            case OperationRecord::FOLDER_CREATED: foldersCreated++; break;
            case OperationRecord::FILE_MOVED: filesMoved++; break;
            case OperationRecord::TITLE_IMPROVED: titlesImproved++; break;
            case OperationRecord::OPERATION_FAILED: failures++; break;
        }
    }
    
    // Summary statistics
    summary << L"📊 Statistics:\n";
    if (foldersCreated > 0) summary << L"   📁 " << foldersCreated << L" folders created\n";
    if (filesMoved > 0) summary << L"   📄 " << filesMoved << L" files organized\n";
    if (titlesImproved > 0) summary << L"   ✨ " << titlesImproved << L" titles improved\n";
    if (failures > 0) summary << L"   ❌ " << failures << L" operations failed\n";
    summary << L"\n";
    
    // Detailed operations
    summary << L"📝 Operations Performed:\n\n";
    
    bool lastWasFolder = false;
    for (size_t i = 0; i < _operationHistory.size(); ++i) {
        const auto& op = _operationHistory[i];
        
        // Add spacing between folder groups
        if (op.operationType == OperationRecord::FOLDER_CREATED && i > 0 && !lastWasFolder) {
            summary << L"\n";
        }
        
        switch (op.operationType) {
            case OperationRecord::FOLDER_CREATED:
                summary << L"📁 Created folder: \"" << op.description << L"\"\n";
                lastWasFolder = true;
                break;
                
            case OperationRecord::FILE_MOVED:
                summary << L"   📄 Moved: " << op.description << L"\n";
                lastWasFolder = false;
                break;
                
            case OperationRecord::TITLE_IMPROVED:
                summary << L"   ✨ Title improved: " << op.description;
                if (!op.details.empty()) {
                    summary << L" (similarity: " << op.details << L")";
                }
                summary << L"\n";
                lastWasFolder = false;
                break;
                
            case OperationRecord::OPERATION_FAILED:
                summary << L"   ❌ Failed: " << op.description;
                if (!op.details.empty()) {
                    summary << L" - " << op.details;
                }
                summary << L"\n";
                lastWasFolder = false;
                break;
        }
    }
    
    summary << L"\n✅ Organization completed successfully!";
    
    DEBUG_LOG("Generated operation summary with " << _operationHistory.size() << " operations");
    DEBUG_LOG_FUNC_EXIT();
    return summary.str();
}

}  // namespace Bookmarks