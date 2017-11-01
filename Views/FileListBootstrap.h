#pragma once
#include "FileListLegacy.h"

namespace Bookmarks
{
    class FileListBootstrap : public FileListLegacy
    {
    protected:
        virtual void InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt);
        //  https://action.mindjet.com/task/14817423
        virtual void PrintScripts();
        //  https://action.mindjet.com/task/14817423
        virtual void PrintStyles();

        virtual void OpenOuterTable();
        virtual void OpenInnerTable();
        //  https://action.mindjet.com/task/14720269
        virtual void OpenInnerFileTableRow(std::wstring url = L"#");
        //  https://action.mindjet.com/task/14720269
        virtual void OpenInnerDirTableRow(std::wstring url = L"#");
        //  Выводит строку со ссылкой.
        virtual void PrintFileRow(std::wstring fileName);

    public:
        FileListBootstrap();
        ~FileListBootstrap();
    };
}
