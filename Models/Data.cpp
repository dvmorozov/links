
#include "Data.h"
#include "LinkFileReader.h"
#include "../Controllers/Commands.h"
#include "../main.h"

#include <sstream>
#include <locale>
#include <iomanip>
#include <algorithm>
#include <assert.h>

namespace Bookmarks
{
    FileVector Data::GetFileList()
    {
        return _fileList->GetFileList();
    }

    FileVector Data::GetDirList()
    {
        return _fileList->GetDirList();
    }
}

