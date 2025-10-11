//  Use snake_case code style for generic function.

#pragma once
#include "App/Globals.h"

//  Working with URL-files.
extern const std::wstring ext_utf8;
extern const std::wstring ext_url;
extern const std::wstring param_url;
extern const std::wstring param_name;
std::wstring              create_url_file(const TCHAR *link_name, const TCHAR *url,
                                          const TCHAR *link_directory = nullptr);

//  Working with encoding.
std::string utf8_to_cp1251(const std::string &utf8);
size_t      decode_html_entities_utf8(TCHAR *dest, const TCHAR *src);
/*	Takes input from <src> and decodes into <dest>, which should be a buffer
    large enough to hold <strlen(src) + 1> characters.

    If <src> is <NULL>, input will be taken from <dest>, decoding
    the entities in-place.

    The function returns the length of the decoded string.
*/
void decode_url(TCHAR *dest, TCHAR *src, unsigned char delete_spaces);
void decode_unicode_escapes(TCHAR *dest, const TCHAR *src);

//  Working with directories and paths.
bool         change_working_directory(const std::wstring &wide_path);
std::wstring concatenate_paths(const std::wstring &part1, const std::wstring &part2);
bool         directory_exists(const std::wstring &path);
std::wstring get_current_working_directory();
std::wstring get_directory_name(const std::wstring &file_path);
std::wstring get_wide_environment_variable(const std::wstring &name);

int hex_char_to_int(TCHAR c);

std::wstring extract_domain(const std::wstring &url);

//  Working with wide-character strings.
const wchar_t *create_new_wide_string(const char *c);
std::string    narrow(const std::wstring &wide_string);
#ifdef _WINDOWS
std::wstring widen(const std::string &narrow_str, unsigned int code_page = CP_UTF8);
#else
std::wstring widen(const std::string &narrow_str, const std::string &code_page = "UTF-8");
//	These functions cause warnings under Windows proposing usage of native API functions.
char *malloc_narrow(const std::wstring &wide_string);
#endif

template <typename T> T substring_before_last(const T &input, typename T::value_type delimiter)
{
    // Find the position of the last delimiter
    typename T::size_type lastDelimiterPos = input.find_last_of(delimiter);

    // Check if the delimiter was found.
    if (lastDelimiterPos != T::npos) {
        // Extract the substring before the last delimiter.
        return input.substr(0, lastDelimiterPos);
    } else {
        // If no delimiter was found, return the entire input string.
        return input;
    }
}

/*
* TODO: create unit-tests and use this function.
template<typename... Args>
std::wstring format_wstring(const std::wstring& format, Args... args) {
    std::wstringstream stream;
    std::wistringstream formatStream(format);
    wchar_t ch;
    int argIndex = 0;
    std::tuple<Args...> argTuple(args...);

    auto insertArg = [&](int index) {
        if (index < 0 || index >= sizeof...(args)) {
            throw std::out_of_range("Argument index out of range in format_wstring");
        }
        ([&](auto& arg) {
            if (index == argIndex++) {
                stream << arg;
            }
            }(std::get<Args>(argTuple)), ...);
        };

    while (formatStream.get(ch)) {
        if (ch == L'{') {
            if (formatStream.peek() == L'{') {
                stream.put(L'{');
                formatStream.get(ch); // Consume the next '{'
            }
            else {
                int index = 0;
                if (formatStream >> index) {
                    insertArg(index);
                    if (formatStream.get(ch) && ch != L'}') {
                        throw std::runtime_error("Expected '}' after argument index in
format_wstring");
                    }
                }
                else {
                    throw std::runtime_error("Expected argument index in format_wstring");
                }
            }
        }
        else if (ch == L'}' && formatStream.peek() == L'}') {
            stream.put(L'}');
            formatStream.get(ch); // Consume the next '}'
        }
        else {
            stream.put(ch);
        }
    }

    return stream.str();
}
*/
