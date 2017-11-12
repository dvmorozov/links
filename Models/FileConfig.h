#ifndef FILECONFIG_H
#define FILECONFIG_H

#include "Config.h"

namespace Bookmarks
{
    class FileConfig : public Config
    {
    public:
        virtual std::wstring GetValue(std::wstring name);
    };
}

#endif // FILECONFIG_H
