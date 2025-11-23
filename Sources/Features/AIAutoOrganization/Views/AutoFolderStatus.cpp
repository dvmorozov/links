#include "App/stdafx.h"
#include "AutoFolderStatus.h"
#include "Shared/Controllers/Commands.h"
#include "Utils/DebugLog.h"
#include "Utils/Miscellaneous.h"

namespace Bookmarks
{

AutoFolderStatus::AutoFolderStatus(const std::wstring& message, bool success, const std::wstring& operationSummary, const std::wstring& currentDirectory)
    : _statusMessage(message), _isSuccess(success), _operationSummary(operationSummary), _currentDirectory(currentDirectory)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("AutoFolderStatus created with success: " << (success ? "YES" : "NO"));
    DEBUG_LOG("Status message: " << narrow(message));
    DEBUG_LOG("Operation summary provided: " << (operationSummary.empty() ? "NO" : "YES"));
    DEBUG_LOG("Current directory: " << narrow(currentDirectory));
}

void AutoFolderStatus::Render()
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Rendering auto-folder status page");
    
    PrintHead(_T("Auto-Organization Status"));

    const std::wstring statusClass = _isSuccess ? L"success" : L"error";
    const std::wstring statusIcon = _isSuccess ? L"✓" : L"✗";
    
    DEBUG_LOG("Status class: " << narrow(statusClass));
    DEBUG_LOG("Status icon: " << narrow(statusIcon));
    
    // For both success and error cases, redirect to original directory
    std::wstring redirectUrl;
    if (!_currentDirectory.empty()) {
        // Use the directory where auto-organization was performed
        redirectUrl = GetFullCommandUrl(cmd_ch_folder, _currentDirectory.c_str(), ok);
    } else {
        // Fallback to current folder
        redirectUrl = std::wstring(full_script_name) + L"?;ch_folder=Ok&;key=" + (key ? key : L"");
    }
    
    wprintf(L"\
    <div class=\"status_container\">\n\
        <div class=\"status_box %ls\">\n\
            <h2>%ls Auto-Organization %ls</h2>\n\
            <p>%ls</p>\n",
            statusClass.c_str(),
            statusIcon.c_str(),
            (_isSuccess ? L"Complete" : L"Failed"),
            _statusMessage.c_str());
    
    // Display operation summary if available and successful
    if (_isSuccess && !_operationSummary.empty()) {
        // Convert the operation summary to HTML format
        std::wstring htmlSummary = _operationSummary;
        
        // Replace newlines with HTML breaks
        size_t pos = 0;
        while ((pos = htmlSummary.find(L"\n", pos)) != std::wstring::npos) {
            htmlSummary.replace(pos, 1, L"<br>");
            pos += 4;
        }
        
        wprintf(L"\
            <div class=\"operation_summary\">\n\
                <details open>\n\
                    <summary><strong>📋 Operation Details</strong></summary>\n\
                    <div class=\"summary_content\">%ls</div>\n\
                </details>\n\
            </div>\n", htmlSummary.c_str());
    }
    
    wprintf(L"\
            <div class=\"buttons\">\n\
                <input type=\"button\" value=\"%ls\" onclick=\"window.location.href='%ls';\" class=\"btn\">\n\
            </div>\n\
        </div>\n\
    </div>\n\
    <style>\n\
    .status_container {\n\
        display: flex;\n\
        justify-content: center;\n\
        align-items: center;\n\
        min-height: 300px;\n\
        padding: 20px;\n\
    }\n\
    .status_box {\n\
        background: white;\n\
        border-radius: 8px;\n\
        padding: 30px;\n\
        box-shadow: 0 2px 10px rgba(0,0,0,0.1);\n\
        text-align: center;\n\
        max-width: 500px;\n\
        width: 100%%;\n\
    }\n\
    .status_box.success {\n\
        border-left: 5px solid #28a745;\n\
    }\n\
    .status_box.error {\n\
        border-left: 5px solid #dc3545;\n\
    }\n\
    .status_box h2 {\n\
        margin: 0 0 15px 0;\n\
        color: #333;\n\
    }\n\
    .status_box p {\n\
        margin: 0 0 20px 0;\n\
        color: #666;\n\
        line-height: 1.5;\n\
    }\n\
    .operation_summary {\n\
        margin: 20px 0;\n\
        text-align: left;\n\
        background: #f8f9fa;\n\
        border-radius: 6px;\n\
        padding: 15px;\n\
    }\n\
    .operation_summary details {\n\
        cursor: pointer;\n\
    }\n\
    .operation_summary summary {\n\
        margin-bottom: 10px;\n\
        color: #495057;\n\
    }\n\
    .summary_content {\n\
        font-family: monospace;\n\
        font-size: 14px;\n\
        line-height: 1.6;\n\
        color: #212529;\n\
        background: white;\n\
        border-radius: 4px;\n\
        padding: 10px;\n\
        border-left: 3px solid #28a745;\n\
    }\n\
    .btn {\n\
        background: #007bff;\n\
        color: white;\n\
        border: none;\n\
        padding: 10px 20px;\n\
        border-radius: 4px;\n\
        cursor: pointer;\n\
        font-size: 16px;\n\
    }\n\
    .btn:hover {\n\
        background: #0056b3;\n\
    }\n\
    </style>",
            L"Continue",
            redirectUrl.c_str());

    PrintTail();
}

}  // namespace Bookmarks