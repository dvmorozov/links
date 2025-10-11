#include "App/stdafx.h"
#include "OllamaService.h"
#include "Utils/Miscellaneous.h"
#include "Utils/DebugLog.h"

#ifdef _WINDOWS
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <sstream>
#include <regex>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <typeinfo>
#include <map>
#include <set>
#include <cmath>

namespace Bookmarks
{

OllamaService::OllamaService(const std::string& baseUrl, const std::string& model, int timeout)
    : _baseUrl(baseUrl), _model(model), _timeoutSeconds(timeout)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("OllamaService created with baseUrl: " << baseUrl << ", model: " << model << ", timeout: " << timeout);
}

bool OllamaService::isAvailable()
{
    DEBUG_LOG_FUNC();
    try {
        std::string testUrl = _baseUrl + "/api/tags";
        DEBUG_LOG("Testing connection to: " << testUrl);
        
        std::string response = sendHttpRequest(testUrl, "");
        DEBUG_LOG("Response length: " << response.length());
        
        if (response.empty()) {
            DEBUG_LOG("ERROR: Empty response from Ollama service");
            return false;
        }
        
        bool hasModels = response.find("models") != std::string::npos;
        DEBUG_LOG("Response contains 'models': " << (hasModels ? "YES" : "NO"));
        
        if (!hasModels) {
            DEBUG_LOG("Response preview: " << response.substr(0, 200));
        }
        
        return hasModels;
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception in isAvailable(): " << e.what());
        DEBUG_LOG("Exception type: " << typeid(e).name());
        return false;
    } catch (...) {
        DEBUG_LOG("Unknown exception in isAvailable()");
        return false;
    }
}

std::string OllamaService::buildJsonPayload(const std::vector<BookmarkData>& bookmarks)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Building JSON payload for " << bookmarks.size() << " bookmarks");
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"model\": \"" << _model << "\",\n";
    json << "  \"prompt\": \"You are an expert bookmark organizer. Your task is to analyze bookmarks and create a logical folder structure. ";
    json << "IMPORTANT: You MUST return valid JSON with organized folders. Empty or invalid responses are not acceptable.\\n\\n";
    json << "RULES:\\n";
    json << "1. Create 3-8 meaningful folder categories\\n";
    json << "2. Each folder must contain at least 1 bookmark\\n";
    json << "3. Group bookmarks by topic, purpose, or domain type\\n";
    json << "4. Use clear, descriptive folder names (e.g., 'Development Tools', 'News & Media', 'Social Networks')\\n";
    json << "5. Distribute bookmarks evenly across folders\\n\\n";
    json << "Bookmarks to analyze:\\n";
    
    for (const auto& bookmark : bookmarks) {
        std::string title = narrow(bookmark.title);
        std::string url = narrow(bookmark.url);
        std::string fileName = narrow(bookmark.fileName);
        
        // Escape quotes in JSON
        std::string escapedTitle = std::regex_replace(title, std::regex("\""), "\\\"");
        std::string escapedUrl = std::regex_replace(url, std::regex("\""), "\\\"");
        
        json << "- Title: \\\"" << escapedTitle << "\\\" ";
        json << "URL: \\\"" << escapedUrl << "\\\" ";
        json << "File: \\\"" << fileName << "\\\"\\n";
    }
    
    json << "\\nEXAMPLE OUTPUT FORMAT:\\n";
    json << "{\\\"folders\\\":[";
    json << "{\\\"name\\\":\\\"Development\\\",\\\"bookmarks\\\":[\\\"github.txt\\\",\\\"stackoverflow.txt\\\"]},";
    json << "{\\\"name\\\":\\\"News\\\",\\\"bookmarks\\\":[\\\"bbc.txt\\\",\\\"cnn.txt\\\"]}";
    json << "]}\\n\\n";
    json << "OUTPUT REQUIREMENTS:\\n";
    json << "- Return ONLY valid JSON, no explanations or other text\\n";
    json << "- Use exact filename from the bookmark list\\n";
    json << "- Create meaningful folder names\\n";
    json << "- Ensure every bookmark is assigned to exactly one folder\",\n";
    json << "  \"stream\": false\n";
    json << "}";
    
    std::string result = json.str();
    
    // Log the complete JSON payload for debugging
    DEBUG_LOG("=== LLM JSON PAYLOAD START ===");
    DEBUG_LOG("Full JSON payload being sent to LLM:");
    DEBUG_LOG(result);
    DEBUG_LOG("=== LLM JSON PAYLOAD END ===");
    
    // Write to separate JSON log file
    writeJsonLog("request", result);
    
    return result;
}

#ifdef _WINDOWS
std::string OllamaService::sendHttpRequest(const std::string& url, const std::string& jsonPayload)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Sending HTTP request to: " << url);
    DEBUG_LOG("Payload length: " << jsonPayload.length());
    
    std::string result;
    
    // Parse URL
    std::string host, path;
    int port = 80;
    
    size_t protocolEnd = url.find("://");
    if (protocolEnd == std::string::npos) return "";
    
    size_t hostStart = protocolEnd + 3;
    size_t pathStart = url.find("/", hostStart);
    
    if (pathStart == std::string::npos) {
        host = url.substr(hostStart);
        path = "/";
    } else {
        host = url.substr(hostStart, pathStart - hostStart);
        path = url.substr(pathStart);
    }
    
    size_t portPos = host.find(":");
    if (portPos != std::string::npos) {
        port = std::stoi(host.substr(portPos + 1));
        host = host.substr(0, portPos);
    }
    
    // Initialize WinINet
    HINTERNET hInternet = InternetOpenA("Links-OllamaClient", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
    if (!hInternet) return "";
    
    // Set timeout (convert seconds to milliseconds)
    DWORD timeoutMs = _timeoutSeconds * 1000;
    InternetSetOptionA(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionA(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof(timeoutMs));
    
    // Connect to server
    HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), port, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }
    
    // Create request
    const char* method = jsonPayload.empty() ? "GET" : "POST";
    HINTERNET hRequest = HttpOpenRequestA(hConnect, method, path.c_str(), nullptr, nullptr, nullptr, 0, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return "";
    }
    
    // Send request
    const char* headers = "Content-Type: application/json\r\n";
    BOOL success;
    if (jsonPayload.empty()) {
        success = HttpSendRequestA(hRequest, nullptr, 0, nullptr, 0);
    } else {
        success = HttpSendRequestA(hRequest, headers, -1, (LPVOID)jsonPayload.c_str(), jsonPayload.length());
    }
    
    if (success) {
        // Read response
        char buffer[4096];
        DWORD bytesRead;
        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            result += buffer;
        }
    }
    
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    
    return result;
}
#else
std::string OllamaService::sendHttpRequest(const std::string& url, const std::string& jsonPayload)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Sending HTTP request to: " << url);
    DEBUG_LOG("Payload length: " << jsonPayload.length());
    DEBUG_LOG("Payload: " << jsonPayload);
    
    std::string result;
    
    // Parse URL
    std::string host, path;
    int port = 80;
    
    size_t protocolEnd = url.find("://");
    if (protocolEnd == std::string::npos) return "";
    
    size_t hostStart = protocolEnd + 3;
    size_t pathStart = url.find("/", hostStart);
    
    if (pathStart == std::string::npos) {
        host = url.substr(hostStart);
        path = "/";
    } else {
        host = url.substr(hostStart, pathStart - hostStart);
        path = url.substr(pathStart);
    }
    
    size_t portPos = host.find(":");
    if (portPos != std::string::npos) {
        try {
            port = std::stoi(host.substr(portPos + 1));
            host = host.substr(0, portPos);
        } catch (const std::exception& e) {
            DEBUG_LOG("ERROR: Failed to parse port number from URL: " << url << " - " << e.what());
            DEBUG_LOG_FUNC_EXIT();
            return "";
        }
    }
    
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return "";
    
    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = _timeoutSeconds;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    // Resolve hostname
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        close(sockfd);
        return "";
    }
    
    // Connect to server
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        return "";
    }
    
    // Build HTTP request
    std::ostringstream request;
    if (jsonPayload.empty()) {
        request << "GET " << path << " HTTP/1.1\r\n";
        request << "Host: " << host << "\r\n";
        request << "Connection: close\r\n\r\n";
    } else {
        request << "POST " << path << " HTTP/1.1\r\n";
        request << "Host: " << host << "\r\n";
        request << "Content-Type: application/json\r\n";
        request << "Content-Length: " << jsonPayload.length() << "\r\n";
        request << "Connection: close\r\n\r\n";
        request << jsonPayload;
    }
    
    std::string requestStr = request.str();
    
    // Send request
    if (send(sockfd, requestStr.c_str(), requestStr.length(), 0) < 0) {
        close(sockfd);
        return "";
    }
    
    // Read response
    char buffer[4096];
    ssize_t bytesRead;
    std::string response;
    while ((bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }
    
    close(sockfd);
    
    DEBUG_LOG("Raw HTTP response length: " << response.length());
    DEBUG_LOG("Raw HTTP response: " << response);
    
    // Extract body from HTTP response
    size_t bodyStart = response.find("\r\n\r\n");
    if (bodyStart != std::string::npos) {
        result = response.substr(bodyStart + 4);
        DEBUG_LOG("Extracted HTTP body length: " << result.length());
        DEBUG_LOG("Extracted HTTP body: " << result);
        
        // Check if this is chunked transfer encoding
        std::string headers = response.substr(0, bodyStart);
        if (headers.find("Transfer-Encoding: chunked") != std::string::npos) {
            DEBUG_LOG("Detected chunked transfer encoding");
            
            // Parse chunked data: first line is chunk size in hex
            size_t chunkSizeEnd = result.find('\n');
            if (chunkSizeEnd != std::string::npos) {
                std::string chunkSizeStr = result.substr(0, chunkSizeEnd);
                // Remove carriage return if present
                if (!chunkSizeStr.empty() && chunkSizeStr.back() == '\r') {
                    chunkSizeStr.pop_back();
                }
                
                DEBUG_LOG("Chunk size string: " << chunkSizeStr);
                
                // Extract the actual JSON data after the chunk size line
                std::string jsonData = result.substr(chunkSizeEnd + 1);
                
                // Remove the trailing chunk terminator (0\r\n\r\n or just 0)
                size_t endMarker = jsonData.find("\n0");
                if (endMarker != std::string::npos) {
                    jsonData = jsonData.substr(0, endMarker);
                }
                
                result = jsonData;
                DEBUG_LOG("Final extracted JSON length: " << result.length());
                DEBUG_LOG("Final extracted JSON: " << result);
            }
        }
    } else {
        DEBUG_LOG("No HTTP body separator found!");
    }
    
    return result;
}
#endif

FolderSuggestions OllamaService::parseResponse(const std::string& response)
{
    FolderSuggestions suggestions;
    
    try {
        // Use the enhanced extractResponseContent method
        std::string responseContent = extractResponseContent(response);
        
        // Log the extracted response content before parsing
        DEBUG_LOG("=== LLM RESPONSE CONTENT START ===");
        DEBUG_LOG("Extracted response content from JSON:");
        DEBUG_LOG(responseContent);
        DEBUG_LOG("=== LLM RESPONSE CONTENT END ===");
        
        // Parse folder suggestions from response content
        // Look for "folders" array
        size_t foldersStart = responseContent.find("\"folders\"");
        if (foldersStart == std::string::npos) return suggestions;
        
        // Simple parsing - look for folder objects
        std::regex folderRegex("\\{[^}]*\"name\"\\s*:\\s*\"([^\"]+)\"[^}]*\"bookmarks\"\\s*:\\s*\\[([^\\]]+)\\]");
        std::sregex_iterator iter(responseContent.begin(), responseContent.end(), folderRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            std::string folderName = match[1].str();
            std::string bookmarksList = match[2].str();
            
            DEBUG_LOG("Found folder: " << folderName << " with bookmarks: " << bookmarksList);
            
            FolderSuggestion folder(widen(folderName));
            
            // Parse bookmarks list
            std::regex bookmarkRegex("\"([^\"]+)\"");
            std::sregex_iterator bookmarkIter(bookmarksList.begin(), bookmarksList.end(), bookmarkRegex);
            std::sregex_iterator bookmarkEnd;
            
            for (; bookmarkIter != bookmarkEnd; ++bookmarkIter) {
                std::smatch bookmarkMatch = *bookmarkIter;
                folder.bookmarkFiles.push_back(widen(bookmarkMatch[1].str()));
            }
            
            DEBUG_LOG("Folder " << folderName << " contains " << folder.bookmarkFiles.size() << " bookmarks");
            
            if (!folder.bookmarkFiles.empty()) {
                suggestions.push_back(folder);
            }
        }
    } catch (const std::exception& e) {
        DEBUG_LOG("Exception during parsing: " << e.what());
    } catch (...) {
        DEBUG_LOG("Unknown exception during parsing");
    }
    
    // Log detailed summary of parsed suggestions
    DEBUG_LOG("=== LLM PARSED SUGGESTIONS START ===");
    DEBUG_LOG("Successfully parsed " << suggestions.size() << " folder suggestions from LLM response:");
    
    for (size_t i = 0; i < suggestions.size(); ++i) {
        const auto& suggestion = suggestions[i];
        DEBUG_LOG("Folder " << (i+1) << ": \"" << narrow(suggestion.folderName) << "\" (" << suggestion.bookmarkFiles.size() << " bookmarks)");
        
        for (size_t j = 0; j < suggestion.bookmarkFiles.size(); ++j) {
            DEBUG_LOG("  - Bookmark " << (j+1) << ": \"" << narrow(suggestion.bookmarkFiles[j]) << "\"");
        }
    }
    
    if (suggestions.empty()) {
        DEBUG_LOG("WARNING: No valid folder suggestions were parsed from LLM response");
    }
    
    DEBUG_LOG("=== LLM PARSED SUGGESTIONS END ===");
    DEBUG_LOG("Parsed " << suggestions.size() << " folder suggestions");
    DEBUG_LOG_FUNC_EXIT();
    return suggestions;
}

FolderSuggestions OllamaService::categorizeBookmarks(const std::vector<BookmarkData>& bookmarks)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Categorizing " << bookmarks.size() << " bookmarks");
    
    if (bookmarks.empty()) {
        DEBUG_LOG("No bookmarks to categorize");
        DEBUG_LOG_FUNC_EXIT();
        return FolderSuggestions();
    }
    
    DEBUG_LOG("Checking if Ollama service is available");
    if (!isAvailable()) {
        DEBUG_LOG("Ollama service is not available");
        throw std::runtime_error("Ollama service is not available");
    }
    
    DEBUG_LOG("Building JSON payload");
    std::string jsonPayload = buildJsonPayload(bookmarks);
    std::string url = _baseUrl + "/api/generate";
    
    DEBUG_LOG("Sending request to: " << url);
    std::string response = sendHttpRequest(url, jsonPayload);
    
    if (response.empty()) {
        DEBUG_LOG("Empty response from Ollama service");
        throw std::runtime_error("Failed to get response from Ollama service");
    }
    
    // Log the complete response for debugging
    DEBUG_LOG("=== LLM RESPONSE START ===");
    DEBUG_LOG("Raw response from LLM service:");
    DEBUG_LOG("Response length: " << response.length() << " characters");
    DEBUG_LOG(response);
    DEBUG_LOG("=== LLM RESPONSE END ===");
    
    // Write response to separate JSON log file
    std::string cleanResponse = extractResponseContent(response);
    writeJsonLog("response", cleanResponse);
    
    DEBUG_LOG("Received response, parsing");
    FolderSuggestions result = parseResponse(response);
    
    // Validate the quality of the response
    if (!validateResponseQuality(result, bookmarks.size())) {
        DEBUG_LOG("Response quality validation failed for categorization");
        writeJsonLog("validation_warning", "Poor quality categorization response detected");
    }
    
    DEBUG_LOG("Categorization completed with " << result.size() << " suggestions");
    DEBUG_LOG_FUNC_EXIT();
    return result;
}

std::string OllamaService::escapeJsonString(const std::string& input)
{
    std::string output;
    output.reserve(input.length() * 2); // Pre-allocate some space
    
    for (char c : input) {
        switch (c) {
            case '"':  output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    // Control characters
                    output += "\\u";
                    char buf[5];
                    snprintf(buf, sizeof(buf), "%04x", static_cast<unsigned char>(c));
                    output += buf;
                } else {
                    output += c;
                }
                break;
        }
    }
    return output;
}

std::string OllamaService::buildJsonPayloadWithContext(const DirectoryContext& context)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Building intelligent categorization prompt for LLM");
    DEBUG_LOG("Root path: " << narrow(context.rootPath));
    DEBUG_LOG("Total bookmarks: " << context.bookmarks.size());
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"model\": \"" << _model << "\",\n";
    json << "  \"prompt\": \"Organize these bookmarks into logical folders based on their meaning and purpose: ";
    
    // Add existing directories
    if (!context.existingFolders.empty()) {
        json << "Existing folders: ";
        bool firstFolder = true;
        for (const auto& folder : context.existingFolders) {
            if (!firstFolder) json << ", ";
            std::string folderName = narrow(folder);
            if (folderName.find("./") == 0) {
                folderName = folderName.substr(2);
            }
            json << "\\\"" << escapeJsonString(folderName) << "\\\"";
            firstFolder = false;
        }
        json << ". Bookmarks to organize: ";
    }
    
    // Add actual bookmark files from context
    bool first = true;
    for (const auto& bookmark : context.bookmarks) {
        if (!first) json << ", ";
        json << "\\\"" << escapeJsonString(narrow(bookmark.title)) << "\\\"";
        first = false;
    }
    
    json << ". Return JSON in this format: {\\\"folders\\\":[{\\\"name\\\":\\\"folder_name\\\",\\\"bookmarks\\\":[\\\"bookmark1\\\",\\\"bookmark2\\\"]}]}. Example: for bookmarks like 'GitHub - Development Platform', 'Facebook - Social Network', 'Netflix - Entertainment' you might create: {\\\"folders\\\":[{\\\"name\\\":\\\"Development\\\",\\\"bookmarks\\\":[\\\"GitHub - Development Platform\\\"]},{\\\"name\\\":\\\"Social\\\",\\\"bookmarks\\\":[\\\"Facebook - Social Network\\\"]},{\\\"name\\\":\\\"Entertainment\\\",\\\"bookmarks\\\":[\\\"Netflix - Entertainment\\\"]}]}. No markdown, no explanations.\",\n";
    json << "  \"stream\": false\n";
    json << "}";
    
    std::string result = json.str();
    
    DEBUG_LOG("=== INTELLIGENT CATEGORIZATION PROMPT START ===");
    DEBUG_LOG(result);
    DEBUG_LOG("=== INTELLIGENT CATEGORIZATION PROMPT END ===");
    
    writeJsonLog("intelligent_categorization_request", result);
    
    DEBUG_LOG_FUNC_EXIT();
    return result;
}

FolderSuggestions OllamaService::reorganizeDirectory(const DirectoryContext& context)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Reorganizing directory with complete context");
    DEBUG_LOG("Root path: " << narrow(context.rootPath));
    DEBUG_LOG("Bookmarks count: " << context.bookmarks.size());
    DEBUG_LOG("Existing folders count: " << context.existingFolders.size());
    
    if (context.bookmarks.empty()) {
        DEBUG_LOG("No bookmarks to reorganize");
        DEBUG_LOG_FUNC_EXIT();
        return FolderSuggestions();
    }
    
    DEBUG_LOG("Checking if Ollama service is available");
    if (!isAvailable()) {
        DEBUG_LOG("Ollama service is not available");
        throw std::runtime_error("Ollama service is not available");
    }
    
    DEBUG_LOG("Building JSON payload with directory context");
    std::string jsonPayload = buildJsonPayloadWithContext(context);
    std::string url = _baseUrl + "/api/generate";
    
    DEBUG_LOG("Sending request to: " << url);
    std::string response = sendHttpRequest(url, jsonPayload);
    
    if (response.empty()) {
        DEBUG_LOG("Empty response from Ollama service");
        throw std::runtime_error("Failed to get response from Ollama service");
    }
    
    // Validate response size and content
    if (!validateResponseSize(response)) {
        DEBUG_LOG("Response size validation failed, truncating");
        response = truncateResponse(response);
    }
    
    if (detectRepetitiveContent(response)) {
        DEBUG_LOG("Repetitive content detected, response may be invalid");
        writeJsonLog("validation_warning", "Repetitive content detected in LLM response");
        // Continue processing but log the issue
    }
    
    // Handle potential partial responses
    response = handlePartialResponse(response);
    
    // Log the complete response for debugging
    DEBUG_LOG("=== LLM DIRECTORY REORGANIZATION RESPONSE START ===");
    DEBUG_LOG("Raw response from LLM service:");
    DEBUG_LOG("Response length: " << response.length() << " characters");
    DEBUG_LOG(response);
    DEBUG_LOG("=== LLM DIRECTORY REORGANIZATION RESPONSE END ===");
    
    // Write response to separate JSON log file
    std::string cleanResponse = extractResponseContent(response);
    writeJsonLog("context_response", cleanResponse);
    
    DEBUG_LOG("Received response, parsing");
    FolderSuggestions result = parseResponse(response);
    
    // Validate the quality of the response
    if (!validateResponseQuality(result, context.bookmarks.size())) {
        DEBUG_LOG("Response quality validation failed, attempting retry with more specific prompt");
        writeJsonLog("validation_warning", "Poor quality response detected, suggestions may be insufficient");
        // Note: In a production system, you might want to retry with a different prompt
    }
    
    DEBUG_LOG("Directory reorganization completed with " << result.size() << " suggestions");
    DEBUG_LOG_FUNC_EXIT();
    return result;
}

std::string OllamaService::getRawReorganizationResponse(const DirectoryContext& context)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Getting raw reorganization response for external parsing");
    DEBUG_LOG("Root path: " << narrow(context.rootPath));
    DEBUG_LOG("Bookmarks count: " << context.bookmarks.size());
    DEBUG_LOG("Existing folders count: " << context.existingFolders.size());
    
    if (context.bookmarks.empty()) {
        DEBUG_LOG("No bookmarks to reorganize");
        DEBUG_LOG_FUNC_EXIT();
        return "";
    }
    
    DEBUG_LOG("Checking if Ollama service is available");
    if (!isAvailable()) {
        DEBUG_LOG("Ollama service is not available");
        throw std::runtime_error("Ollama service is not available");
    }
    
    DEBUG_LOG("Building JSON payload with directory context");
    std::string jsonPayload = buildJsonPayloadWithContext(context);
    std::string url = _baseUrl + "/api/generate";
    
    DEBUG_LOG("Sending request to: " << url);
    std::string response = sendHttpRequest(url, jsonPayload);
    
    if (response.empty()) {
        DEBUG_LOG("Empty response from Ollama service");
        throw std::runtime_error("Failed to get response from Ollama service");
    }
    
    // Extract the actual LLM response content
    std::string cleanResponse = extractResponseContent(response);
    
    DEBUG_LOG("=== RAW LLM RESPONSE FOR EXTERNAL PARSING START ===");
    DEBUG_LOG("Raw response content:");
    DEBUG_LOG("Response length: " << cleanResponse.length() << " characters");
    DEBUG_LOG(cleanResponse);
    DEBUG_LOG("=== RAW LLM RESPONSE FOR EXTERNAL PARSING END ===");
    
    DEBUG_LOG("Returning raw response for external parsing");
    DEBUG_LOG_FUNC_EXIT();
    return cleanResponse;
}

void OllamaService::writeJsonLog(const std::string& type, const std::string& content)
{
    try {
        // Get current timestamp
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        
        std::ostringstream timestamp;
        timestamp << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        
        // Create JSON object - check if content is already JSON
        std::ostringstream jsonLog;
        jsonLog << "{";
        jsonLog << "\"timestamp\":\"" << timestamp.str() << "\",";
        jsonLog << "\"type\":\"" << type << "\",";
        jsonLog << "\"content\":";
        
        // Check if content looks like JSON (starts with { or [)
        std::string trimmed = content;
        // Remove leading whitespace
        size_t start = trimmed.find_first_not_of(" \t\n\r");
        if (start != std::string::npos) {
            trimmed = trimmed.substr(start);
        }
        
        if (!trimmed.empty() && (trimmed[0] == '{' || trimmed[0] == '[')) {
            // Content is already JSON, convert escaped sequences to actual characters for readability
            std::string readable = content;
            readable = std::regex_replace(readable, std::regex("\\\\n"), "\n");
            readable = std::regex_replace(readable, std::regex("\\\\\""), "\"");
            jsonLog << readable;
        } else {
            // Content is plain text, convert escaped sequences to actual characters for readability
            std::string readable = content;
            
            // Convert escaped sequences to actual characters
            readable = std::regex_replace(readable, std::regex("\\\\n"), "\n");
            readable = std::regex_replace(readable, std::regex("\\\\\""), "\"");
            readable = std::regex_replace(readable, std::regex("\\\\r"), "\r");
            readable = std::regex_replace(readable, std::regex("\\\\t"), "\t");
            
            // Now escape it properly for JSON, but preserve actual line breaks
            std::string escaped = readable;
            
            // Replace backslashes first (must be done before quotes)
            std::string::size_type pos = 0;
            while ((pos = escaped.find("\\", pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\\");
                pos += 2;
            }
            
            // Replace quotes
            pos = 0;
            while ((pos = escaped.find("\"", pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\"");
                pos += 2;
            }
            
            // Replace carriage returns (but keep actual newlines for readability)
            pos = 0;
            while ((pos = escaped.find("\r", pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\r");
                pos += 2;
            }
            
            // Replace tabs
            pos = 0;
            while ((pos = escaped.find("\t", pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\t");
                pos += 2;
            }
            
            // Note: We intentionally do NOT escape \n (newlines) to preserve readability
            
            jsonLog << "\"" << escaped << "\"";
        }
        
        jsonLog << "}";
        
        // Write to log file in same directory as debug log
        std::ofstream logFile("/var/log/links/llm_json.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << jsonLog.str() << std::endl;
            logFile.close();
        }
    } catch (...) {
        // Silently ignore logging errors to avoid disrupting main functionality
    }
}

std::string OllamaService::extractResponseContent(const std::string& response)
{
    try {
        // Extract the "response" field content from Ollama API response
        size_t responseStart = response.find("\"response\":");
        if (responseStart == std::string::npos) return response; // Return original if not found
        
        responseStart = response.find("\"", responseStart + 11);
        if (responseStart == std::string::npos) return response;
        responseStart++;
        
        // Find the end of the JSON string by counting escape sequences
        size_t pos = responseStart;
        size_t responseEnd = std::string::npos;
        
        while (pos < response.length()) {
            if (response[pos] == '\\') {
                // Skip escaped character
                pos += 2;
                continue;
            }
            if (response[pos] == '"') {
                // Found unescaped quote - this should be the end
                responseEnd = pos;
                break;
            }
            pos++;
        }
        
        if (responseEnd == std::string::npos) return response;
        
        std::string responseContent = response.substr(responseStart, responseEnd - responseStart);
        
        // Unescape JSON
        responseContent = std::regex_replace(responseContent, std::regex("\\\\n"), "\n");
        responseContent = std::regex_replace(responseContent, std::regex("\\\\\""), "\"");
        responseContent = std::regex_replace(responseContent, std::regex("\\\\\\\\"), "\\");
        
        // Handle Unicode escape sequences (\uXXXX)
        std::regex unicodeRegex("\\\\u([0-9a-fA-F]{4})");
        std::sregex_iterator iter(responseContent.begin(), responseContent.end(), unicodeRegex);
        std::sregex_iterator end;
        
        std::string result = responseContent;
        std::vector<std::pair<size_t, std::pair<size_t, std::string>>> replacements;
        
        for (auto it = iter; it != end; ++it) {
            std::smatch match = *it;
            std::string hexStr = match[1].str();
            
            // Convert hex string to Unicode code point
            unsigned int codePoint = std::stoul(hexStr, nullptr, 16);
            
            // Convert Unicode code point to UTF-8
            std::string utf8Char;
            if (codePoint <= 0x7F) {
                // ASCII character
                utf8Char = static_cast<char>(codePoint);
            } else if (codePoint <= 0x7FF) {
                // 2-byte UTF-8
                utf8Char += static_cast<char>(0xC0 | (codePoint >> 6));
                utf8Char += static_cast<char>(0x80 | (codePoint & 0x3F));
            } else if (codePoint <= 0xFFFF) {
                // 3-byte UTF-8
                utf8Char += static_cast<char>(0xE0 | (codePoint >> 12));
                utf8Char += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
                utf8Char += static_cast<char>(0x80 | (codePoint & 0x3F));
            } else {
                // 4-byte UTF-8 (for completeness, though unlikely in folder names)
                utf8Char += static_cast<char>(0xF0 | (codePoint >> 18));
                utf8Char += static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F));
                utf8Char += static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F));
                utf8Char += static_cast<char>(0x80 | (codePoint & 0x3F));
            }
            
            replacements.push_back({static_cast<size_t>(match.position()), {match.length(), utf8Char}});
        }
        
        // Apply replacements in reverse order to maintain correct positions
        for (auto it = replacements.rbegin(); it != replacements.rend(); ++it) {
            result.replace(it->first, it->second.first, it->second.second);
        }
        
        responseContent = result;
        
        // Remove markdown code blocks if present
        responseContent = std::regex_replace(responseContent, std::regex("```[^\\n]*\\n?"), "");
        responseContent = std::regex_replace(responseContent, std::regex("```$"), "");
        
        // Remove leading/trailing whitespace
        size_t start = responseContent.find_first_not_of(" \t\n\r");
        if (start != std::string::npos) {
            size_t end = responseContent.find_last_not_of(" \t\n\r");
            responseContent = responseContent.substr(start, end - start + 1);
        }
        
        return responseContent;
    } catch (...) {
        return response; // Return original if parsing fails
    }
}

bool OllamaService::validateResponseSize(const std::string& response)
{
    DEBUG_LOG_FUNC();
    bool isValid = response.length() <= MAX_RESPONSE_SIZE;
    if (!isValid) {
        DEBUG_LOG("Response too large: " << response.length() << " bytes (max: " << MAX_RESPONSE_SIZE << ")");
        writeJsonLog("validation_error", "Response size exceeded maximum limit");
    }
    return isValid;
}

bool OllamaService::detectRepetitiveContent(const std::string& response)
{
    DEBUG_LOG_FUNC();
    
    // Look for repeated patterns in the response
    // Check for the same bookmark title appearing multiple times
    std::regex titlePattern("\"title\"\\s*:\\s*\"([^\"]+)\"");
    std::sregex_iterator iter(response.begin(), response.end(), titlePattern);
    std::sregex_iterator end;
    
    std::map<std::string, size_t> titleCounts;
    
    while (iter != end) {
        std::string title = (*iter)[1].str();
        titleCounts[title]++;
        ++iter;
    }
    
    // Check if any title appears more than the allowed limit
    for (const auto& pair : titleCounts) {
        if (pair.second > MAX_REPETITION_COUNT) {
            DEBUG_LOG("Repetitive content detected: '" << pair.first << "' appears " << pair.second << " times");
            writeJsonLog("validation_error", "Repetitive content detected: " + pair.first);
            return true;
        }
    }
    
    return false;
}

std::string OllamaService::truncateResponse(const std::string& response)
{
    DEBUG_LOG_FUNC();
    
    if (response.length() <= MAX_RESPONSE_SIZE) {
        return response;
    }
    
    // Try to truncate at a reasonable JSON boundary
    std::string truncated = response.substr(0, MAX_RESPONSE_SIZE);
    
    // Find the last complete JSON object by looking for the last '}'
    size_t lastBrace = truncated.rfind('}');
    if (lastBrace != std::string::npos) {
        truncated = truncated.substr(0, lastBrace + 1);
    }
    
    DEBUG_LOG("Response truncated from " << response.length() << " to " << truncated.length() << " bytes");
    writeJsonLog("truncation", "Response truncated due to size limit");
    
    return truncated;
}

std::string OllamaService::handlePartialResponse(const std::string& response)
{
    DEBUG_LOG_FUNC();
    
    // Check if response looks like a partial JSON
    if (response.empty()) {
        DEBUG_LOG("Empty response detected");
        writeJsonLog("partial_response_error", "Empty response received");
        return "{}";  // Return minimal valid JSON
    }
    
    // Check if response starts with valid JSON structure
    std::string trimmed = response;
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        trimmed = trimmed.substr(start);
    }
    
    if (trimmed.empty() || trimmed[0] != '{') {
        DEBUG_LOG("Response doesn't start with valid JSON");
        writeJsonLog("partial_response_error", "Invalid JSON format detected");
        return "{}";  // Return minimal valid JSON
    }
    
    // Try to fix incomplete JSON by adding closing braces
    int openBraces = 0;
    int openBrackets = 0;
    bool inString = false;
    char prevChar = 0;
    
    for (char c : trimmed) {
        if (!inString) {
            if (c == '{') openBraces++;
            else if (c == '}') openBraces--;
            else if (c == '[') openBrackets++;
            else if (c == ']') openBrackets--;
            else if (c == '"' && prevChar != '\\') inString = true;
        } else {
            if (c == '"' && prevChar != '\\') inString = false;
        }
        prevChar = c;
    }
    
    // Add missing closing characters
    std::string fixed = trimmed;
    for (int i = 0; i < openBrackets; i++) {
        fixed += "]";
    }
    for (int i = 0; i < openBraces; i++) {
        fixed += "}";
    }
    
    if (fixed != trimmed) {
        DEBUG_LOG("Fixed incomplete JSON by adding closing braces/brackets");
        writeJsonLog("partial_response_fix", "Fixed incomplete JSON structure");
    }
    
    return fixed;
}

bool OllamaService::validateResponseQuality(const FolderSuggestions& suggestions, size_t expectedBookmarkCount)
{
    DEBUG_LOG_FUNC();
    
    // Check if we have any suggestions at all
    if (suggestions.empty()) {
        DEBUG_LOG("Response quality check failed: No folder suggestions provided");
        writeJsonLog("quality_validation", "No folder suggestions in response");
        return false;
    }
    
    // Count total bookmarks assigned
    size_t totalAssignedBookmarks = 0;
    for (const auto& suggestion : suggestions) {
        totalAssignedBookmarks += suggestion.bookmarkFiles.size();
    }
    
    // Check if we have reasonable folder distribution
    if (suggestions.size() < 2) {
        DEBUG_LOG("Response quality check failed: Only " << suggestions.size() << " folder(s) created, expected at least 2");
        writeJsonLog("quality_validation", "Insufficient folder diversity - need at least 2 folders");
        return false;
    }
    
    if (suggestions.size() > 10) {
        DEBUG_LOG("Response quality check failed: Too many folders (" << suggestions.size() << "), maximum is 10");
        writeJsonLog("quality_validation", "Excessive folder creation - more than 10 folders");
        return false;
    }
    
    // Check if bookmarks are reasonably distributed
    double avgBookmarksPerFolder = static_cast<double>(totalAssignedBookmarks) / suggestions.size();
    size_t emptyFolders = 0;
    size_t oversizedFolders = 0;
    
    for (const auto& suggestion : suggestions) {
        if (suggestion.bookmarkFiles.empty()) {
            emptyFolders++;
        }
        
        // Check if any folder has more than 60% of all bookmarks
        double folderPercentage = static_cast<double>(suggestion.bookmarkFiles.size()) / totalAssignedBookmarks;
        if (folderPercentage > 0.6) {
            oversizedFolders++;
            DEBUG_LOG("Folder '" << narrow(suggestion.folderName) << "' contains " << 
                     (folderPercentage * 100) << "% of all bookmarks");
        }
    }
    
    if (emptyFolders > 0) {
        DEBUG_LOG("Response quality check failed: " << emptyFolders << " empty folders found");
        writeJsonLog("quality_validation", "Empty folders detected in response");
        return false;
    }
    
    if (oversizedFolders > 0) {
        DEBUG_LOG("Response quality check failed: " << oversizedFolders << " oversized folders (>60% of bookmarks)");
        writeJsonLog("quality_validation", "Poor bookmark distribution - some folders too large");
        return false;
    }
    
    // Check if we're covering a reasonable percentage of the input bookmarks
    double coveragePercentage = static_cast<double>(totalAssignedBookmarks) / expectedBookmarkCount;
    if (coveragePercentage < 0.8) {
        DEBUG_LOG("Response quality check failed: Only " << (coveragePercentage * 100) << 
                 "% of bookmarks assigned (" << totalAssignedBookmarks << "/" << expectedBookmarkCount << ")");
        writeJsonLog("quality_validation", "Poor bookmark coverage - less than 80% assigned");
        return false;
    }
    
    // Check for reasonable folder names (not too generic)
    std::set<std::string> genericNames = {"misc", "other", "others", "various", "stuff", "items", "bookmarks", "links"};
    size_t genericFolders = 0;
    
    for (const auto& suggestion : suggestions) {
        std::string lowerName = narrow(suggestion.folderName);
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (genericNames.find(lowerName) != genericNames.end()) {
            genericFolders++;
        }
    }
    
    if (genericFolders > 1 || (genericFolders > 0 && suggestions.size() <= 3)) {
        DEBUG_LOG("Response quality check warning: " << genericFolders << " generic folder names detected");
        writeJsonLog("quality_validation", "Generic folder names detected - organization may not be optimal");
        // Don't fail for this, just log the warning
    }
    
    DEBUG_LOG("Response quality check passed: " << suggestions.size() << " folders, " << 
             totalAssignedBookmarks << "/" << expectedBookmarkCount << " bookmarks assigned (" << 
             (coveragePercentage * 100) << "% coverage)");
    
    DEBUG_LOG_FUNC_EXIT();
    return true;
}

std::vector<float> OllamaService::computeEmbedding(const std::string& text)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Computing embedding for text: " << text.substr(0, 100) << (text.length() > 100 ? "..." : ""));
    
    try {
        std::string embedUrl = _baseUrl + "/api/embeddings";
        
        // Create JSON payload for embedding request
        std::ostringstream payload;
        payload << "{"
                << "\"model\": \"nomic-embed-text\","
                << "\"prompt\": \"" << escapeJsonString(text) << "\""
                << "}";
        
        DEBUG_LOG("Embedding request payload: " << payload.str());
        
        std::string response = sendHttpRequest(embedUrl, payload.str());
        
        if (response.empty()) {
            DEBUG_LOG("ERROR: Empty response from embedding service");
            DEBUG_LOG_FUNC_EXIT();
            return {};
        }
        
        // Parse embedding response - look for "embedding": [numbers...]
        std::regex embeddingPattern(R"("embedding"\s*:\s*\[([^\]]+)\])");
        std::smatch match;
        
        if (!std::regex_search(response, match, embeddingPattern)) {
            DEBUG_LOG("ERROR: No embedding data found in response");
            DEBUG_LOG("Response: " << response.substr(0, 500) << "...");
            DEBUG_LOG_FUNC_EXIT();
            return {};
        }
        
        std::string numbersStr = match[1].str();
        std::vector<float> embedding;
        
        // Parse comma-separated numbers
        std::istringstream stream(numbersStr);
        std::string token;
        
        while (std::getline(stream, token, ',')) {
            try {
                // Trim whitespace
                token.erase(0, token.find_first_not_of(" \t\n\r"));
                token.erase(token.find_last_not_of(" \t\n\r") + 1);
                
                float value = std::stof(token);
                embedding.push_back(value);
            } catch (const std::exception& e) {
                DEBUG_LOG("ERROR: Failed to parse embedding value: " << token << " - " << e.what());
            }
        }
        
        DEBUG_LOG("Successfully computed embedding with " << embedding.size() << " dimensions");
        DEBUG_LOG_FUNC_EXIT();
        return embedding;
        
    } catch (const std::exception& e) {
        DEBUG_LOG("ERROR: Exception in computeEmbedding: " << e.what());
        DEBUG_LOG_FUNC_EXIT();
        return {};
    }
}

float OllamaService::computeSimilarity(const std::vector<float>& embedding1, const std::vector<float>& embedding2)
{
    DEBUG_LOG_FUNC();
    
    if (embedding1.empty() || embedding2.empty() || embedding1.size() != embedding2.size()) {
        DEBUG_LOG("ERROR: Invalid embeddings for similarity computation - sizes: " << 
                 embedding1.size() << " vs " << embedding2.size());
        DEBUG_LOG_FUNC_EXIT();
        return 0.0f;
    }
    
    // Compute dot product
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < embedding1.size(); ++i) {
        dotProduct += embedding1[i] * embedding2[i];
        norm1 += embedding1[i] * embedding1[i];
        norm2 += embedding2[i] * embedding2[i];
    }
    
    // Compute cosine similarity
    float similarity = 0.0f;
    if (norm1 > 0.0f && norm2 > 0.0f) {
        similarity = dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
    }
    
    DEBUG_LOG("Computed cosine similarity: " << similarity);
    DEBUG_LOG_FUNC_EXIT();
    return similarity;
}

}  // namespace Bookmarks


