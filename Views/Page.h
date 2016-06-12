#pragma once

#include "View.h"

namespace Bookmarks
{
    class Page : public View
    {
    private: 
        const std::wstring hint_folder = _T("Папка");
        const std::wstring home = _T("В НАЧАЛО");
        const std::wstring hint_delete = _T("Удаление");
        const std::wstring hint_edit = _T("Редактирование");

        void print_folders();
        std::vector<std::wstring> read_folders();
        void insert_add_buttons();

        void PrintRowTag();     //  Реализует машину состояний для вывода внутр. таблиц.

    protected:
        static void Page::print_info();

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
        static void print_html_head(TCHAR *title);
        static void print_html_tail();
    };
}
