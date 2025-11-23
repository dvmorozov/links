#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>

#ifdef _DEBUG

// Debug logging macro that only compiles in debug builds
// Log directory is mapped from host for easy Windows access
#define DEBUG_LOG(message) do { \
    std::ofstream logFile("/var/log/links/debug.log", std::ios::app); \
    if (logFile.is_open()) { \
        auto now = std::time(nullptr); \
        logFile << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] "; \
        logFile << "[" << __FILE__ << ":" << __LINE__ << "] "; \
        logFile << message << std::endl; \
        logFile.close(); \
    } \
} while(0)

#define DEBUG_LOG_FUNC() DEBUG_LOG("Entering function: " << __FUNCTION__)
#define DEBUG_LOG_FUNC_EXIT() DEBUG_LOG("Exiting function: " << __FUNCTION__)

#else

// In release builds, these macros do nothing
#define DEBUG_LOG(message) ((void)0)
#define DEBUG_LOG_FUNC() ((void)0)
#define DEBUG_LOG_FUNC_EXIT() ((void)0)

#endif // _DEBUG