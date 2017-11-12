#include "ConfigFactory.h"
#include "FileConfig.h"

namespace Bookmarks
{
    Config* ConfigFactory::GetConfig()
    {
        return new FileConfig();
    }
}
