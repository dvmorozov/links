#pragma once
#include "App/Globals.h"
#include "IBookmarkRepository.h"
#include "IDirectoryRepository.h"
#include <memory>

namespace Bookmarks
{

// Factory for creating repository instances
// Provides a centralized way to create and configure repositories
// Can be easily extended to support different storage backends
class RepositoryFactory
{
public:
    // Repository types
    enum class RepositoryType
    {
        Filesystem,  // Current filesystem-based implementation
        Database,    // Future database-based implementation
        Memory       // In-memory implementation for testing
    };

    // Create bookmark repository instance
    static std::shared_ptr<IBookmarkRepository> CreateBookmarkRepository(
        RepositoryType type = RepositoryType::Filesystem
    );

    // Create directory repository instance
    static std::shared_ptr<IDirectoryRepository> CreateDirectoryRepository(
        RepositoryType type = RepositoryType::Filesystem
    );

    // Get the default repository type (can be configured)
    static RepositoryType GetDefaultRepositoryType();

    // Set the default repository type
    static void SetDefaultRepositoryType(RepositoryType type);

private:
    static RepositoryType _defaultType;
};

} // namespace Bookmarks