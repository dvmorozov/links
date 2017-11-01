#pragma once

#include "Page.h"

namespace Bookmarks
{
class FileListLegacy : public Page
{
private:
    const std::wstring _hintFolder = _T("Папка");
    const std::wstring _home = _T("В НАЧАЛО");

    void PrintFolders();
    void PrintFiles();
    void PrintFileList(std::vector<std::wstring> files);
    void PrintDirList(std::vector<std::wstring> dirs);
    std::vector<std::wstring> ReadFolders();
    std::vector<std::wstring> ReadFiles();
    void InsertAddButtons();

    Bookmarks::Data _data;

protected:
    const std::wstring HintDelete = _T("Удаление");
    const std::wstring HintEdit = _T("Редактирование");

    std::wstring GetCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file);
    std::wstring GetFullCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file);

    virtual std::wstring GetImagePath(std::wstring image_file);
    virtual void InsertRowCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, const TCHAR *image_file, const TCHAR* hint);
    virtual void InsertCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, const TCHAR *image_file, const TCHAR* hint);
    virtual void InsertLinkButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt);
    virtual void InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt);

    virtual void OpenOuterTable();
    virtual void CloseOuterTable();

    virtual void OpenInnerTable();
    virtual void CloseInnerTable();

    //  https://action.mindjet.com/task/14720269
    virtual void OpenInnerFileTableRow(std::wstring url = L"#");
    //  https://action.mindjet.com/task/14720269
    virtual void OpenInnerDirTableRow(std::wstring url = L"#");
    virtual void CloseInnerTableRow();

    //  Выводит строку со ссылкой.
    virtual void PrintFileRow(std::wstring fileName);
    void PrintFileNameWithoutExt(std::wstring fileName);

public:
    FileListLegacy();
    ~FileListLegacy();

    virtual void Render();
};
}
