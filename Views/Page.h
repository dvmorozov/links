#pragma once

#include "View.h"

namespace Bookmarks
{
    class Page : public View
    {
    private: 
        const std::string hint_folder = _T("Папка");
        const std::string home = _T("В НАЧАЛО");
        const std::string hint_delete = _T("Удаление");
        const std::string hint_edit = _T("Редактирование");

        void print_folders();
        std::vector<std::string> read_folders();
        void insert_add_buttons();

        void PrintRowTag();     //  Реализует машину состояний для вывода внутр. таблиц.

    protected:
        static void Page::print_info();

        virtual std::string GetImagePath(std::string image_file);
        virtual void InsertRowCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint);
        virtual void InsertCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint);
        virtual void InsertLinkButton(std::string image_file, const std::string script_name, const std::string url, int size, const std::string alt);
        virtual void InsertButton(std::string image_file, const std::string script_name, const std::string url, int size, const std::string alt);
        virtual void PrintHead(std::string title);
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
