#include <cstdio>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file.
#include <catch2/catch.hpp>

#include "App/stdafx.h"
#include "Utils/Miscellaneous.h"

TEST_CASE("Converts an empty wide string", "[narrow]") {
    std::wstring emptyWideString = L"";
    std::string result = narrow(emptyWideString);
    REQUIRE(result == "");
}

TEST_CASE("Converts ASCII characters in wide string", "[narrow]") {
    std::wstring asciiWideString = L"Hello, World!";
    std::string result = narrow(asciiWideString);
    REQUIRE(result == "Hello, World!");
}

TEST_CASE("Converts wide string with Unicode characters", "[narrow]") {
    std::wstring unicodeWideString = L"こんにちは世界"; // "Hello, World!" in Japanese.
    std::string result = narrow(unicodeWideString);
    REQUIRE(result == "こんにちは世界");
}

TEST_CASE("concatenate_paths correctly concatenates paths with appropriate delimiter", "[path]") {
    SECTION("Concatenates two normal paths") {
        REQUIRE(concatenate_paths(L"home", L"user") == L"home/user");
    }

    SECTION("Handles leading delimiter in second path part") {
        REQUIRE(concatenate_paths(L"home", L"/user") == L"home/user");
    }

    SECTION("Handles trailing delimiter in first path part") {
        REQUIRE(concatenate_paths(L"home/", L"user") == L"home/user");
    }

    SECTION("Correctly handles root paths") {
        REQUIRE(concatenate_paths(L"/", L"user") == L"/user");
    }

    SECTION("Returns second part if first part is empty") {
        REQUIRE(concatenate_paths(L"", L"user") == L"user");
    }

    SECTION("Returns first part if second part is empty") {
        REQUIRE(concatenate_paths(L"home", L"") == L"home");
    }

    SECTION("Concatenates without adding extra delimiters") {
        REQUIRE(concatenate_paths(L"home/", L"/user") == L"home/user");
    }

    SECTION("Handles both parts being empty") {
        REQUIRE(concatenate_paths(L"", L"") == L"");
    }
}

#ifdef _WINDOWS
TEST_CASE("concatenate_paths uses backslash for Windows", "[path][windows]") {
    REQUIRE(concatenate_paths(L"C:", L"Windows") == L"C:\\Windows");
}
#else
TEST_CASE("concatenate_paths uses forward slash for non-Windows", "[path][non-windows]") {
    REQUIRE(concatenate_paths(L"usr", L"bin") == L"usr/bin");
}
#endif

TEST_CASE("widen function converts narrow strings to wide strings", "[string_conversion]") {
    SECTION("Converts ASCII string correctly") {
        std::string narrow = "Hello, World!";
        std::wstring wide = L"Hello, World!";
        REQUIRE(widen(narrow) == wide);
    }

    SECTION("Converts from UTF-8 correctly") {
        std::string narrow = "Привет, мир!"; // Example in Russian
        std::wstring wide = L"Привет, мир!";
        REQUIRE(widen(narrow, "UTF-8") == wide);
    }

    SECTION("Handles empty string") {
        std::string narrow = "";
        std::wstring wide = L"";
        REQUIRE(widen(narrow) == wide);
    }

    //  This test assumes the system's default codepage can correctly handle the conversion.
    //  You might need to adjust the codepage or the test string depending on your environment.
    /*  TODO: fix this test.
    SECTION("Converts using non-default codepage") {
        std::string narrow = "\xC2\xA2"; // Cent sign in ISO-8859-1
        std::wstring wide = L"¢";
        REQUIRE(widen(narrow, "ISO-8859-1") == wide);
    }
    */

    SECTION("Returns empty string on conversion failure") {
        std::string narrow = "\xFF"; // Invalid UTF-8 sequence.
        std::wstring wide = L"";
        REQUIRE(widen(narrow, "UTF-8") == wide);
    }
}