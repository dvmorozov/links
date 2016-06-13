#pragma once

//  коды команд
#define CMD_CHANGE_FOLDER       0
#define CMD_EDIT                1
#define CMD_ADD                 2
#define CMD_DEL                 3                       //  удаление ссылки
#define CMD_EDIT_FOLDER         4
#define CMD_ADD_FOLDER          5
#define CMD_DEL_FOLDER          6                       //  удаление папки
#define CMD_DEL_CONF            7                       //  запрос на удаление ссылки
#define CMD_DEL_FOLDER_CONF     8                       //  запрос на удаление папки
#define CMD_EDIT_FOLDER_CONF    9                       //  запрос на изменение имени папки
#define CMD_EDIT_CONF           10                      //  запрос на изменение ссылки
#define CMD_ADD_CONF            11                      //  запрос на создание ссылки (выводит форму ввода ссылки)
#define CMD_ADD_FOLDER_CONF     12                      //  запрос на создание папки (выводит форму ввода имени папки)
#define CMD_LOG_IN_CONF         13                      //  вывод формы ввода имени пользователя и пароля
#define CMD_LOG_IN              14                      //  обработка формы ввода имени пользователя и пароля
#define CMD_KEY                 15                      //  klyuch zaprosa
#define CMD_UNKNOWN             0xff

