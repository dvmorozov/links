
#pragma once
#include "App/Globals.h"
#include "Features/BookmarkManagement/Models/Data.h"

namespace Bookmarks
{
class View
{
public:
    View() {}
    virtual ~View() {}

    // Renders data to the standard output stream.
    virtual void Render() = 0;
};
}  // namespace Bookmarks
