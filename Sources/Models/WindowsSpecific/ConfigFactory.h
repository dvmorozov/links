#ifndef CONFIGFACTORY_H
#define CONFIGFACTORY_H

#include "Shared/Models/Config.h"

namespace Bookmarks
{
    class ConfigFactory
    {
    public:
        static Config* GetConfig();
    };
}

#endif // CONFIGFACTORY_H
