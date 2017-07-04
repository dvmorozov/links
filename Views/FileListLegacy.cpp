
#include "FileListLegacy.h"
#include "../Models/FileReader.h"
#include "../Controllers/Commands.h"
#include "../main.h"

namespace Bookmarks
{
    FileListLegacy::FileListLegacy()
    {
    }

    FileListLegacy::~FileListLegacy()
    {
    }

    std::wstring FileListLegacy::GetImagePath(std::wstring image_file)
    {
        if (!img_path)
            return _T("");
        return std::wstring(img_path) + image_file;
    }

    void FileListLegacy::InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        std::wstring image_path = GetImagePath(image_file);

        if (!image_path.empty())
        {
            if (!script_name.empty())
            {
                if (!url.empty())
                {
                    _tprintf(_T("\
                            <a href=\"%s?%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), url.c_str(), image_path.c_str(), alt.c_str(), size, size);
                }
                else
                {
                    //  ���������� ������ �� ������� ������; script_name �������� ������ � �������
                    _tprintf(_T("\
                            <a href=\"%s\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), script_name.c_str(), image_path.c_str(), alt.c_str(), size, size);
                }
            }
            else
            {// ����������� ������ ��� ������
                if (!url.empty()) //  � url ���������� ���������� � ������, ����������� ����� alt
                {
                    _tprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), url.c_str(), size, size);
                }
                else
                {
                    _tprintf(_T("\
                            <a href=\"#\"><img src=\"%s\" align=\"absmiddle\" border=\"0\" alt=\"%s\" width=\"%i\" height=\"%i\"></a>"), image_path.c_str(), alt.c_str(), size, size);
                }
            }
        }
    }

    void FileListLegacy::InsertRowCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, TCHAR *image_file, const TCHAR* hint)
    {
        _tprintf(_T("<td>"));
        InsertCommandButton(cmd, dir, file, image_file, hint);
        _tprintf(_T("</td>"));
    }

    void FileListLegacy::InsertLinkButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        _tprintf(_T("<td>"));
        InsertButton(image_file, script_name, url, 16, alt);
        _tprintf(_T("</td>"));
    }

    std::wstring FileListLegacy::GetCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file)
    {
        std::wstring command;
        if (dir) command += dir;
        if (cmd_key) command += cmd_key;
        if (key) command += key;
        if (cmd) command += cmd;
        if (file) command += file;
        return command;
    }

    std::wstring FileListLegacy::GetFullCommandUrl(TCHAR *cmd, const TCHAR* dir, const TCHAR* file)
    {
        return std::wstring(full_script_name ? full_script_name : _T("")) + _T("?") + GetCommandUrl(cmd, dir, file);
    }

    void FileListLegacy::InsertCommandButton(TCHAR *cmd, const TCHAR* dir, const TCHAR* file, TCHAR *image_file, const TCHAR* hint)
    {
        InsertButton(image_file, full_script_name ? full_script_name : _T(""), GetCommandUrl(cmd, dir, file), 16, hint ? hint : _T(""));
    }

    //  �������� ������� ������� � ������ ������ ������
    std::vector<std::wstring> FileListLegacy::ReadFolders()
    {
        FileVector files = _data.GetDirList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->FileName);
        return result;
    }

    std::vector<std::wstring> FileListLegacy::ReadFiles()
    {
        FileVector files = _data.GetFileList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->FileName);
        return result;
    }

    void FileListLegacy::InsertAddButtons()
    {
        _tprintf(_T("\n<p>\n"));
        InsertCommandButton(cmd_add_conf, query, _T(""), _T("add_link.bmp"), _T("������� ������"));
        InsertCommandButton(cmd_add_folder_conf, query, _T(""), _T("add_folder.bmp"), _T("������� �����"));
        _tprintf(_T("\n</p>\n"));
    }

    void FileListLegacy::OpenInnerTable()
    {
        //  Opens new cell for internal table.
        _tprintf(_T("<td valign=\"top\">\n\
            <table border=\"0\" cellspacing=\"0\" cellpadding=\"1\">\n\
                <tbody>"));
    }

    void FileListLegacy::CloseInnerTable()
    {
        _tprintf(_T("</tbody>\n\
                </table>\n\
            </td>"));
    }

    //  https://action.mindjet.com/task/14720269
    void FileListLegacy::OpenInnerFileTableRow(std::wstring url)
    {
        _tprintf(_T("<tr>"));
    }

    //  https://action.mindjet.com/task/14720269
    void FileListLegacy::OpenInnerDirTableRow(std::wstring url)
    {
        _tprintf(_T("<tr>"));
    }

    //  ����������� ������ � ������ ���������� �������
    void FileListLegacy::CloseInnerTableRow()
    {
        _tprintf(_T("</tr>"));
    }

    void FileListLegacy::OpenOuterTable()
    {
        //  Creates outer table with 2 columns for folders and files.
        //  Adds new row for internal tables.
        _tprintf(_T("\n\
<table border=\"1\" cellspacing=\"0\" cellpadding=\"0\">\n\
    <tbody>\n\
        <tr>\n\
            <th>%s\n\
            </th>\n\
            <th>%s [%s]\n\
            </th>\n\
        </tr><tr>\n"), _T("�����"), _T("������"), query);
    }

    void FileListLegacy::CloseOuterTable()
    {
        //  For the first closes internal table row.
        _tprintf(_T("</tr>\n\
    </tbody>\n\
</table>\n"));
    }

    void FileListLegacy::PrintDirList(std::vector<std::wstring> dirs)
    {
        for (std::vector<std::wstring>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
        {
            if (dir->size())
            {
                // ��� �������� ����� �� ����������� ������ "�����" � "������"
                if (*dir == _T("."))
                {// ������� � �������� ����� ��������
                    if (wcslen(query))
                    {
                        OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */));
                        InsertRowCommandButton(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */, _T("to_start_page.bmp"), _hintFolder.c_str());
                        _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _home.c_str()/*��������*/);
                        CloseInnerTableRow();
                    }
                }
                else
                if (*dir == _T(".."))
                {// ������� �� ���� ������� ����� ��� � �������� ����� ��������
                    if (wcslen(query))
                    {
                        std::wstring upDir = query;
                        //  �������� ��� ������� �����.
                        auto slashPos = upDir.rfind(_T("/"));
                        upDir = slashPos != std::string::npos ? upDir.substr(0, slashPos) : upDir;

                        OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */));
                        //  ������� � ������ �������� ������ �������� (���� ���� ����. ����� ���. ��������).
                        InsertRowCommandButton(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */, _T("to_upper_folder.bmp"), _hintFolder.c_str());
                        _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _T("�����"));

                        CloseInnerTableRow();
                        /*  ������� � �������� ����� ��� ����������� ���� */
                    }
                }
                else
                    //  ���� ������� ������ �� ����� ".", ".." ��� 0, ��
                    //  ������ ������� �������� ��� ������� �����, ������� �����
                    //  ��������� � ������ ��������� ������ ��� ����, �����
                    //  ��� ����������� ������� ��������� ������� � ������ �����
                {
                    //  full_dir � ���������� �������� url ������������ url ������� 
                    //  � ����� ��� ����� - ������������ ��� �������� ������
                    std::wstring upDir = query;
                    upDir += _T("/");
                    upDir += *dir;
                    OpenInnerDirTableRow(GetFullCommandUrl(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */));

                    //  ������� �� �������� �� ���� ������� �����
                    InsertRowCommandButton(cmd_ch_folder, upDir.c_str(), ok/* to, chto posle komandy */, _T("folder.bmp"), _hintFolder.c_str());

                    //  ����� �������� �����
                    _tprintf(_T("<td width=\"100%%\">%s</td>"), dir->c_str());
                    InsertRowCommandButton(cmd_del_folder_conf, upDir.c_str(), dir->c_str(), _T("delete_folder.bmp"), HintDelete.c_str());
                    InsertRowCommandButton(cmd_edit_folder_conf, upDir.c_str(), dir->c_str(), _T("edit_folder.bmp"), HintEdit.c_str());
                    CloseInnerTableRow();
                }
            }
        }
    }

    void FileListLegacy::PrintFileList(std::vector<std::wstring> files)
    {
        for (std::vector<std::wstring>::iterator fileName = files.begin(); fileName != files.end(); ++fileName)
        {
            //  ������� ������ �� �������.
            PrintFileRow(*fileName);
        }
    }

    void FileListLegacy::PrintFolders()
    {
        OpenInnerTable();
        PrintDirList(ReadFolders());
        CloseInnerTable();
    }

    void FileListLegacy::PrintFiles()
    {
        OpenInnerTable();
        PrintFileList(ReadFiles());
        CloseInnerTable();
    }

    void FileListLegacy::PrintFileNameWithoutExt(std::wstring fileName)
    {
        // ������ �������� �� ����� ����� ��� ����������
        auto pointPos = fileName.rfind(_T("."));
        std::wstring fileNameWithoutExt = pointPos != std::string::npos ? fileName.substr(0, pointPos) : fileName;
        _tprintf(_T("<td width=\"100%%\">%s</td>\n"), fileNameWithoutExt.c_str());
    }

    //  ������� ������ �� �������.
    void FileListLegacy::PrintFileRow(std::wstring fileName)
    {
        //  !!! ����� ����������� ���������� ���������������
        //  �� lineptr, ����� ����������� �������� ������� !!!
        //  �����������������
        Bookmarks::FileReader fr(cwd);
        std::wstring url = fr.GetParamCurDir(fileName, ParamURL);
#ifdef EXTENDED_URL_FILE
        std::wstring name = fr.GetParamCurDir(fileName, ParamName);
#endif
        OpenInnerFileTableRow();

        if (!url.empty())
        {   //  vstavlyaetsya vneschnyaya ssylka
            InsertLinkButton(_T("link.bmp"), url, _T(""), 16, _T("������"));
        }
        else
            //  ??? ����� ����� ������� ������
            InsertLinkButton(_T("error.bmp"), _T(""), _T("���������� ��������� URL �� �����!"), 16, _T("������: "));
#ifdef EXTENDED_URL_FILE
        //  ����� ����� ������
        if (!name.empty())
        {
            _tprintf(_T("<td width=\"100%%\">%s</td>\n"), name.c_str());
        }
        else
#endif
            PrintFileNameWithoutExt(fileName);
        //  ������� ������
        InsertRowCommandButton(cmd_del_conf, query, fileName.c_str(), _T("delete_link.bmp"), HintDelete.c_str());
        InsertRowCommandButton(cmd_edit_conf, query, fileName.c_str(), _T("edit_link.bmp"), HintEdit.c_str());
        CloseInnerTableRow();
    }

    //  ��� ������ ��������� �������� ��� �������, �������
    //  ����� ������� ������ ���������, ��������� ��� ��������
    void FileListLegacy::Render()
    {
        PrintHead(_T("��������� ������"));
        OpenOuterTable();
        PrintFolders();
        PrintFiles();
        //  ����������� ������ ������� ������� � ������� �������
        CloseOuterTable();
        InsertAddButtons();
        PrintTail();
    }

    void FileListLegacy::PrintHtmlHead(TCHAR *title)
    {
        FileListLegacy p;
        p.PrintHead(title);
    }

    void FileListLegacy::PrintHtmlTail()
    {
        FileListLegacy p;
        p.PrintTail();
    }
}