#ifndef CONFIGFACTORY_H
#define CONFIGFACTORY_H

#include "Config.h"

namespace Bookmarks
{
    class ConfigFactory
    {
    public:
        static Config* GetConfig();
    };
}

#endif // CONFIGFACTORY_H
