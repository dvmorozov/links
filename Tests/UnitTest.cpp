#include "stdafx.h"

#include "..\trunk\Models\Data.h"
#include "..\trunk\Models\FileReader.h"
#include "..\trunk\Models\FileListReader.h"
#include "..\trunk\Models\FileListReaderTest.h"
#include "..\trunk\main.h"
#include "..\trunk\Views\PageBootstrap.h"
#include "..\trunk\Controllers\Commands.h"

TCHAR test_out[100 * 1024];
TCHAR *test_str = test_out;

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace Bookmarks;

namespace bookmarks_test
{
    [TestClass]
    public ref class UnitTest
    {
    private:
        TestContext^ testContextInstance;

    public: 
        /// <summary>
        ///Получает или устанавливает контекст теста, в котором предоставляются
        ///сведения о текущем тестовом запуске и обеспечивается его функциональность.
        ///</summary>
        property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext
        {
            Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
            {
                return testContextInstance;
            }
            System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
            {
                testContextInstance = value;
            }
        };

        #pragma region Additional test attributes
        //
        //При написании тестов можно использовать следующие дополнительные атрибуты:
        //
        //ClassInitialize используется для выполнения кода до запуска первого теста в классе
        //[ClassInitialize()]
        //static void MyClassInitialize(TestContext^ testContext) {};
        //
        //ClassCleanup используется для выполнения кода после завершения работы всех тестов в классе
        //[ClassCleanup()]
        //static void MyClassCleanup() {};
        //
        //TestInitialize используется для выполнения кода перед запуском каждого теста
        //[TestInitialize()]
        //void MyTestInitialize() {};
        //
        //TestCleanup используется для выполнения кода после завершения каждого теста
        //[TestCleanup()]
        //void MyTestCleanup() {};
        //
        #pragma endregion 

        [TestMethod]
        void TestGetFileList()
        {
            FileListReader *flrt = GetFileReader();
            //  Returns all the string from file.
            std::vector<std::wstring> list = flrt->ReadFileList();

            Assert::AreEqual(UInt32(36), list.size());
            delete flrt;
        };

        [TestMethod]
        void TestReadFileList()
        {
            Bookmarks::Data d;

            FileVector files = d.GetFileList();
            FileVector dirs = d.GetDirList();

            Assert::AreEqual(UInt32(22), files.size());
            Assert::AreEqual(UInt32(6), dirs.size());
        };

        [TestMethod]
        void TestFileList()
        {
            FileListReader *flrt = GetFileReader();
            Bookmarks::FileList fl(flrt);

            FileVector files = fl.GetFileList();
            FileVector dirs = fl.GetDirList();

            Assert::AreEqual(UInt32(22), files.size());
            Assert::AreEqual(UInt32(6), dirs.size());
        };

        [TestMethod]
        void TestCheckNamesWithSpaces()
        {
            Bookmarks::Data d;

            FileVector _data = d.GetDirList();
            FileVector matches;

            for (auto f = _data.begin(); f != _data.end(); ++f)
            {
                if (f->Name == _T("Эхо Москвы")) matches.push_back(*f);
            }

            Assert::AreEqual(UInt32(1), matches.size());
        };

        [TestMethod]
        void TestRenderBootstrap()
        {
            static TCHAR queryData[] = _T("http://fiteasily.com/cgi-bin/links.cgi?;log_in=lvbnhbq vjhjpjd&;log_in=dct pfrkflrb&;log_in=Ok");
            query = &queryData[0];
            cwd = _T("test");       //  Д. б. ненулевой ук-ль.

            Bookmarks::PageBootstrap p;
            p.Render();
        };

        [TestMethod]
        void TestParamsParsing()
        {
            static TCHAR queryData[] = _T("http://fiteasily.com/cgi-bin/links.cgi?;log_in=lvbnhbq vjhjpjd&;log_in=dct pfrkflrb&;log_in=Ok");
            query = &queryData[0];
            Assert::IsTrue(check_log_in_params() == nullptr);
            Assert::IsTrue(std::wstring(username) == std::wstring(L"lvbnhbq vjhjpjd"));
            Assert::IsTrue(std::wstring(password) == std::wstring(L"dct pfrkflrb"));
            //  Все параметры должны быть удалены.
            Assert::IsTrue(std::wstring(query).find(L";&") == std::wstring::npos);
        };

        [TestMethod]
        void TestFileReader()
        {
            Bookmarks::FileReader fr(_T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk"));
            std::wstring url = fr.GetParamCurDir(_T("57564efb.url"), ParamURL);
            std::wstring name = fr.GetParamCurDir(_T("57564efb.url"), ParamName);

            Assert::IsTrue(_T("https://www.youtube.com/watch?v=_8v0Xs7MJLw") == url);
            Assert::IsTrue(_T("Москва и провинция: что там у людей? (Рыжков)") == name);

            Bookmarks::FileReader fr2(_T(""));
            std::wstring url2 = fr.GetParam(_T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\57564efb.url"), ParamURL);
            std::wstring name2 = fr.GetParam(_T("C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\57564efb.url"), ParamName);

            Assert::IsTrue(_T("https://www.youtube.com/watch?v=_8v0Xs7MJLw") == url2);
            Assert::IsTrue(_T("Москва и провинция: что там у людей? (Рыжков)") == name2);
        };

        [TestMethod]
        void TestGetKeyFileName()
        {
            std::wstring fileName = get_key_file_name();
        };

        [TestMethod]
        void TestMakeFolder()
        {
            const char *str = "C:\\04 - morozov\\my\\appsoft\\web\\robot\\bookmarks\\trunk\\TestFolder\\Тест";
            wchar_t wstr[MAX_LINE_LENGTH];
            MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, MAX_LINE_LENGTH);
            MakeFolder(wstr);
        };

        [TestMethod]
        void TestHandleAddFolderQuery()
        {
            TCHAR query[] = L"Estate%3Badd_folder=%D2%E5%F1%F2&%3Bkey%3D1471160335%3Badd_folder=Ok";
            HandleQuery(query, L"/cgi-bin/links.cgi");
        }

        [TestMethod]
        void TestHandleDeleteLinkQuery()
        {
            TCHAR query[] = L"Estate/%CF%F0%E0%E3%E0;key=1471160335;del_conf=57b2edc6.url";
            HandleQuery(query, L"/cgi-bin/links.cgi");
        }
    };
}
