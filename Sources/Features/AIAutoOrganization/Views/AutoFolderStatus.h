#pragma once
#include "App/Globals.h"
#include "Shared/UI/Page.h"

namespace Bookmarks
{
class AutoFolderStatus : public Page
{
private:
    std::wstring _statusMessage;
    std::wstring _operationSummary;
    std::wstring _currentDirectory;
    bool _isSuccess;

public:
    AutoFolderStatus(const std::wstring& message, bool success, const std::wstring& operationSummary = L"", const std::wstring& currentDirectory = L"");
    virtual void Render();
};
}  // namespace Bookmarks