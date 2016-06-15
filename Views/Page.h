#pragma once

#include "View.h"

namespace Bookmarks
{
    class Page : public View
    {
    private: 
        const std::wstring _hintFolder = _T("Папка");
        const std::wstring _home = _T("В НАЧАЛО");
        const std::wstring _hintDelete = _T("Удаление");
        const std::wstring _hintEdit = _T("Редактирование");

        void PrintFolders();
        std::vector<std::wstring> ReadFolders();
        void InsertAddButtons();

        void PrintRowTag();     //  Реализует машину состояний для вывода внутр. таблиц.

        bool FirstFolder = true;
        bool FirstLink = true;

    protected:
        static void Page::PrintInfo();

        virtual std::wstring GetImagePath(std::wstring image_file);
        virtual void InsertRowCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint);
        virtual void InsertCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint);
        virtual void InsertLinkButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt);
        virtual void InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt);
        virtual void PrintHead(std::wstring title);
        virtual void PrintTail();

        virtual void OpenOuterTable();
        virtual void OpenInnerTable();
        virtual void CloseInnerTable();
        virtual void OpenInnerTableRow();
        virtual void CloseInnerTableRow();

    public:
        Page();
        ~Page();

        virtual void Render();
        static void PrintHtmlHead(TCHAR *title);
        static void PrintHtmlTail();
    };
}
