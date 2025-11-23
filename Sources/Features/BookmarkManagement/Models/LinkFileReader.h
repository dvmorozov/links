
#pragma once
#include "App/Globals.h"

namespace Bookmarks
{
class LinkFileReader
{
private:
    std::wstring _curDir;

public:
    LinkFileReader() {}
    LinkFileReader(std::wstring curDir) : _curDir(curDir) {}
    ~LinkFileReader() {}

    //  Read parameter from file.
    static std::wstring GetParam(std::wstring fileName /* absolute path to the file */,
                                 std::wstring paramName);
    //  Read parameter from file from current directory.
    std::wstring GetParamFromCurDirFile(std::wstring fileName /* path to the file relative to cwd*/,
                                        std::wstring paramName);
};
}  // namespace Bookmarks
