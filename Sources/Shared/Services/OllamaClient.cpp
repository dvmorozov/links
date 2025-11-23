#include "App/stdafx.h"
#include "OllamaClient.h"
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
#include <typeinfo>

namespace Shared
{

OllamaClient::OllamaClient(const std::string& baseUrl, const std::string& model, int timeout)
    : _baseUrl(baseUrl), _model(model), _timeoutSeconds(timeout)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("OllamaClient created with baseUrl: " << baseUrl << ", model: " << model << ", timeout: " << timeout);
}

bool OllamaClient::isAvailable()
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

std::string OllamaClient::sendPrompt(const std::string& prompt, bool stream)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Sending prompt to Ollama service, length: " << prompt.length());
    
    // Build JSON payload
    std::ostringstream json;
    json << "{\n";
    json << "  \"model\": \"" << _model << "\",\n";
    json << "  \"prompt\": \"" << escapeJsonString(prompt) << "\",\n";
    json << "  \"stream\": " << (stream ? "true" : "false") << "\n";
    json << "}";
    
    std::string jsonPayload = json.str();
    
    // Log the request
    writeJsonLog("request", jsonPayload);
    
    // Send HTTP request
    std::string url = _baseUrl + "/api/generate";
    std::string response = sendHttpRequest(url, jsonPayload);
    
    if (!response.empty()) {
        // Log the response
        writeJsonLog("response", response);
    }
    
    return response;
}

std::string OllamaClient::sendEmbeddingRequest(const std::string& model, const std::string& text)
{
    std::string url = _baseUrl + "/api/embeddings";
    
    // Create JSON payload for embedding request
    std::ostringstream payload;
    payload << "{"
            << "\"model\": \"" << model << "\","
            << "\"prompt\": \"" << escapeJsonString(text) << "\""
            << "}";
    
    // Send request (logging handled by sendHttpRequest)
    std::string response = sendHttpRequest(url, payload.str());
    
    return response;
}

std::string OllamaClient::escapeJsonString(const std::string& input)
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

void OllamaClient::writeJsonLog(const std::string& type, const std::string& content)
{
    // Use DEBUG_LOG instead of separate files for unified logging
    DEBUG_LOG("=== OLLAMA " << type << " START ===");
    DEBUG_LOG(content);
    DEBUG_LOG("=== OLLAMA " << type << " END ===");
}

#ifdef _WINDOWS
std::string OllamaClient::sendHttpRequest(const std::string& url, const std::string& jsonPayload)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Sending Windows HTTP request to: " << url);
    DEBUG_LOG("Payload length: " << jsonPayload.length());
    
    std::string result;
    
    HINTERNET hInternet = InternetOpenA("OllamaClient/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";
    
    // Parse URL
    std::string host, path;
    size_t protocolEnd = url.find("://");
    if (protocolEnd == std::string::npos) {
        InternetCloseHandle(hInternet);
        return "";
    }
    
    size_t hostStart = protocolEnd + 3;
    size_t pathStart = url.find("/", hostStart);
    
    if (pathStart == std::string::npos) {
        host = url.substr(hostStart);
        path = "/";
    } else {
        host = url.substr(hostStart, pathStart - hostStart);
        path = url.substr(pathStart);
    }
    
    INTERNET_PORT port = INTERNET_DEFAULT_HTTP_PORT;
    size_t portPos = host.find(":");
    if (portPos != std::string::npos) {
        try {
            port = static_cast<INTERNET_PORT>(std::stoi(host.substr(portPos + 1)));
            host = host.substr(0, portPos);
        } catch (...) {
            InternetCloseHandle(hInternet);
            return "";
        }
    }
    
    HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }
    
    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    HINTERNET hRequest;
    
    if (jsonPayload.empty()) {
        hRequest = HttpOpenRequestA(hConnect, "GET", path.c_str(), NULL, NULL, NULL, flags, 0);
    } else {
        hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), NULL, NULL, NULL, flags, 0);
        
        if (hRequest) {
            std::string headers = "Content-Type: application/json\r\n";
            HttpAddRequestHeadersA(hRequest, headers.c_str(), headers.length(), HTTP_ADDREQ_FLAG_ADD);
        }
    }
    
    if (hRequest) {
        BOOL requestResult;
        if (jsonPayload.empty()) {
            requestResult = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);
        } else {
            requestResult = HttpSendRequestA(hRequest, NULL, 0, (LPVOID)jsonPayload.c_str(), jsonPayload.length());
        }
        
        if (requestResult) {
            char buffer[4096];
            DWORD bytesRead;
            while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                result += buffer;
            }
        }
    }
    
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    
    // Add structured logging for LLM responses
    if ((url.find("/api/generate") != std::string::npos || url.find("/api/embeddings") != std::string::npos) && !result.empty()) {
        std::string logType = url.find("/api/embeddings") != std::string::npos ? "embedding_direct_response" : "response";
        writeJsonLog(logType, result);
    }
    
    return result;
}
#else
std::string OllamaClient::sendHttpRequest(const std::string& url, const std::string& jsonPayload)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Sending HTTP request to: " << url);
    DEBUG_LOG("Payload length: " << jsonPayload.length());
    
    // Add structured logging for LLM requests
    if (url.find("/api/generate") != std::string::npos || url.find("/api/embeddings") != std::string::npos) {
        std::string logType = url.find("/api/embeddings") != std::string::npos ? "embedding_direct_request" : "request";
        writeJsonLog(logType, jsonPayload);
    }
    
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
    
    // Extract body from HTTP response
    size_t bodyStart = response.find("\r\n\r\n");
    if (bodyStart != std::string::npos) {
        result = response.substr(bodyStart + 4);
        DEBUG_LOG("Extracted HTTP body length: " << result.length());
        
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
            }
        }
    } else {
        DEBUG_LOG("No HTTP body separator found!");
    }
    
    // Add structured logging for LLM responses
    if ((url.find("/api/generate") != std::string::npos || url.find("/api/embeddings") != std::string::npos) && !result.empty()) {
        std::string logType = url.find("/api/embeddings") != std::string::npos ? "embedding_direct_response" : "response";
        writeJsonLog(logType, result);
    }
    
    return result;
}
#endif

}  // namespace Shared