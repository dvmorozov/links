
#include "Page.h"
#include "../Models/FileReader.h"
#include "../Controllers/Commands.h"
#include "../main.h"

namespace Bookmarks
{

    Page::Page()
    {
    }


    Page::~Page()
    {
    }

    std::wstring Page::GetImagePath(std::wstring image_file)
    {
        if (!img_path)
            return _T("");
        return std::wstring(img_path) + image_file;
    }

    void Page::InsertButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
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

    void Page::PrintHead(std::wstring title)
    {
        _tprintf(_T("%s"), _T("<html>"));
        _tprintf(_T("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"><meta http-equiv=\"Content-Language\" content=\"ru\">\n"));
        _tprintf(_T("<title>%s</title></head>\n"), title.c_str());
        _tprintf(_T("<body><br>\n"));
    }

    void Page::PrintTail()
    {
        PrintInfo();
        _tprintf(_T("%s"), _T("</body></html>\n"));
    }

    void Page::InsertRowCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint)
    {
        _tprintf(_T("<td>"));
        InsertCommandButton(cmd, dir, file, image_file, hint);
        _tprintf(_T("</td>"));
    }

    void Page::InsertLinkButton(std::wstring image_file, const std::wstring script_name, const std::wstring url, int size, const std::wstring alt)
    {
        _tprintf(_T("<td>"));
        InsertButton(image_file, script_name, url, 16, alt);
        _tprintf(_T("</td>"));
    }

    void Page::InsertCommandButton(TCHAR *cmd, TCHAR* dir, TCHAR* file, TCHAR *image_file, const TCHAR* hint)
    {
        std::wstring command;
        if (dir) command += dir;
        if (cmd_key) command += cmd_key;
        if (key) command += key;
        if (cmd) command += cmd;
        if (file) command += file;

        InsertButton(image_file, full_script_name ? full_script_name : _T(""), command, 16, hint ? hint : _T(""));
    }

    //  �������� ������� ������� � ������ ������ ������
    std::vector<std::wstring> Page::ReadFolders()
    {
        FileVector files = _data.GetFileList();
        std::vector<std::wstring> result;
        for (auto f = files.begin(); f != files.end(); ++f)
            result.push_back(f->Name);
        return result;
    }

    void Page::InsertAddButtons()
    {
        _tprintf(_T("\n<p>\n"));
        InsertCommandButton(cmd_add_conf, query, _T(""), _T("add_link.bmp"), _T("������� ������"));
        InsertCommandButton(cmd_add_folder_conf, query, _T(""), _T("add_folder.bmp"), _T("������� �����"));
        _tprintf(_T("\n</p>\n"));
    }

    //  ������� �����������
    void Page::PrintRowTag()
    {
        if (_firstFolder)
        {
            if (!_firstLink)
                //  ��������� ��������� ������� � ������ ������� �������
                CloseInnerTable();

            //  ��������� ������ ������� �������, ������� ��������� �������,
            //  ��������� ������ � ������ ��������� �������
            OpenInnerTable();
            _firstFolder = false;
            _firstLink = true;
        }
        OpenInnerTableRow();
    }

    //  ��������� ������ ������� �. � ������� ������ ��� ��������� �������.
    void Page::OpenInnerTable()
    {
        _tprintf(_T("<tr>\n\
        <td valign=\"top\">\n\
            <table border=\"0\" cellspacing=\"0\" cellpadding=\"1\">\n\
                <tbody>"));
    }

    void Page::CloseInnerTable()
    {
        _tprintf(_T("</tbody>\n\
                </table>\n\
            </td>\n\
        </tr>"));
    }

    //  https://action.mindjet.com/task/14720269
    void Page::OpenInnerTableRow(std::wstring url)
    {
        _tprintf(_T("<tr>"));
    }

    //  ����������� ������ � ������ ���������� �������
    void Page::CloseInnerTableRow()
    {
        _tprintf(_T("</tr>"));
    }

    void Page::OpenOuterTable()
    {
        //  ��������� ������� �������, � ���. ���������� �������-�������
        _tprintf(_T("\n\
<table border=\"1\" cellspacing=\"0\" cellpadding=\"0\">\n\
    <tbody>\n\
        <tr>\n\
            <th>%s\n\
            </th>\n\
            <th>%s [%s]\n\
            </th>\n\
        </tr>\n"), _T("�����"), _T("������"), query);
    }

    void Page::CloseOuterTable()
    {
        _tprintf(_T("\
    </tbody>\n\
</table>\n"));
    }

    //  ��� ������ ��������� �������� ��� �������, �������
    //  ����� ������� ������ ���������, ��������� ��� ��������
    void Page::PrintFolders()
    {
        OpenOuterTable();

        std::vector<std::wstring> dirs = ReadFolders();
        for (std::vector<std::wstring>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
        {
            std::wstring s = *dir;
            s.resize(MAX_LINE_LENGTH - 1);
            //  ���������� ������������� �� ������ �������� ������
            TCHAR lineinp[MAX_LINE_LENGTH];
            _tcscpy(lineinp, s.c_str());

            int n = _tcslen(lineinp);
            if (n)
            {
                TCHAR lineptr[MAX_LINE_LENGTH];
                //  ��������� ������ ����� url-������ ��� ����� �����
                unsigned char folder = 0;
                _tcscpy(lineptr, lineinp);

                while ((lineptr[n - 1] == '\x0a') || (lineptr[n - 1] == '\x0d')
#ifdef LINUX
                    || (lineptr[n - 1] == '*')
#endif
                    ) n--;
                lineptr[n] = 0;         //  ������� �� ������ ��� ����. ����. => 
                                        //  ������ �������� ��� �����
#ifdef USE_CYGWIN || LINUX
                if (lineptr[n - 1] != '/')
                {// imya fayla
                 //  proverka dopusimosti rasschireniya
                    if (!(
                        ((lineptr[n - 1] == 'l') || (lineptr[n - 1] == 'L')) &&
                        ((lineptr[n - 2] == 'r') || (lineptr[n - 2] == 'R')) &&
                        ((lineptr[n - 3] == 'u') || (lineptr[n - 3] == 'U')) &&
                        (lineptr[n - 4] == '.')
                        ))continue;
                    //  imya fayla s rasschireniem
                }
                else
                {// imay papki
                    lineptr[n - 1] = 0;
                    folder = 1;
                }
#else
                                        //  �������� ����������
                if ((
                    ((lineptr[n - 1] == 'l') || (lineptr[n - 1] == 'L')) &&
                    ((lineptr[n - 2] == 'r') || (lineptr[n - 2] == 'R')) &&
                    ((lineptr[n - 3] == 'u') || (lineptr[n - 3] == 'U')) &&
                    (lineptr[n - 4] == '.')
                    ))
                {// imya fayla s rasschireniem
                }
                else
                {
                    if ((lineptr[n - 1] == '$') && (lineptr[n - 2] == '$') &&
                        (lineptr[n - 3] == '$') && (lineptr[n - 4] == '.'))
                    {
                        // imya sluzhebnogo fayla
                        continue;
                    }
                    else
                    {
                        // imya papki
                        folder = 1;
                    }
                }
#endif
                // ��� �������� ����� �� ����������� ������ "�����" � "������"
                if (!_tcscmp(lineptr, _T(".")))
                {// ������� � �������� ����� ��������
                    if (_tcslen(query))
                    {
                        PrintRowTag();
                        InsertRowCommandButton(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */, _T("to_start_page.bmp"), _hintFolder.c_str());
                        _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _home.c_str()/*��������*/);
                        CloseInnerTableRow();
                    }
                }
                else
                    if (!_tcscmp(lineptr, _T("..")))
                    {// ������� �� ���� ������� ����� ��� � �������� ����� ��������
                        if (_tcslen(query))
                        {
                            TCHAR *temp = (TCHAR *)malloc((_tcslen(query) + 1) * sizeof(TCHAR));
                            PrintRowTag();

                            if (temp)
                            {
                                //  �������� ��� ������� �����.
                                TCHAR *up_dir;
                                _tcscpy(temp, query);
                                up_dir = (TCHAR*)_tcsrchr(temp, '/');
                                //  ��������� �� ������� ����� ������.
                                if (up_dir)
                                    *up_dir = 0;

                                //  ������� � ������ �������� ������ �������� (���� ���� ����. ����� ���. ��������).
                                InsertRowCommandButton(cmd_ch_folder, _T("")/* url */, ok/* to, chto posle komandy */, _T("to_upper_folder.bmp"), _hintFolder.c_str());
                                _tprintf(_T("<td width='100%%' colspan='3'>%s</td>"), _T("�����"));
                                free(temp);
                            }
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
                        if (_tcscmp(query, _T(".")) && _tcscmp(query, _T("..")))
                        {
                            //  ����������� URL ������� ������ � ��������� ������
                            if (folder)
                            {
                                //  full_dir � ���������� �������� url ������������ url ������� 
                                //  � ����� ��� ����� - ������������ ��� �������� ������
                                TCHAR *full_dir = lineptr;
                                if (_tcslen(query))
                                {
                                    full_dir = (TCHAR *)malloc((_tcslen(query) + _tcslen(lineptr) + 2) * sizeof(TCHAR));
                                    if (full_dir)
                                    {
                                        _tcscpy(full_dir, query);
                                        _tcscat(full_dir, _T("/"));
                                        _tcscat(full_dir, lineptr);
                                    }
                                    /*
                                    else
                                    {// oshibka fatal'na - nichego ne vyvoditsya,
                                    // poskol'ku zagolovok stranitsy uzhe vyveden
                                    }
                                    */
                                }

                                PrintRowTag();
                                if (full_dir)
                                    //  ������� �� �������� �� ���� ������� �����
                                    InsertRowCommandButton(cmd_ch_folder, full_dir/* url */, ok/* to, chto posle komandy */, _T("folder.bmp"), _hintFolder.c_str());

                                //  ����� �������� �����
                                _tprintf(_T("<td width=\"100%%\">%s</td>"), lineptr);
                                InsertRowCommandButton(cmd_del_folder_conf, query, lineptr, _T("delete_folder.bmp"), HintDelete.c_str());
                                InsertRowCommandButton(cmd_edit_folder_conf, query, lineptr, _T("edit_folder.bmp"), HintEdit.c_str());
                                CloseInnerTableRow();
                                if (full_dir != lineptr)free(full_dir);
                            }
                            else
                            {
                                if (_firstLink)
                                {
                                    if (!_firstFolder)
                                        //  ��������� ��������� ������� � ������ ������� �������
                                        CloseInnerTable();
                                    //  ��������� ������ ������� �������, ������� ��������� �������,
                                    //  ��������� ������ � ������ ��������� �������
                                    OpenInnerTable();
                                    _firstLink = false;
                                    _firstFolder = true;
                                }
                                //  ������� ������ �� �������.
                                PrintLinkRow(lineptr);
                            }
                        }
                    }
            }
        }

        if (!_firstFolder || !_firstLink)
            //  ����������� ��������� ������� � ������ ������� �������
            CloseInnerTable();
        //  ����������� ������ ������� ������� � ������� �������
        CloseOuterTable();
    }

    //  ������� ������ �� �������.
    void Page::PrintLinkRow(TCHAR *lineptr)
    {
        //  !!! ����� ����������� ���������� ���������������
        //  �� lineptr, ����� ����������� �������� ������� !!!
        //  �����������������
        Bookmarks::FileReader fr(cwd);
        std::wstring url = fr.GetParamCurDir(lineptr, ParamURL);
#ifdef EXTENDED_URL_FILE
        std::wstring name = fr.GetParamCurDir(lineptr, ParamName);
#endif
        OpenInnerTableRow();

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
        {// ������ �������� �� ����� ����� ��� ����������
            TCHAR *dot = (TCHAR*)_tcsrchr(lineptr, '.');
            if (dot)*dot = 0;
            _tprintf(_T("<td width=\"100%%\">%s</td>\n"), lineptr);
            if (dot)*dot = '.';  //  �������������� ����� �����
        }
        //  ������� ������
        InsertRowCommandButton(cmd_del_conf, query, lineptr, _T("delete_link.bmp"), HintDelete.c_str());
        InsertRowCommandButton(cmd_edit_conf, query, lineptr, _T("edit_link.bmp"), HintEdit.c_str());
        CloseInnerTableRow();
    }

    void Page::Render()
    {
        PrintHead(_T("��������� ������"));
        PrintFolders();
        InsertAddButtons();
        PrintTail();
    }

    void Page::PrintInfo()
    {
#if _DEBUG
        std::vector<std::wstring> params = {
            _T("SERVER_SOFTWARE"), _T("SERVER_NAME"), _T("SCRIPT_NAME"),
            _T("REQUEST_METHOD"), _T("QUERY_STRING"), _T("DOCUMENT_ROOT")
        };

        _tprintf(_T("%s"), _T("<br><hr><br>\n"));
        for (auto par = params.begin(); par != params.end(); ++par)
            _tprintf(_T("%s = %s<br>\n"), par->c_str(), _tgetenv(par->c_str()));

        _tprintf(_T("HOME = %s<br>\n"), cwd);
        _tprintf((DocumentRoot.substr(0, DocumentRoot.rfind('/')) + _T("\n")).c_str());
        _tprintf(_T("Error = %i<br>\n"), error);
#endif
    }

    void Page::PrintHtmlHead(TCHAR *title)
    {
        Page p;
        p.PrintHead(title);
    }

    void Page::PrintHtmlTail()
    {
        Page p;
        p.PrintTail();
    }
}
