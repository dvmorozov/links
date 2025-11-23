
#include "App/stdafx.h"

#include "ConfigFactory.h"
#include "Utils/DebugLog.h"
#include "FileConfig.h"

namespace Bookmarks
{
Config *ConfigFactory::GetConfig() 
{ 
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating new FileConfig instance");
    Config* config = new FileConfig();
    DEBUG_LOG("FileConfig instance created successfully");
    DEBUG_LOG_FUNC_EXIT();
    return config;
}
}  // namespace Bookmarks
