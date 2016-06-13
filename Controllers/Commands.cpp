
#include "../main.h"

//  команды дл€ исполнени€ программой (вставл€ютс€ в url)
//  !!! нужны наборы одинаковых команд дл€ папок и файлов, поскольку расширение
//  не передаетс€ через url, а внутри папки может находитьс€ папка и файл с 
//  одинаковым именем !!!
//  komandy dolzhny nachinat'sya s ; - sm. realizatsiyu insert_command_image
TCHAR cmd_edit[] = _T(";edit=");
TCHAR cmd_edit_conf[] = _T(";edit_conf=");
TCHAR cmd_add[] = _T(";add=");
TCHAR cmd_log_in[] = _T(";log_in=");
TCHAR cmd_add_conf[] = _T(";add_conf=");
TCHAR cmd_del[] = _T(";del=");
TCHAR cmd_del_conf[] = _T(";del_conf=");
TCHAR cmd_ch_folder[] = _T(";ch_folder=");
TCHAR cmd_edit_folder[] = _T(";edit_folder=");
TCHAR cmd_edit_folder_conf[] = _T(";edit_folder_conf=");
TCHAR cmd_add_folder[] = _T(";add_folder=");
TCHAR cmd_add_folder_conf[] = _T(";add_folder_conf=");
TCHAR cmd_del_folder[] = _T(";del_folder=");
TCHAR cmd_del_folder_conf[] = _T(";del_folder_conf=");
TCHAR cmd_key[] = _T(";key=");

TCHAR ok[] = _T("Ok");
TCHAR cancel[] = _T("Cancel");
TCHAR pict_error[] = _T("error.bmp");

