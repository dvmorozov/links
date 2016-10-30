
#include "ConfigTest.h"

#include <assert.h>
#include <vcclr.h>

using namespace System;
using namespace Microsoft::Win32;

namespace Bookmarks
{
    //  https://action.mindjet.com/task/14777741
    String^ GetStringRegKey(String^ hKey, String^ strValueName)
    {
        RegistryKey ^localKey;
        if (Environment::Is64BitOperatingSystem)
            localKey = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry64);
        else
            localKey = RegistryKey::OpenBaseKey(RegistryHive::LocalMachine, RegistryView::Registry32);
        RegistryKey ^myKey = localKey->OpenSubKey(hKey, false);
        String^ value = (String^)myKey->GetValue(strValueName);
        return value;
    }

    //  https://action.mindjet.com/task/14777741
    std::wstring RegConfig::GetValue(std::wstring name)
    {
        String^ value = GetStringRegKey(gcnew String(L"SOFTWARE\\Links"), gcnew String(name.c_str()));
        cli::interior_ptr<const wchar_t> p = PtrToStringChars(value);
        std::wstring result;
        while (p != nullptr && *p != 0)
            result += *(p++);
        return result;
    }
}
