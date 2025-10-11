
#pragma once
#include "App/Globals.h"
#include "Config.h"

namespace Bookmarks
{
class ConfigFactory
{
public:
    static Config *GetConfig();
};
}  // namespace Bookmarks
