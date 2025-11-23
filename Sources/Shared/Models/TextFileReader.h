
#pragma once
#include "App/Globals.h"

namespace Bookmarks
{
class TextFileReader
{
public:
    virtual std::vector<std::wstring> ReadFileLines() = 0;
    void                              SetFileName(std::wstring fileName);

    virtual ~TextFileReader() {}

protected:
    std::wstring FileName;
#ifdef _WINDOWS
    UINT CodePage;
#endif
    TextFileReader(
#ifdef _WINDOWS
        UINT codePage
#endif
        )
#ifdef _WINDOWS
        : CodePage(codePage)
#endif
    {
    }

    std::vector<std::wstring> ReadFile();
};
}  // namespace Bookmarks
