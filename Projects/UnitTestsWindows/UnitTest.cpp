
#ifdef _WINDOWS
#include <Windows.h>
#endif

#include "../App/stdafx.h"
#include "../Models/Data.h"
#include "../Models/Config.h"
#include "../Models/ConfigFactory.h"
#include "../Models/FileList.h"
#include "../Models/LinkFileReader.h"
#include "../Models/TextFileReader.h"
#include "../Views/FileListBootstrap.h"
#include "../Controllers/Commands.h"
#include "../Utils/Miscellaneous.h"
#include "../App/Globals.h"

#include <memory>
#include <iostream>
#include <filesystem>

TCHAR test_out[100 * 1024];
TCHAR *test_str = test_out;

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bookmarks;

namespace NativeTest
{
    TEST_CLASS(FeatureTest)
    {
    public:
        //  Integration test reading file and directory list of current directory by default reader.
        TEST_METHOD(TestReadBookmarksData)
        {
            Login();

            //  Content of the directory must be preserved, so it is not copied!
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            auto testFolder = config->GetValue(_T("TestFolder")) + _T("TestFileList/");

            //  Moving to the test directory to read test files.
            //  TODO: check returning value.
            _wchdir(testFolder.c_str());

            Bookmarks::Data d(1251);

            FileVector files = d.GetFileList();
            FileVector dirs = d.GetDirList();

            Assert::AreEqual(5, (int)files.size());
            Assert::AreEqual(5, (int)dirs.size());
        };

        //  Integration test reading file and directory list from filesystem.
        TEST_METHOD(TestFileList)
        {
            Login();

            //  Content of the directory must be preserved, so it is not copied!
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            auto testFolder = config->GetValue(_T("TestFolder")) + _T("TestFileList/");

            //  Moving to the test directory to read test files.
            //  TODO: check returning value.
            _wchdir(testFolder.c_str());

            TextFileReader *flrt = FileListReaderDir::GetFileReader(1251);
            Bookmarks::FileList fl(flrt);

            FileVector files = fl.GetFileList();
            FileVector dirs = fl.GetDirList();

            //  Only files with the ".url" extension are counted.
            Assert::AreEqual(5, (int)files.size());
            //  Dirs ".", ".." are included in the count.
            Assert::AreEqual(5, (int)dirs.size());
        };

        //  Integration test checking reading directory list for directories containing spaces and cyrilic symbols.
        TEST_METHOD(TestCheckNamesWithSpaces)
        {
            Login();
            //  This doesn't work when the executable is started under IIS but the test doesn't work without this.
            //  Without that the command writes "?" instead of cyrilic symbols in the temporary file.
            //  The result indicates failure and must be ignored. But the test works.
            SetConsoleOutputCP(1251);

            //  TODO: session key must be initialized.
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            auto testFolder = config->GetValue(_T("TestFolder")) + _T("TestCheckNamesWithSpaces/");

            //  Moving to the test directory to read test files.
            //  TODO: check returning value.
            _wchdir(testFolder.c_str());

            Bookmarks::Data d(1251);

            FileVector _data = d.GetDirList();
            FileVector matches;

            for (auto f = _data.begin(); f != _data.end(); ++f)
            {
                //  Directory name is in UTF-8 encoding.
                if (f->FileName == _T("Эхо Москвы")) 
                    matches.push_back(*f);
            }

            Assert::AreEqual(1, (int)matches.size());
        };

        TEST_METHOD(TestRenderBootstrap)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            pepare_query_buffer(config->GetValue(_T("LoginQuery")).size());
            wcscpy(query, config->GetValue(_T("LoginQuery")).c_str());
            //  The cmd variable must be set!
            cwd = (TCHAR*)_T("test");

            Bookmarks::FileListBootstrap p;
            p.Render();
            Assert::IsTrue(fatal_error != 1);
        };

        TEST_METHOD(TestLoginParametersParsing)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            pepare_query_buffer(config->GetValue(_T("LoginQuery")).size());
            wcscpy(query, config->GetValue(_T("LoginQuery")).c_str());

            Assert::ExpectException<std::exception>([this](){check_log_in_params()});
            Assert::IsTrue(std::wstring(username) == config->GetValue(_T("UserName")));
            Assert::IsTrue(std::wstring(password) == config->GetValue(_T("Password")));
            //  All the parameters must be removed from the query string.
            Assert::IsTrue(std::wstring(query).find(L";&") == std::wstring::npos);
        };

        TEST_METHOD(TestHtmlDecoding)
        {
            const std::wstring test_query = L"/cgi-bin/links.cgi?/Radio;add=Radiot&#233;ka&;add=Ok";

            pepare_query_buffer(test_query.size());
            decode_html_entities_utf8(query, test_query.c_str());
            Assert::IsTrue(std::wstring(L"/cgi-bin/links.cgi?/Radio;add=Radiotéka&;add=Ok") == query);
        };

        //  Integration test reading parameters from file located in current directory
        //  or a directory given by absolute path. Test file must be encoded in Windows-1251
        //  and contain cyrilic characters.
        TEST_METHOD(TestFileReader)
        {
            //  Content of the directory must be preserved, so it is not copied!
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            auto testFolder = config->GetValue(_T("TestFolder")) + _T("TestFileReader/");

            //  Moving to the test directory to read test files.
            //  TODO: check returning value.
            _wchdir(testFolder.c_str());

            //  Test file must be encoded in Windows-1251.
            //  Reading data from file from the current directory identified by "TestFolder".
            std::wstring fileName = _T("57564efb.url");
            
            //  Current directory name is not updated!
            Bookmarks::LinkFileReader fr(testFolder);
            std::wstring url = fr.GetParamCurDir(fileName, param_url);
            std::wstring name = fr.GetParamCurDir(fileName, param_name);

            Assert::IsTrue(L"https://www.youtube.com/watch?v=_8v0Xs7MJLw" == url);
            Assert::IsTrue(L"Москва и провинция: что там у людей? (Рыжков)" == name);

            //  Reading data from file from the directory set by absolute path.
            //  TODO: use Boost to concatenate paths. 
            //  For now "testFolder" must contain terminating path separator!
            std::wstring filePath = testFolder + fileName;
            Bookmarks::LinkFileReader fr2(_T(""));
            std::wstring url2 = fr2.GetParam(filePath, param_url);
            std::wstring name2 = fr2.GetParam(filePath, param_name);

            Assert::IsTrue(L"https://www.youtube.com/watch?v=_8v0Xs7MJLw" == url2);
            Assert::IsTrue(L"Москва и провинция: что там у людей? (Рыжков)" == name2);
        };

        TEST_METHOD(TestReadingUTF8File)
        {
            //  Content of the directory must be preserved, so it is not copied!
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            auto testFolder = config->GetValue(_T("TestFolder")) + _T("TestReadingUTF8File/");

            //  TODO: Rename files as *.utf8.url.
            std::wstring fileName = _T("57564efb.url.utf8");

            //  Reading data from file from the directory set by absolute path.
            //  TODO: use Boost to concatenate paths. 
            //  For now "testFolder" must contain terminating path separator!
            std::wstring filePath = testFolder + fileName;
            Bookmarks::LinkFileReader fr2(_T(""));
            std::wstring url = fr2.GetParam(filePath, param_url);
            std::wstring name = fr2.GetParam(filePath, param_name);

            Assert::IsTrue(L"https://www.youtube.com/watch?v=_8v0Xs7MJLw" == url);
            Assert::IsTrue(L"Москва и провинция: что там у людей? (Рыжков)" == name);
        };

        TEST_METHOD(TestCreatingUTF8File)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            auto tempFolder = config->GetValue(_T("TestFolder")) + _T("Тест/");

            std::wstring testFileDir = tempFolder + _T("TestCreatingUTF8File");
           
            //  Directory must exist before creating a file.
            if (!std::filesystem::exists(testFileDir))
                std::filesystem::create_directory(testFileDir);

            //  Full file path is returned...
            std::wstring testFilePath = create_url_file(_T("Radiotéka"), _T("http://www.radioteka.cz/"), testFileDir.c_str());

            //  ...so path is not to the reader.
            Bookmarks::LinkFileReader fr(L"");

            std::wstring name = fr.GetParam(testFilePath, param_name);
            Assert::IsTrue(_T("Radiotéka") == name);
        };

        TEST_METHOD(TestGetKeyFileName)
        {
            std::wstring fileName = get_key_file_path();
            Assert::IsNotNull(key);
        };

        TEST_METHOD(TestMakeFolder)
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            std::wstring wstr = config->GetValue(_T("TestFolder")) + L"\\Тест";
            make_directory(wstr);
        };

        TEST_METHOD(TestHandleAddFolderQuery)
        {
            //  Login is not required because the key is contained in the URL.
            handle_query(std::wstring(L"Estate%3Badd_folder=%D2%E5%F1%F2&%3Bkey%3D1471160335%3Badd_folder=Ok").c_str());
        }

        //  Creates new session file and fills the "key" variable.
        void Login()
        {
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());

            //  Must be set before processing query.
            document_root = config->GetValue(_T("DOCUMENT_ROOT"));

            handle_query(config->GetValue(_T("LoginQuery")).c_str());
            Assert::IsTrue(fatal_error != 1);
        }

        TEST_METHOD(TestAddLink)
        {
            //  Login is not required because the key is contained in the URL.
            //  The key int URL must coincide with name of temporary file.
            //  The URL mustn't include domain and script name.
            std::wstring testQueryAddLink = L"Radio%3Badd=Radiot%26%23233%3Bka&%3Badd=http%3A%2F%2Fwww.radioteka.cz%2F&%3Bkey%3D1471160335%3Badd=Ok";

            handle_query(testQueryAddLink.c_str());
            Assert::IsTrue(fatal_error != 1);
        };

        TEST_METHOD(TestHandleDeleteLinkQuery)
        {
            //  Login is not required because the key is contained in the URL.
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            //  Copies files to the test directory.
            TextFileReader* flrt = FileListReaderDir::GetFileReader(1251);
            auto testFolder = config->GetValue(_T("TestFolder")) + _T("TestHandleDeleteLinkQuery/");
            auto tempFolder = config->GetValue(_T("TestFolder")) + _T("users/links/test/Estate/Прага");

            if (!std::filesystem::exists(tempFolder))
                std::filesystem::create_directory(tempFolder);

            //  Copying test files to temporary test directory.
            std::filesystem::remove_all(tempFolder);
            std::filesystem::copy(testFolder, tempFolder);

            //  URL must be passed in the decoded form.
            //  Encode URL: L"Estate%2F%D0%9F%D1%80%D0%B0%D0%B3%D0%B0%3Bkey%3D1471160335%3Bdel_conf%3D57564efb.url".
            std::wstring testQueryDeleteLink = L"Estate%2F%D0%9F%D1%80%D0%B0%D0%B3%D0%B0;key=1471160335;del_conf=57564efb.url";
            handle_query(testQueryDeleteLink.c_str());
            Assert::IsTrue(fatal_error != 1);
        }

        TEST_METHOD(TestChangeFolder)
        {
            //  Login is not required because the key is contained in the URL.
            //  Key must correspond to test user data ("1471160335").
            //  Business/Трейдинг;key=1471160335;ch_folder=Ok
            //  Query must be set without script path!
            //  http://localhost/cgi-bin/links-legacy.exe?Business/Трейдинг;key=1471160335;ch_folder=Ok
            handle_query(L"Business/%D0%A2%D1%80%D0%B5%D0%B9%D0%B4%D0%B8%D0%BD%D0%B3;key=1471160335;ch_folder=Ok");
            const std::wstring currentDirAfter = get_current_working_directory();

            Assert::IsTrue(currentDirAfter.find(L"Business") != std::wstring::npos);
            Assert::IsTrue(fatal_error != 1);
        }

        TEST_METHOD(TestDeleteDuplicates)
        {
            Login();

            //  Copies files to the test directory.
            //  Files must be encoded by UTF-8.
            std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
            auto testFolder = config->GetValue(_T("TestFolder")) + _T("TestDeleteDuplicates/");
            auto tempFolder = config->GetValue(_T("TestFolder")) + _T("Тест/");

            //  Copying test files to temporary test directory.
            std::filesystem::remove_all(tempFolder);
            std::filesystem::copy(testFolder, tempFolder);

            //  Moving to the test directory to read test files.
            //  TODO: check returning value.
            _wchdir(tempFolder.c_str());

            //  Reads original file list from test directory. 
            //  Test directory must contain files having duplicated URLs.
            TextFileReader* flrt = FileListReaderDir::GetFileReader(1251);
            Bookmarks::FileList fl(flrt);

            auto fileListBefore = fl.GetFileList();
            Assert::AreEqual(3, (int)fileListBefore.size());

            fl.DeleteFilesWithDuplicatedUrls();

            //  Reads resulting file list.
            auto fileListAfter = fl.GetFileList();
            Assert::AreEqual(2, (int)fileListAfter.size());
        }
    };
}
