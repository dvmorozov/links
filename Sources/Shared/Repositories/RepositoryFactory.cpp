#include "App/stdafx.h"
#include "RepositoryFactory.h"
#include "FilesystemBookmarkRepository.h"
#include "FilesystemDirectoryRepository.h"
#include "Utils/DebugLog.h"

namespace Bookmarks
{

// Default to filesystem implementation
RepositoryFactory::RepositoryType RepositoryFactory::_defaultType = RepositoryType::Filesystem;

std::shared_ptr<IBookmarkRepository> RepositoryFactory::CreateBookmarkRepository(RepositoryType type)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating bookmark repository of type: " << static_cast<int>(type));
    
    switch (type) {
        case RepositoryType::Filesystem:
            DEBUG_LOG("Created FilesystemBookmarkRepository");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemBookmarkRepository>();
            
        case RepositoryType::Database:
            DEBUG_LOG("Database repository not yet implemented, falling back to Filesystem");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemBookmarkRepository>();
            
        case RepositoryType::Memory:
            DEBUG_LOG("Memory repository not yet implemented, falling back to Filesystem");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemBookmarkRepository>();
            
        default:
            DEBUG_LOG("Unknown repository type, falling back to Filesystem");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemBookmarkRepository>();
    }
}

std::shared_ptr<IDirectoryRepository> RepositoryFactory::CreateDirectoryRepository(RepositoryType type)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating directory repository of type: " << static_cast<int>(type));
    
    switch (type) {
        case RepositoryType::Filesystem:
            DEBUG_LOG("Created FilesystemDirectoryRepository");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemDirectoryRepository>();
            
        case RepositoryType::Database:
            DEBUG_LOG("Database repository not yet implemented, falling back to Filesystem");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemDirectoryRepository>();
            
        case RepositoryType::Memory:
            DEBUG_LOG("Memory repository not yet implemented, falling back to Filesystem");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemDirectoryRepository>();
            
        default:
            DEBUG_LOG("Unknown repository type, falling back to Filesystem");
            DEBUG_LOG_FUNC_EXIT();
            return std::make_shared<FilesystemDirectoryRepository>();
    }
}

RepositoryFactory::RepositoryType RepositoryFactory::GetDefaultRepositoryType()
{
    return _defaultType;
}

void RepositoryFactory::SetDefaultRepositoryType(RepositoryType type)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Setting default repository type to: " << static_cast<int>(type));
    _defaultType = type;
    DEBUG_LOG_FUNC_EXIT();
}

} // namespace Bookmarks