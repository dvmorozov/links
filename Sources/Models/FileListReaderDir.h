
#pragma once
#include "App/Globals.h"
#include "TextFileReader.h"

namespace Bookmarks
{
//  Reads contents of current directory by means of system utility.
class FileListReaderDir : public TextFileReader
{
public:
    virtual std::vector<std::wstring> ReadFileLines();

    static TextFileReader *GetFileReader(
#ifdef _WINDOWS
        UINT codePage
#endif
    );

private:
    FileListReaderDir(
#ifdef _WINDOWS
        UINT codePage
#endif
        )
        : TextFileReader(
#ifdef _WINDOWS
              codePage
#endif
          )
    {
    }
};
}  // namespace Bookmarks
