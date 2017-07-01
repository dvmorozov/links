#include "stdafx.h"

#include "..\Models\Data.h"
#include "..\Models\ConfigTest.h"
#include "..\Models\FileList.h"
#include "..\Models\FileReader.h"
#include "..\Models\FileListReader.h"
#include "..\Models\FileListReaderTest.h"
#include "..\main.h"
#include "..\Views\FileListBootstrap.h"
#include "..\Controllers\Commands.h"

TCHAR test_out[100 * 1024];
TCHAR *test_str = test_out;

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace Bookmarks;

namespace bookmarks_test
{
    std::wstring testQuery;

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
                if (f->FileName == _T("Эхо Москвы")) matches.push_back(*f);
            }

            Assert::AreEqual(UInt32(1), matches.size());
        };

        [TestMethod]
        void TestRenderBootstrap()
        {
            //  https://action.mindjet.com/task/14777741
            testQuery = Bookmarks::RegConfig::GetValue(_T("TestQuery"));
            //  Allow string modification!
            query = (wchar_t*)testQuery.c_str();
            cwd = _T("test");       //  Д. б. ненулевой ук-ль.

            Bookmarks::FileListBootstrap p;
            p.Render();
        };

        [TestMethod]
        void TestParamsParsing()
        {
            //  https://action.mindjet.com/task/14777741
            testQuery = Bookmarks::RegConfig::GetValue(_T("TestQuery"));
            //  Allow string modification!
            query = (wchar_t*)testQuery.c_str();
            Assert::IsTrue(check_log_in_params() == nullptr);
            Assert::IsTrue(std::wstring(username) == Bookmarks::RegConfig::GetValue(_T("UserName")));
            Assert::IsTrue(std::wstring(password) == Bookmarks::RegConfig::GetValue(_T("Password")));
            //  Все параметры должны быть удалены.
            Assert::IsTrue(std::wstring(query).find(L";&") == std::wstring::npos);
        };

        [TestMethod]
        void TestFileReader()
        {
            Bookmarks::FileReader fr(Bookmarks::RegConfig::GetValue(_T("TestFolder")));
            std::wstring url = fr.GetParamCurDir(_T("57564efb.url"), ParamURL);
            std::wstring name = fr.GetParamCurDir(_T("57564efb.url"), ParamName);

            Assert::IsTrue(_T("https://www.youtube.com/watch?v=_8v0Xs7MJLw") == url);
            Assert::IsTrue(_T("Москва и провинция: что там у людей? (Рыжков)") == name);

            Bookmarks::FileReader fr2(_T(""));
            std::wstring url2 = fr.GetParam(Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\57564efb.url"), ParamURL);
            std::wstring name2 = fr.GetParam(Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\57564efb.url"), ParamName);

            Assert::IsTrue(_T("https://www.youtube.com/watch?v=_8v0Xs7MJLw") == url2);
            Assert::IsTrue(_T("Москва и провинция: что там у людей? (Рыжков)") == name2);
        };

        [TestMethod]
        void TestGetKeyFileName()
        {
            std::wstring fileName = getKeyFileName();
        };

        [TestMethod]
        void TestMakeFolder()
        {
            std::wstring wstr = Bookmarks::RegConfig::GetValue(_T("TestFolder")) + L"\\Тест";
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

        [TestMethod]
        //  https://action.mindjet.com/task/14732139
        void TestDeleteDuplicates()
        {
            //  Copies files to the test directory.
            FileListReader *flrt = GetFileReader();
            //  Overwriting test file.
            CopyFile(
                (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\14732139\\test.txt")).c_str(),
                (Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\Тест\\test.txt")).c_str(),
                FALSE
            );
            _wchdir((Bookmarks::RegConfig::GetValue(_T("TestFolder")) + _T("\\Тест")).c_str());

            FileList fl(flrt);
            auto fileListBefore = fl.GetFileList();
            Assert::AreEqual((UInt32)3, fileListBefore.size());

            fl.DeleteDuplicates();

            auto fileListAfter = fl.GetFileList();
            Assert::AreEqual((UInt32)2, fileListAfter.size());
        }
    };
}
