#pragma once
#include "App/Globals.h"
#include "Features/BookmarkManagement/Models/Data.h"
#include "OllamaService.h"
#include "Utils/Miscellaneous.h"
#include "Shared/Repositories/IBookmarkRepository.h"
#include "Shared/Repositories/IDirectoryRepository.h"

namespace Bookmarks
{
// Structure to hold reorganization statistics
struct ReorganizationStats
{
    int totalMoved = 0;
    int totalFailed = 0;
};

// Structure to track individual operations performed
struct OperationRecord
{
    enum Type { FILE_MOVED, FOLDER_CREATED, TITLE_IMPROVED, OPERATION_FAILED };
    
    Type operationType;
    std::wstring description;
    std::wstring sourcePath;
    std::wstring targetPath;
    std::wstring details; // Additional information (e.g., similarity score, error message)
    
    OperationRecord(Type type, const std::wstring& desc, const std::wstring& source = L"", 
                   const std::wstring& target = L"", const std::wstring& extra = L"")
        : operationType(type), description(desc), sourcePath(source), targetPath(target), details(extra) {}
};

// Structure to track LLM title improvements
struct TitleImprovement
{
    std::wstring originalTitle;
    std::wstring improvedTitle;
    std::wstring fileName;
    float similarityScore;
    
    TitleImprovement(const std::wstring& orig, const std::wstring& improved, const std::wstring& file, float score)
        : originalTitle(orig), improvedTitle(improved), fileName(file), similarityScore(score) {}
};

//  Class responsible for analyzing and reorganizing bookmarks using AI
class BookmarkReorganizer
{
private:
    Data* _dataLayer;
    OllamaService* _aiService;
    std::wstring _currentDirectory;
    
    // Repository Pattern - Optional modern data access
    std::shared_ptr<IBookmarkRepository> _bookmarkRepo;
    std::shared_ptr<IDirectoryRepository> _directoryRepo;
    
    // Convert File objects to BookmarkData for AI analysis (legacy method)
    std::vector<BookmarkData> prepareBookmarksForAnalysis(const FileVector& files);
    
    // Convert directory tree to comprehensive context for AI analysis (new method)
    DirectoryContext prepareDirectoryContext();
    DirectoryContext prepareDirectoryContext(DirectoryTreeNodePtr rootNode);
    
    // Apply folder suggestions by creating folders and moving files
    ReorganizationStats applyFolderSuggestions(const FolderSuggestions& suggestions);
    
    // Create a folder if it doesn't exist
    bool createFolderIfNotExists(const std::wstring& folderName);
    
    // Move a bookmark file to a target folder
    bool moveBookmarkToFolder(const std::wstring& bookmarkFile, const std::wstring& targetFolder);
    
    // Get safe folder name (remove invalid characters)
    std::wstring getSafeFolderName(const std::wstring& originalName);
    
    // Helper methods for tree traversal
    void collectBookmarksFromTree(DirectoryTreeNodePtr node, std::vector<BookmarkData>& bookmarks, const std::wstring& currentPath = L"");
    void collectFoldersFromTree(DirectoryTreeNodePtr node, std::vector<std::wstring>& folders, const std::wstring& currentPath = L"");
    
    // Enhanced LLM recommendation processing
    FolderSuggestions parseLLMJsonRecommendations(const std::string& jsonResponse);
    std::wstring findBestMatchingFile(const std::wstring& suggestedName, const FileVector& availableFiles);
    double calculateFileNameSimilarity(const std::wstring& name1, const std::wstring& name2);
    
    // Title-to-filename mapping for LLM integration
    std::map<std::wstring, std::wstring> _titleToFileNameMap;
    std::vector<TitleImprovement> _titleImprovements; // Track LLM title improvements
    std::vector<OperationRecord> _operationHistory; // Track all operations performed
    void buildTitleToFileNameMapping(const std::vector<BookmarkData>& bookmarks);
    std::wstring mapTitleToFileName(const std::wstring& title);
    void applyTitleImprovements(); // Apply LLM-improved titles to bookmark files
    
    // Operation tracking methods
    void recordOperation(OperationRecord::Type type, const std::wstring& description, 
                        const std::wstring& source = L"", const std::wstring& target = L"", const std::wstring& details = L"");
    void clearOperationHistory();
    std::wstring getBookmarkTitle(const std::wstring& fileName) const; // Get title for filename
    
    // Auto-organize detailed logging
    void writeAutoOrganizeLog(const std::string& message);
    void logOperationStart();
    void logOperationEnd(int totalMoved, int totalFailed);
    void logFolderCreation(const std::wstring& folderName, bool success);
    void logFileMovement(const std::wstring& fileName, const std::wstring& fromPath, const std::wstring& toPath, bool success);
    
public:
    BookmarkReorganizer(Data* dataLayer, OllamaService* aiService);
    ~BookmarkReorganizer();
    
    // Repository Pattern - Set modern repository instances
    void SetRepositories(std::shared_ptr<IBookmarkRepository> bookmarkRepo, 
                        std::shared_ptr<IDirectoryRepository> directoryRepo);
    
    // Main method to reorganize bookmarks using enhanced tree-based analysis
    bool reorganizeBookmarks();
    
    // Check if reorganization is possible (AI service available, etc.)
    bool canReorganize();
    
    // Apply LLM recommendations directly from JSON format
    bool applyLLMRecommendations(const std::string& llmJsonResponse);
    
    // Get status message for user feedback
    std::wstring getLastStatusMessage() const;
    
    // Get formatted summary of operations performed
    std::wstring getOperationSummary() const;
    
private:
    std::wstring _lastStatusMessage;
};
}  // namespace Bookmarks