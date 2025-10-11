#pragma once

#include "App/Globals.h"
#include "FileListLegacy.h"

namespace Bookmarks
{
class FileListBootstrap : public FileListLegacy
{
protected:
    virtual void InsertButton(std::wstring imageName, const std::wstring endpointUrl,
                              const std::wstring url, int size, const std::wstring alt);
    virtual void PrintScripts();
    virtual void PrintStyles();

    virtual void OpenOuterTable();
    virtual void OpenInnerTable();
    virtual void OpenInnerFileTableRow(std::wstring url = L"#");
    virtual void OpenInnerDirTableRow(std::wstring url = L"#");
    // Renders a row with a link.
    virtual void PrintFileRow(const File &file);

public:
    FileListBootstrap();
};
}  // namespace Bookmarks
