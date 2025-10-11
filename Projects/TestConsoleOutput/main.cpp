#include <cstdio>
#include <locale.h>
#include <wchar.h>

int main()
{
    //  https://stackoverflow.com/questions/65494406/why-does-wprintf-transliterate-russian-text-in-unicode-into-latin-on-linux
    //  Cyrillic text by default is converted into translit.
    //  This converts text to UTF-8 codepage and displays cyrillic properly.
    setlocale(LC_ALL, "");
    //  This doesn't work.
    //  setlocale(LC_ALL, "ru_RU.utf8");
    //  The format %ls must be used!
    wprintf(L"Вас приветствует %ls!\n", L"Тест");
    return 0;
}