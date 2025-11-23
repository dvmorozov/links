#pragma once
#include "App/Globals.h"
#include "Shared/Services/OllamaClient.h"
#include <string>
#include <vector>
#include <memory>

namespace Bookmarks
{
//  Structure representing a folder suggestion from AI
struct FolderSuggestion
{
    std::wstring folderName;
    std::vector<std::wstring> bookmarkFiles;
    
    FolderSuggestion(const std::wstring& name) : folderName(name) {}
};

typedef std::vector<FolderSuggestion> FolderSuggestions;

//  Structure representing bookmark data for AI analysis
struct BookmarkData
{
    std::wstring fileName;
    std::wstring title;
    std::wstring url;
    std::wstring domain;
    std::wstring currentPath;  // Current location in directory tree
    
    BookmarkData(const std::wstring& file, const std::wstring& t, const std::wstring& u, const std::wstring& d, const std::wstring& path = L"")
        : fileName(file), title(t), url(u), domain(d), currentPath(path) {}
};

//  Structure representing complete directory context for AI analysis
struct DirectoryContext
{
    std::vector<BookmarkData> bookmarks;          // All bookmarks in the tree
    std::vector<std::wstring> existingFolders;    // Current folder structure
    std::wstring rootPath;                        // Root directory being analyzed
    
    DirectoryContext(const std::wstring& root = L"") : rootPath(root) {}
};

//  Service for communicating with Ollama API for bookmark organization
class OllamaService
{
private:
    std::unique_ptr<Shared::OllamaClient> _client;
    std::string _baseUrl;  // Keep for backward compatibility
    std::string _model;    // Keep for backward compatibility
    std::string _embeddingModel;  // Model for embeddings (different from text generation)
    int _timeoutSeconds;   // Keep for backward compatibility
    static const size_t MAX_RESPONSE_SIZE = 50 * 1024;  // 50KB max response size
    static const size_t MAX_REPETITION_COUNT = 3;       // Max allowed repetitions
    
    // HTTP client functions (now delegate to OllamaClient)
    std::string sendHttpRequest(const std::string& url, const std::string& jsonPayload);
    std::string buildJsonPayload(const std::vector<BookmarkData>& bookmarks);
    std::string buildJsonPayloadWithContext(const DirectoryContext& context);
    
    // Bookmark-specific prompt building
    std::string buildBookmarkOrganizationPrompt(const std::vector<BookmarkData>& bookmarks);
    std::string buildDirectoryReorganizationPrompt(const DirectoryContext& context);
    
    // JSON logging for debugging LLM interactions
    void writeJsonLog(const std::string& type, const std::string& content);
    
    // Helper method to extract clean response content from Ollama API response
    std::string extractResponseContent(const std::string& response);
    
    // Helper method to escape strings for JSON
    std::string escapeJsonString(const std::string& input);
    
    // Response validation methods
    bool validateResponseSize(const std::string& response);
    bool detectRepetitiveContent(const std::string& response);
    bool validateResponseQuality(const FolderSuggestions& suggestions, size_t expectedBookmarkCount);
    bool validateBookmarkResponse(const FolderSuggestions& suggestions, size_t expectedBookmarkCount);
    std::string truncateResponse(const std::string& response);
    std::string handlePartialResponse(const std::string& response);
    
public:
    OllamaService(const std::string& baseUrl = "http://localhost:11435", 
                  const std::string& model = "llama2",
                  int timeout = 30,
                  const std::string& embeddingModel = "nomic-embed-text");
    
    // Check if Ollama service is available
    bool isAvailable();
    
    // Original method for backward compatibility
    FolderSuggestions categorizeBookmarks(const std::vector<BookmarkData>& bookmarks);
    
    // Enhanced method with complete directory context
    FolderSuggestions reorganizeDirectory(const DirectoryContext& context);
    
    // Get raw LLM response for external parsing 
    std::string getRawReorganizationResponse(const DirectoryContext& context);
    
    // Parse AI response into folder suggestions
    FolderSuggestions parseResponse(const std::string& response);
    
    // Embedding-based title matching (kept for compatibility with BookmarkReorganizer)
    std::vector<float> computeEmbedding(const std::string& text);
    float computeSimilarity(const std::vector<float>& embedding1, const std::vector<float>& embedding2);
};
}  // namespace Bookmarks