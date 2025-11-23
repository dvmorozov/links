#pragma once
#include <string>

namespace Shared
{
//  Generic client for communicating with Ollama API
//  Handles HTTP communication, JSON construction, and basic API interactions
class OllamaClient
{
private:
    std::string _baseUrl;
    std::string _model;
    int _timeoutSeconds;
    
public:
    OllamaClient(const std::string& baseUrl = "http://localhost:11435", 
                 const std::string& model = "llama2",
                 int timeout = 30);
    
    // Check if Ollama service is available
    bool isAvailable();
    
    // HTTP client functions
    std::string sendHttpRequest(const std::string& url, const std::string& jsonPayload);
    
    // Helper method to escape strings for JSON
    std::string escapeJsonString(const std::string& input);
    
    // JSON logging for debugging LLM interactions
    void writeJsonLog(const std::string& type, const std::string& content);
    
    // Send a prompt to Ollama and get response
    std::string sendPrompt(const std::string& prompt, bool stream = false);
    
    // Send embedding request to Ollama
    std::string sendEmbeddingRequest(const std::string& model, const std::string& text);
    
    // Get the configured model name
    const std::string& getModel() const { return _model; }
    
    // Get the configured base URL
    const std::string& getBaseUrl() const { return _baseUrl; }
};
}  // namespace Shared