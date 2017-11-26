#include "stdafx.h"

#include "..\Models\Data.h"
#include "..\Models\Config.h"
#include "..\Models\ConfigFactory.h"
#include "..\Models\FileList.h"
#include "..\Models\LinkFileReader.h"
#include "..\Models\TextFileReader.h"
#include "..\Views\FileListBootstrap.h"
#include "..\Controllers\Commands.h"
#include "..\Utils\entities.h"
#include "..\main.h"

#include <memory>

TCHAR test_out[100 * 1024];
TCHAR *test_str = test_out;

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bookmarks;

namespace NativeTest
{
    TEST_CLASS(UnitTest)
    {
    private:
        std::wstring testQuery;

    public:
        TEST_METHOD(TestGetFileList)
        {
            TextFileReader *flrt = GetFileReader();
            //  Returns all the string from file.
            std::vector<std::wstring> list = flrt->ReadFileLines();

            Assert::AreEqual(36, (int)list.size());
            delete flrt;
        };

        TEST_METHOD(TestReadFileList)
        {
            Bookmarks::Data d;

            FileVector files = d.GetFileList();
            FileVector dirs = d.GetDirList();

            Assert::AreEqual(22, (int)files.size());
            Assert::AreEqual(6, (int)dirs.size());
        };

        TEST_METHOD(TestFileList)
        {
            TextFileReader *flrt = GetFileReader();
            Bookmarks::FileList fl(flrt);

            FileVector files = fl.GetFileList();
            FileVector dirs = fl.GetDirList();

            Assert::AreEqual(22, (int)files.size());
            Assert::AreEqual(6, (int)dirs.size());
        };

        TEST_METHOD(TestCheckNamesWithSpaces)
        {
            Bookmarks::Data d;

            FileVector _data = d.GetDirList();
            FileVector matches;

            for (auto f = _data.begin(); f != _data.end(); ++f)
            {
                if (f->_fileName == _T("Эхо Москвы")) 
                    matches.push_back(*f);
            }

            Assert::AreEqual(1, (int)matches.size());
        };

        TEST_METHOD(TestRenderBootstrap)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            //  https://action.mindjet.com/task/14777741
            testQuery = config->GetValue(_T("TestQuery"));
            //  Allow string modification!
            query = (wchar_t*)testQuery.c_str();
            cwd = _T("test");       //  Д. б. ненулевой ук-ль.

            Bookmarks::FileListBootstrap p;
            p.Render();
            Assert::IsTrue(fatal_error != 1);
        };

        TEST_METHOD(TestParamsParsing)
        {
            //  https://action.mindjet.com/task/14777741
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            testQuery = config->GetValue(_T("TestQuery"));
            //  Allow string modification!
            query = (wchar_t*)testQuery.c_str();
            Assert::IsTrue(check_log_in_params() == nullptr);
            Assert::IsTrue(std::wstring(username) == config->GetValue(_T("UserName")));
            Assert::IsTrue(std::wstring(password) == config->GetValue(_T("Password")));
            //  Все параметры должны быть удалены.
            Assert::IsTrue(std::wstring(query).find(L";&") == std::wstring::npos);
        };

        TEST_METHOD(TestHtmlDecoding)
        {
            //  https://action.mindjet.com/task/14817423
            testQuery = L"/cgi-bin/links.cgi?/Radio;add=Radiot&#233;ka&;add=Ok";

            //  https://action.mindjet.com/task/14817423
            prepare_query_buffer((int)testQuery.size());
            decode_html_entities_utf8(query, testQuery.c_str());
            Assert::IsTrue(std::wstring(L"/cgi-bin/links.cgi?/Radio;add=Radiotéka&;add=Ok") == query);
        };

        TEST_METHOD(TestFileReader)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            Bookmarks::LinkFileReader fr(config->GetValue(_T("TestFolder")));
            std::wstring url = fr.GetParamCurDir(_T("57564efb.url"), ParamURL);
            std::wstring name = fr.GetParamCurDir(_T("57564efb.url"), ParamName);

            Assert::IsTrue(_T("https://www.youtube.com/watch?v=_8v0Xs7MJLw") == url);
            Assert::IsTrue(_T("Москва и провинция: что там у людей? (Рыжков)") == name);

            Bookmarks::LinkFileReader fr2(_T(""));
            std::wstring url2 = fr.GetParam(config->GetValue(_T("TestFolder")) + _T("\\57564efb.url"), ParamURL);
            std::wstring name2 = fr.GetParam(config->GetValue(_T("TestFolder")) + _T("\\57564efb.url"), ParamName);

            Assert::IsTrue(_T("https://www.youtube.com/watch?v=_8v0Xs7MJLw") == url2);
            Assert::IsTrue(_T("Москва и провинция: что там у людей? (Рыжков)") == name2);
        };

        TEST_METHOD(TestUTF8File)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            Bookmarks::LinkFileReader fr(config->GetValue(_T("TestFolder")));

            std::wstring folderName = config->GetValue(_T("TestFolder")) + _T("\\14817423");
            std::wstring fileName = create_url_file(_T("Radiotéka"), _T("http://www.radioteka.cz/"), folderName.c_str());

            std::wstring localName = fr.GetParam(fileName, ParamName);
            Assert::IsTrue(_T("Radiotéka") == localName);
        };

        TEST_METHOD(TestGetKeyFileName)
        {
            std::wstring fileName = getKeyFileName();
        };

        TEST_METHOD(TestMakeFolder)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            std::wstring wstr = config->GetValue(_T("TestFolder")) + L"\\Тест";
            MakeFolder(wstr);
        };

        TEST_METHOD(TestHandleAddFolderQuery)
        {
            testQuery = L"Estate%3Badd_folder=%D2%E5%F1%F2&%3Bkey%3D1471160335%3Badd_folder=Ok";
            HandleQuery((wchar_t*)testQuery.c_str()
#ifdef USE_SCRIPT_NAME
                , L"/cgi-bin/links.cgi"
#endif
            );
        }

        //  https://action.mindjet.com/task/14817423
        TEST_METHOD(TestAddLink)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            testQuery = config->GetValue(_T("TestQuery"));
            //  The key int URL must coincide with name of temporary file.
            //  The URL mustn't include domain and script name.
            std::wstring testQueryAddLink = L"Radio%3Badd=Radiot%26%23233%3Bka&%3Badd=http%3A%2F%2Fwww.radioteka.cz%2F&%3Bkey%3D1471160335%3Badd=Ok";
            //  Allow string modification!
            query = (wchar_t*)testQuery.c_str();
            Assert::IsTrue(check_log_in_params() == nullptr);

            HandleQuery((wchar_t*)testQueryAddLink.c_str()
#ifdef USE_SCRIPT_NAME
                , L"/cgi-bin/links.cgi"
#endif
            );
            Assert::IsTrue(fatal_error != 1);
        };

        TEST_METHOD(TestHandleDeleteLinkQuery)
        {
            testQuery = L"Estate/%CF%F0%E0%E3%E0;key=1471160335;del_conf=57b2edc6.url";
            HandleQuery((wchar_t*)testQuery.c_str()
#ifdef USE_SCRIPT_NAME
                , L"/cgi-bin/links.cgi"
#endif
            );
        }

        TEST_METHOD(TestChangeFolder)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            testQuery = config->GetValue(_T("TestQuery"));
            //  Allow string modification!
            query = (wchar_t*)testQuery.c_str();
            //  Log in must be done before processing command.
            Assert::IsTrue(check_log_in_params() == nullptr);
            //  Key must correspond to test user data ("1471160335").
            testQuery = L"Business/%D0%A2%D1%80%D0%B5%D0%B9%D0%B4%D0%B8%D0%BD%D0%B3;key=1471160335;ch_folder=Ok";
            HandleQuery((wchar_t*)testQuery.c_str()
#ifdef USE_SCRIPT_NAME
                , L"/cgi-bin/links.cgi"
#endif
            );
            Assert::IsTrue(fatal_error != 1);
        }

        //  https://action.mindjet.com/task/14732139
        TEST_METHOD(TestDeleteDuplicates)
        {
            //  Copies files to the test directory.
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            TextFileReader *flrt = GetFileReader();
            auto testFolder = config->GetValue(_T("TestFolder"));
            auto outFileName = testFolder + _T("\\Тест\\test.txt");

            //  Overwriting test file.
            CopyFile(
                (config->GetValue(_T("TestFolder")) + _T("\\14732139\\test.txt")).c_str(),
                outFileName.c_str(),
                FALSE
            );
            _wchdir((config->GetValue(_T("TestFolder")) + _T("\\Тест")).c_str());
            flrt->SetFileName(outFileName);

            FileList fl(flrt);
            auto fileListBefore = fl.GetFileList();
            Assert::AreEqual(3, (int)fileListBefore.size());

            fl.DeleteDuplicates();

            auto fileListAfter = fl.GetFileList();
            Assert::AreEqual(2, (int)fileListAfter.size());
        }
    };
}
