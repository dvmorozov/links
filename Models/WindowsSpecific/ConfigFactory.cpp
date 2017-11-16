#include "ConfigFactory.h"
#include "RegConfig.h"

namespace Bookmarks
{
    Config* ConfigFactory::GetConfig()
    {
        return new RegConfig();
    }
}
