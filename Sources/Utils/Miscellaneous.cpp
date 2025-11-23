#include "App/stdafx.h"

#include "Miscellaneous.h"
#include "Utils/DebugLog.h"
#include "App/Globals.h"
#include "Errors.h"
#include "Shared/Models/Config.h"
#include "Shared/Models/ConfigFactory.h"

#define UNICODE_MAX 0x10FFFFul

static const TCHAR *const NAMED_ENTITIES[][2] = {{_T("AElig;"), _T("Æ")},
                                                 {_T("Aacute;"), _T("Á")},
                                                 {_T("Acirc;"), _T("Â")},
                                                 {_T("Agrave;"), _T("À")},
                                                 {_T("Alpha;"), _T("Α")},
                                                 {_T("Aring;"), _T("Å")},
                                                 {_T("Atilde;"), _T("Ã")},
                                                 {_T("Auml;"), _T("Ä")},
                                                 {_T("Beta;"), _T("Β")},
                                                 {_T("Ccedil;"), _T("Ç")},
                                                 {_T("Chi;"), _T("Χ")},
                                                 {_T("Dagger;"), _T("‡")},
                                                 {_T("Delta;"), _T("Δ")},
                                                 {_T("ETH;"), _T("Ð")},
                                                 {_T("Eacute;"), _T("É")},
                                                 {_T("Ecirc;"), _T("Ê")},
                                                 {_T("Egrave;"), _T("È")},
                                                 {_T("Epsilon;"), _T("Ε")},
                                                 {_T("Eta;"), _T("Η")},
                                                 {_T("Euml;"), _T("Ë")},
                                                 {_T("Gamma;"), _T("Γ")},
                                                 {_T("Iacute;"), _T("Í")},
                                                 {_T("Icirc;"), _T("Î")},
                                                 {_T("Igrave;"), _T("Ì")},
                                                 {_T("Iota;"), _T("Ι")},
                                                 {_T("Iuml;"), _T("Ï")},
                                                 {_T("Kappa;"), _T("Κ")},
                                                 {_T("Lambda;"), _T("Λ")},
                                                 {_T("Mu;"), _T("Μ")},
                                                 {_T("Ntilde;"), _T("Ñ")},
                                                 {_T("Nu;"), _T("Ν")},
                                                 {_T("OElig;"), _T("Œ")},
                                                 {_T("Oacute;"), _T("Ó")},
                                                 {_T("Ocirc;"), _T("Ô")},
                                                 {_T("Ograve;"), _T("Ò")},
                                                 {_T("Omega;"), _T("Ω")},
                                                 {_T("Omicron;"), _T("Ο")},
                                                 {_T("Oslash;"), _T("Ø")},
                                                 {_T("Otilde;"), _T("Õ")},
                                                 {_T("Ouml;"), _T("Ö")},
                                                 {_T("Phi;"), _T("Φ")},
                                                 {_T("Pi;"), _T("Π")},
                                                 {_T("Prime;"), _T("″")},
                                                 {_T("Psi;"), _T("Ψ")},
                                                 {_T("Rho;"), _T("Ρ")},
                                                 {_T("Scaron;"), _T("Š")},
                                                 {_T("Sigma;"), _T("Σ")},
                                                 {_T("THORN;"), _T("Þ")},
                                                 {_T("Tau;"), _T("Τ")},
                                                 {_T("Theta;"), _T("Θ")},
                                                 {_T("Uacute;"), _T("Ú")},
                                                 {_T("Ucirc;"), _T("Û")},
                                                 {_T("Ugrave;"), _T("Ù")},
                                                 {_T("Upsilon;"), _T("Υ")},
                                                 {_T("Uuml;"), _T("Ü")},
                                                 {_T("Xi;"), _T("Ξ")},
                                                 {_T("Yacute;"), _T("Ý")},
                                                 {_T("Yuml;"), _T("Ÿ")},
                                                 {_T("Zeta;"), _T("Ζ")},
                                                 {_T("aacute;"), _T("á")},
                                                 {_T("acirc;"), _T("â")},
                                                 {_T("acute;"), _T("´")},
                                                 {_T("aelig;"), _T("æ")},
                                                 {_T("agrave;"), _T("à")},
                                                 {_T("alefsym;"), _T("ℵ")},
                                                 {_T("alpha;"), _T("α")},
                                                 {_T("amp;"), _T("&")},
                                                 {_T("and;"), _T("∧")},
                                                 {_T("ang;"), _T("∠")},
                                                 {_T("apos;"), _T("'")},
                                                 {_T("aring;"), _T("å")},
                                                 {_T("asymp;"), _T("≈")},
                                                 {_T("atilde;"), _T("ã")},
                                                 {_T("auml;"), _T("ä")},
                                                 {_T("bdquo;"), _T("„")},
                                                 {_T("beta;"), _T("β")},
                                                 {_T("brvbar;"), _T("¦")},
                                                 {_T("bull;"), _T("•")},
                                                 {_T("cap;"), _T("∩")},
                                                 {_T("ccedil;"), _T("ç")},
                                                 {_T("cedil;"), _T("¸")},
                                                 {_T("cent;"), _T("¢")},
                                                 {_T("chi;"), _T("χ")},
                                                 {_T("circ;"), _T("ˆ")},
                                                 {_T("clubs;"), _T("♣")},
                                                 {_T("cong;"), _T("≅")},
                                                 {_T("copy;"), _T("©")},
                                                 {_T("crarr;"), _T("↵")},
                                                 {_T("cup;"), _T("∪")},
                                                 {_T("curren;"), _T("¤")},
                                                 {_T("dArr;"), _T("⇓")},
                                                 {_T("dagger;"), _T("†")},
                                                 {_T("darr;"), _T("↓")},
                                                 {_T("deg;"), _T("°")},
                                                 {_T("delta;"), _T("δ")},
                                                 {_T("diams;"), _T("♦")},
                                                 {_T("divide;"), _T("÷")},
                                                 {_T("eacute;"), _T("é")},
                                                 {_T("ecirc;"), _T("ê")},
                                                 {_T("egrave;"), _T("è")},
                                                 {_T("empty;"), _T("∅")},
                                                 {_T("emsp;"), _T("\xE2\x80\x83")},
                                                 {_T("ensp;"), _T("\xE2\x80\x82")},
                                                 {_T("epsilon;"), _T("ε")},
                                                 {_T("equiv;"), _T("≡")},
                                                 {_T("eta;"), _T("η")},
                                                 {_T("eth;"), _T("ð")},
                                                 {_T("euml;"), _T("ë")},
                                                 {_T("euro;"), _T("€")},
                                                 {_T("exist;"), _T("∃")},
                                                 {_T("fnof;"), _T("ƒ")},
                                                 {_T("forall;"), _T("∀")},
                                                 {_T("frac12;"), _T("½")},
                                                 {_T("frac14;"), _T("¼")},
                                                 {_T("frac34;"), _T("¾")},
                                                 {_T("frasl;"), _T("⁄")},
                                                 {_T("gamma;"), _T("γ")},
                                                 {_T("ge;"), _T("≥")},
                                                 {_T("gt;"), _T(">")},
                                                 {_T("hArr;"), _T("⇔")},
                                                 {_T("harr;"), _T("↔")},
                                                 {_T("hearts;"), _T("♥")},
                                                 {_T("hellip;"), _T("…")},
                                                 {_T("iacute;"), _T("í")},
                                                 {_T("icirc;"), _T("î")},
                                                 {_T("iexcl;"), _T("¡")},
                                                 {_T("igrave;"), _T("ì")},
                                                 {_T("image;"), _T("ℑ")},
                                                 {_T("infin;"), _T("∞")},
                                                 {_T("int;"), _T("∫")},
                                                 {_T("iota;"), _T("ι")},
                                                 {_T("iquest;"), _T("¿")},
                                                 {_T("isin;"), _T("∈")},
                                                 {_T("iuml;"), _T("ï")},
                                                 {_T("kappa;"), _T("κ")},
                                                 {_T("lArr;"), _T("⇐")},
                                                 {_T("lambda;"), _T("λ")},
                                                 {_T("lang;"), _T("〈")},
                                                 {_T("laquo;"), _T("«")},
                                                 {_T("larr;"), _T("←")},
                                                 {_T("lceil;"), _T("⌈")},
                                                 {_T("ldquo;"), _T("“")},
                                                 {_T("le;"), _T("≤")},
                                                 {_T("lfloor;"), _T("⌊")},
                                                 {_T("lowast;"), _T("∗")},
                                                 {_T("loz;"), _T("◊")},
                                                 {_T("lrm;"), _T("\xE2\x80\x8E")},
                                                 {_T("lsaquo;"), _T("‹")},
                                                 {_T("lsquo;"), _T("‘")},
                                                 {_T("lt;"), _T("<")},
                                                 {_T("macr;"), _T("¯")},
                                                 {_T("mdash;"), _T("—")},
                                                 {_T("micro;"), _T("µ")},
                                                 {_T("middot;"), _T("·")},
                                                 {_T("minus;"), _T("−")},
                                                 {_T("mu;"), _T("μ")},
                                                 {_T("nabla;"), _T("∇")},
                                                 {_T("nbsp;"), _T("\xC2\xA0")},
                                                 {_T("ndash;"), _T("–")},
                                                 {_T("ne;"), _T("≠")},
                                                 {_T("ni;"), _T("∋")},
                                                 {_T("not;"), _T("¬")},
                                                 {_T("notin;"), _T("∉")},
                                                 {_T("nsub;"), _T("⊄")},
                                                 {_T("ntilde;"), _T("ñ")},
                                                 {_T("nu;"), _T("ν")},
                                                 {_T("oacute;"), _T("ó")},
                                                 {_T("ocirc;"), _T("ô")},
                                                 {_T("oelig;"), _T("œ")},
                                                 {_T("ograve;"), _T("ò")},
                                                 {_T("oline;"), _T("‾")},
                                                 {_T("omega;"), _T("ω")},
                                                 {_T("omicron;"), _T("ο")},
                                                 {_T("oplus;"), _T("⊕")},
                                                 {_T("or;"), _T("∨")},
                                                 {_T("ordf;"), _T("ª")},
                                                 {_T("ordm;"), _T("º")},
                                                 {_T("oslash;"), _T("ø")},
                                                 {_T("otilde;"), _T("õ")},
                                                 {_T("otimes;"), _T("⊗")},
                                                 {_T("ouml;"), _T("ö")},
                                                 {_T("para;"), _T("¶")},
                                                 {_T("part;"), _T("∂")},
                                                 {_T("permil;"), _T("‰")},
                                                 {_T("perp;"), _T("⊥")},
                                                 {_T("phi;"), _T("φ")},
                                                 {_T("pi;"), _T("π")},
                                                 {_T("piv;"), _T("ϖ")},
                                                 {_T("plusmn;"), _T("±")},
                                                 {_T("pound;"), _T("£")},
                                                 {_T("prime;"), _T("′")},
                                                 {_T("prod;"), _T("∏")},
                                                 {_T("prop;"), _T("∝")},
                                                 {_T("psi;"), _T("ψ")},
                                                 {_T("quot;"), _T("\"")},
                                                 {_T("rArr;"), _T("⇒")},
                                                 {_T("radic;"), _T("√")},
                                                 {_T("rang;"), _T("〉")},
                                                 {_T("raquo;"), _T("»")},
                                                 {_T("rarr;"), _T("→")},
                                                 {_T("rceil;"), _T("⌉")},
                                                 {_T("rdquo;"), _T("”")},
                                                 {_T("real;"), _T("ℜ")},
                                                 {_T("reg;"), _T("®")},
                                                 {_T("rfloor;"), _T("⌋")},
                                                 {_T("rho;"), _T("ρ")},
                                                 {_T("rlm;"), _T("\xE2\x80\x8F")},
                                                 {_T("rsaquo;"), _T("›")},
                                                 {_T("rsquo;"), _T("’")},
                                                 {_T("sbquo;"), _T("‚")},
                                                 {_T("scaron;"), _T("š")},
                                                 {_T("sdot;"), _T("⋅")},
                                                 {_T("sect;"), _T("§")},
                                                 {_T("shy;"), _T("\xC2\xAD")},
                                                 {_T("sigma;"), _T("σ")},
                                                 {_T("sigmaf;"), _T("ς")},
                                                 {_T("sim;"), _T("∼")},
                                                 {_T("spades;"), _T("♠")},
                                                 {_T("sub;"), _T("⊂")},
                                                 {_T("sube;"), _T("⊆")},
                                                 {_T("sum;"), _T("∑")},
                                                 {_T("sup1;"), _T("¹")},
                                                 {_T("sup2;"), _T("²")},
                                                 {_T("sup3;"), _T("³")},
                                                 {_T("sup;"), _T("⊃")},
                                                 {_T("supe;"), _T("⊇")},
                                                 {_T("szlig;"), _T("ß")},
                                                 {_T("tau;"), _T("τ")},
                                                 {_T("there4;"), _T("∴")},
                                                 {_T("theta;"), _T("θ")},
                                                 {_T("thetasym;"), _T("ϑ")},
                                                 {_T("thinsp;"), _T("\xE2\x80\x89")},
                                                 {_T("thorn;"), _T("þ")},
                                                 {_T("tilde;"), _T("˜")},
                                                 {_T("times;"), _T("×")},
                                                 {_T("trade;"), _T("™")},
                                                 {_T("uArr;"), _T("⇑")},
                                                 {_T("uacute;"), _T("ú")},
                                                 {_T("uarr;"), _T("↑")},
                                                 {_T("ucirc;"), _T("û")},
                                                 {_T("ugrave;"), _T("ù")},
                                                 {_T("uml;"), _T("¨")},
                                                 {_T("upsih;"), _T("ϒ")},
                                                 {_T("upsilon;"), _T("υ")},
                                                 {_T("uuml;"), _T("ü")},
                                                 {_T("weierp;"), _T("℘")},
                                                 {_T("xi;"), _T("ξ")},
                                                 {_T("yacute;"), _T("ý")},
                                                 {_T("yen;"), _T("¥")},
                                                 {_T("yuml;"), _T("ÿ")},
                                                 {_T("zeta;"), _T("ζ")},
                                                 {_T("zwj;"), _T("\xE2\x80\x8D")},
                                                 {_T("zwnj;"), _T("\xE2\x80\x8C")}};

static int cmp(const void *key, const void *value)
{
    return wcsncmp((const TCHAR *)key, *(const TCHAR *const *)value,
                   std::wcslen(*(const TCHAR *const *)value));
}

static const TCHAR *get_named_entity(const TCHAR *name)
{
    const TCHAR *const *entity = (const TCHAR *const *)bsearch(
        name, NAMED_ENTITIES, sizeof NAMED_ENTITIES / sizeof *NAMED_ENTITIES,
        sizeof *NAMED_ENTITIES, cmp);

    return entity ? entity[1] : NULL;
}

static size_t putc_utf8(unsigned long cp, char *buffer)
{
    char *bytes = (char *)buffer;

    if (cp <= 0x007Ful) {
        bytes[0] = (char)cp;
        return 1;
    }

    if (cp <= 0x07FFul) {
        bytes[1] = (char)((2 << 6) | (cp & 0x3F));
        bytes[0] = (char)((6 << 5) | (cp >> 6));
        return 2;
    }

    if (cp <= 0xFFFFul) {
        bytes[2] = (char)((2 << 6) | (cp & 0x3F));
        bytes[1] = (char)((2 << 6) | ((cp >> 6) & 0x3F));
        bytes[0] = (char)((14 << 4) | (cp >> 12));
        return 3;
    }

    if (cp <= 0x10FFFFul) {
        bytes[3] = (char)((2 << 6) | (cp & 0x3F));
        bytes[2] = (char)((2 << 6) | ((cp >> 6) & 0x3F));
        bytes[1] = (char)((2 << 6) | ((cp >> 12) & 0x3F));
        bytes[0] = (char)((30 << 3) | (cp >> 18));
        return 4;
    }

    return 0;
}

static bool parse_entity(const TCHAR *current, TCHAR **to, const TCHAR **from)
{
    // TODO: Remake this by using C++.
    const TCHAR *end = wcschr(current, ';');
    if (!end)
        return 0;

    if (current[1] == '#') {
        TCHAR *tail       = NULL;
        int    errno_save = errno;
        bool   hex        = current[2] == 'x' || current[2] == 'X';

        errno            = 0;
        unsigned long cp = wcstoul(current + (hex ? 3 : 2), &tail, hex ? 16 : 10);

        bool fail = errno || tail != end || cp > UNICODE_MAX;
        errno     = errno_save;
        if (fail)
            return 0;

#ifdef _WINDOWS
#ifndef _UNICODE
        *to += putc_utf8(cp, *to);
#else
        // TODO:
        char utf8char[5];
        // Add terminating zeros.
        memset(utf8char, 0, sizeof(utf8char));
        putc_utf8(cp, utf8char);
        wchar_t wchar[2];
        MultiByteToWideChar(CP_UTF8, 0, utf8char, -1, wchar, 2);
        **to = wchar[0];
        *to += 1;
#endif
#else
        char utf8char[5];
        // Add terminating zeros.
        memset(utf8char, 0, sizeof(utf8char));
        putc_utf8(cp, utf8char);
        wchar_t wchar[2];
        std::mbstowcs(wchar, utf8char, 1);
        **to = wchar[0];
        *to += 1;
#endif
        *from = end + 1;
        return 1;
    } else {
        const TCHAR *entity = get_named_entity(&current[1]);
        if (!entity)
            return 0;

        size_t len = std::wcslen(entity);
        memcpy(*to, entity, len);

        *to += len;
        *from = end + 1;

        return 1;
    }
}

size_t decode_html_entities_utf8(TCHAR *dest, const TCHAR *src)
{
    if (!src)
        src = dest;

    TCHAR       *to   = dest;
    const TCHAR *from = src;

    for (const TCHAR *current; (current = wcschr(from, _T('&')));) {
        memmove(to, from, (size_t)(current - from) * sizeof(TCHAR));
        to += current - from;

        if (parse_entity(current, &to, &from))
            continue;

        from  = current;
        *to++ = *from++;
    }

    size_t remaining = std::wcslen(from);

    memmove(to, from, remaining * sizeof(TCHAR));
    to += remaining;
    *to = 0;

    return (size_t)(to - dest) * sizeof(TCHAR);
}

// Search % and insert corresponding characters,
// '+' are replaced by spaces.
void decode_url(TCHAR *dest, TCHAR *src, unsigned char delete_spaces)
{
    size_t query_len = std::wcslen(src);
    TCHAR *ptr;
    ptr     = src;
    dest[0] = 0;

    // When the browser sends a request in response to a button press,
    // it replaces spaces with '+', and '+' with '%' followed by the code
    // (=> first you need to convert '+' to ' ', and then '%' to characters);
    // when the browser follows a link, it replaces spaces with '%'
    // followed by the code, and '+' remains as they were
    // (=> only need to convert '%' to characters).

    // '+' replaced by spaces. 'Src' is modified!
    if (delete_spaces) {
        while ((ptr = (TCHAR *)wcschr(ptr, '+'))) {
            *ptr = ' ';
            ptr++;
        }
    }

    ptr           = src;
    std::string s = "";
    // Character conversion specified by codes.
    while ((size_t)(ptr - src) <= query_len) {
        char chr;

        if (*ptr == '%') {
            chr = (char)((hex_char_to_int(*(ptr + 1)) << 4) + hex_char_to_int(*(ptr + 2)));

            ptr += 3;
            s += chr;
        } else {
            s += (char)*ptr;
            ptr++;
        }
    }

    // Convert std::string (UTF-8) to std::wstring.
    wchar_t wstr[MAX_LINE_LENGTH];
#ifdef _WINDOWS
    int ret = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wstr, MAX_LINE_LENGTH);

    if (ret != 0)
        // Concatenate converted wide string to another wide string.
        wcscat(dest, wstr);
    else
        // Handle conversion error.
        system_error(L"Error in MultiByteToWideChar.");
#else
    const char *s_cstr = s.c_str();    // s is the std::string to convert.
    mbstate_t   state  = mbstate_t();  // Initial conversion state.
    size_t      ret    = std::mbsrtowcs(wstr, &s_cstr, MAX_LINE_LENGTH, &state);

    if (ret != (size_t)-1)
        // Concatenate converted wide string to another wide string.
        wcscat(dest, wstr);
    else
        // Handle conversion error.
        system_error(L"Error in std::mbsrtowcs.");
#endif
}
//-------------------------------------------------------------------------------------------------

// Decode Unicode escape sequences like \u0026 to actual characters
void decode_unicode_escapes(TCHAR *dest, const TCHAR *src)
{
    if (!src || !dest) return;
    
    const TCHAR *input = src;
    TCHAR *output = dest;
    
    while (*input) {
        if (*input == L'\\' && *(input + 1) == L'u' && 
            input + 5 < src + std::wcslen(src)) {
            
            // Check if we have 4 hex digits
            bool valid = true;
            for (int i = 2; i < 6; i++) {
                TCHAR c = input[i];
                if (!((c >= L'0' && c <= L'9') || 
                      (c >= L'A' && c <= L'F') || 
                      (c >= L'a' && c <= L'f'))) {
                    valid = false;
                    break;
                }
            }
            
            if (valid) {
                // Parse the 4 hex digits
                wchar_t hexStr[5] = {input[2], input[3], input[4], input[5], L'\0'};
                wchar_t codePoint = static_cast<wchar_t>(std::wcstol(hexStr, nullptr, 16));
                *output++ = codePoint;
                input += 6; // Skip \uXXXX
                continue;
            }
        }
        
        // Copy character as-is
        *output++ = *input++;
    }
    
    *output = L'\0';
}
//-------------------------------------------------------------------------------------------------

#ifdef _WINDOWS
// Convert a wide Unicode string to an UTF-8 string.
std::string narrow(const std::wstring &wide_string)
{
    int requiredSize =
        WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (requiredSize == 0) {
        system_error(L"Error calculating wide string length.");
        return "";
    }

    std::string utf8_string(requiredSize, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, &utf8_string[0], requiredSize, nullptr,
                        nullptr);

    return utf8_string;
}
#else
std::string narrow(const std::wstring &wide_string)
{
    // Create a wstring_convert object with the UTF-8 locale.
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // Use the wstring_convert object to perform the conversion.
    return converter.to_bytes(wide_string);
}
#endif
//-------------------------------------------------------------------------------------------------

#ifdef _WINDOWS
std::wstring widen(const std::string &narrow_str, unsigned int code_page)
{
    // Calculate the length of the wide string buffer needed.
    int wide_string_length = MultiByteToWideChar(code_page, 0, narrow_str.c_str(), -1, nullptr, 0);
    if (wide_string_length == 0) {
        system_error(L"Error calculating wide string length.");
        return L"";
    }

    // Allocate memory for the wide string buffer.
    std::wstring wide_string;
    wide_string.resize(wide_string_length);

    // Convert the multibyte string to a wide string.
    if (MultiByteToWideChar(code_page, 0, narrow_str.c_str(), -1, &wide_string[0],
                            wide_string_length) == 0) {
        system_error(L"Error converting multibyte string to wide string.");
        return L"";
    }

    return wide_string;
}
#else
std::wstring widen(const std::string &narrow_str, const std::string &code_page)
{
    std::string effective_codepage = code_page;
    // Workaround for common codepage naming issues.
    if (code_page == "ISO-8859-1") {
        effective_codepage = "ISO_8859-1";  // Adjust the codepage name if necessary.
    }

    iconv_t cd = iconv_open("WCHAR_T", effective_codepage.c_str());
    if (cd == (iconv_t)-1) {
        // Handle error: iconv_open failed.
        return std::wstring();
    }

    size_t               inBytesLeft  = narrow_str.size();
    const char          *inBuf        = narrow_str.c_str();
    size_t               outBytesLeft = narrow_str.size() * sizeof(wchar_t);
    std::vector<wchar_t> wide_str(outBytesLeft / sizeof(wchar_t), 0);
    char                *outBuf = reinterpret_cast<char *>(wide_str.data());

    while (inBytesLeft > 0) {
        size_t result =
            iconv(cd, const_cast<char **>(&inBuf), &inBytesLeft, &outBuf, &outBytesLeft);
        if (result == (size_t)-1) {
            if (errno == E2BIG) {
                // Output buffer is too small; this should not happen in this setup.
                break;
            } else {
                // Handle other errors (e.g., EILSEQ, EINVAL).
                iconv_close(cd);
                return std::wstring();
            }
        }
    }

    iconv_close(cd);
    size_t wideLength = wide_str.size() - (outBytesLeft / sizeof(wchar_t));
    return std::wstring(wide_str.begin(), wide_str.begin() + wideLength);
}
#endif
//-------------------------------------------------------------------------------------------------

#ifndef _WINDOWS
char *malloc_narrow(const std::wstring &wide_string)
{
    // Create a locale object with UTF-8 encoding.
    std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>());

    // Create a wstring_convert object with the UTF-8 locale.
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // Use the wstring_convert object to perform the conversion.
    std::string narrow_string = converter.to_bytes(wide_string);

    // Allocate memory for the narrow string on the heap using malloc.
    char *narrow_cstr =
        static_cast<char *>(std::malloc((narrow_string.length() + 1) * sizeof(char)));

    if (narrow_cstr != nullptr) {
        // Copy the narrow string into the allocated memory.
        strcpy(narrow_cstr, narrow_string.c_str());
    } else {
        // TODO: Handle allocation failure.
        // system_error(L"Failed to allocate memory for narrow string");
    }

    // Return the pointer to the dynamically allocated char buffer.
    return narrow_cstr;
}
//-------------------------------------------------------------------------------------------------
#endif

bool directory_exists(const std::wstring &path) { return std::filesystem::is_directory(path); }
//-------------------------------------------------------------------------------------------------

std::wstring get_directory_name(const std::wstring &file_path)
{
    std::filesystem::path path(file_path);
    return path.parent_path().wstring();
}
//-------------------------------------------------------------------------------------------------

std::wstring get_wide_environment_variable(const std::wstring &name)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Getting environment variable: " << narrow(name));
    
#ifdef _WINDOWS
    // For Windows, use _wgetenv to retrieve the value of the environment variable as a wide string.
    const wchar_t *value = _wgetenv(name.c_str());
    if (value) {
        DEBUG_LOG("Environment variable found: " << narrow(std::wstring(value)));
        return value;
    }
    DEBUG_LOG("Environment variable not found in system environment");
#else
    // Convert the wide-character name to narrow string.
    std::string narrow_name = narrow(name);
    // Use std::getenv to retrieve the value of the environment variable as a narrow string.
    const char *value = std::getenv(narrow_name.c_str());
    if (value) {
        std::wstring result = widen(value);
        DEBUG_LOG("Environment variable found: " << narrow(result));
        return result;
    }
    DEBUG_LOG("Environment variable not found in system environment");
#endif

    DEBUG_LOG("Checking config for environment variable");
    std::unique_ptr<Bookmarks::Config> config(Bookmarks::ConfigFactory::GetConfig());
    std::wstring result = config->GetValue(name);
    DEBUG_LOG("Config result: " << narrow(result));
    DEBUG_LOG_FUNC_EXIT();
    return result;
}
//-------------------------------------------------------------------------------------------------

// TODO: Move Linux-specific code to a separate file.
#ifndef _WINDOWS
std::string sha3_256(const std::string &input)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX   *context = EVP_MD_CTX_new();

    if (context == nullptr) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    if (EVP_DigestInit_ex(context, EVP_sha3_256(), nullptr) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to initialize SHA-3 context");
    }

    if (EVP_DigestUpdate(context, input.c_str(), input.size()) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to update SHA-3 hash");
    }

    if (EVP_DigestFinal_ex(context, hash, nullptr) != 1) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to finalize SHA-3 hash");
    }

    EVP_MD_CTX_free(context);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}
#endif
//-------------------------------------------------------------------------------------------------

std::wstring get_file_name(const std::wstring &content)
{
#ifdef _WINDOWS
    (void)content;
    // TODO: For Windows, use the GetTempFileName function to generate a unique temporary file
    // name. Or use OpenSSL to generate a SHA-3 hash of the content.
    TCHAR timestamp[9];
    std::swprintf(timestamp, sizeof(timestamp) / sizeof(*timestamp), _T("%08x"), (int)time(0));

    return timestamp;
#else
    return widen(sha3_256(narrow(content)));
#endif
}
//-------------------------------------------------------------------------------------------------

// Creating URL file with the specified link name and URL.
// Extensions must include the dot.
const std::wstring ext_utf8   = _T(".utf8");
const std::wstring ext_url    = _T(".url");
const std::wstring param_url  = _T("URL");
const std::wstring param_name = _T("Name");
#ifdef EXTENDED_URL_FILE
const TCHAR *url_file_content_template = _T("\n\
[InternetShortcut]\n\
URL=%ls\n\
Name=%ls\n\
");
#else
const TCHAR *url_file_content_template = _T("\n\
[InternetShortcut]\n\
URL=%ls\n\
");
#endif

std::wstring get_link_file_content(const std::wstring &link_name, const std::wstring &url)
{
    // TODO: Remake this by using C++ classes, std::format is not
    // supported yet even by C++20 at the moment on Linux.
    TCHAR url_file_content[1000];
    swprintf(url_file_content, sizeof(url_file_content) / sizeof(TCHAR), url_file_content_template,
             url.c_str(), link_name.c_str());
    return url_file_content;
}
//-------------------------------------------------------------------------------------------------

std::wstring create_url_file(const TCHAR *link_name, const TCHAR *url, const TCHAR *link_directory)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Creating URL file for link: " << narrow(std::wstring(link_name)));
    DEBUG_LOG("URL: " << narrow(std::wstring(url)));
    if (link_directory) {
        DEBUG_LOG("Directory: " << narrow(std::wstring(link_directory)));
    } else {
        DEBUG_LOG("Directory: (current directory)");
    }
    
    const std::wstring url_file_content = get_link_file_content(link_name, url);

#ifdef EXTENDED_URL_FILE
    std::wstring file_path;
    // An extension is not added to the file name assuming that it is in UTF-8.
    std::wstring file_name = get_file_name(url_file_content);

    if (link_directory)
        file_path = concatenate_paths(std::wstring(link_directory), file_name);
    else
        file_path = file_name;

#ifdef _WINDOWS
    FILE *f = _wfopen(file_path.c_str(), _T("w, ccs=UTF-8"));
    if (f) {
        fwprintf(f, url_file_content);
        fclose(f);
    }
    return file_path;
#else
    std::wofstream file(narrow(file_path));
    if (file.is_open()) {
        // Ensure UTF-8 encoding to output Cyrillic characters correctly.
        file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>));
        file << url_file_content;
        file.close();
    }
    return file_path;
#endif
#else
    // TODO: Remove or upgrade this part.
    TCHAR *full_file_name = malloc((std::wcslen(link_name) + std::wcslen(ext) + 1) * sizeof(TCHAR));
    if (full_file_name) {
        wcscpy(full_file_name, link_name);
        wcscat(full_file_name, ext);
        {
#ifdef _WINDOWS
            FILE *f = _wfopen(full_file_name, _T("w"));
            if (f) {
                fwprintf(f, url_file_content_template, url);
                fclose(f);
            }
#else
            std::wofstream file(narrow(file_name));
            if (file.is_open()) {
                // Ensure UTF-8 encoding to output Cyrillic characters correctly.
                file.imbue(std::locale(file.getloc(),
                                       new std::codecvt_utf8<wchar_t>));  // Ensure UTF-8 encoding.
                file << url_file_content;
                file.close();
            }
#endif
        }
        free(full_file_name);
    }
#endif
}
//-------------------------------------------------------------------------------------------------

#ifdef _WINDOWS
std::string utf8_to_cp1251(const std::string &utf8)
{
    int wide_string_length = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (wide_string_length == 0) {
        system_error(L"Error converting multibyte string to wide string.");
        return "";
    }

    std::wstring wide_string(wide_string_length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wide_string[0], wide_string_length);

    wide_string_length =
        WideCharToMultiByte(1251, 0, wide_string.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (wide_string_length == 0) {
        system_error(L"Error converting wide string to multibyte string.");
        return "";
    }

    std::string cp1251str(wide_string_length, '\0');
    WideCharToMultiByte(1251, 0, wide_string.c_str(), -1, &cp1251str[0], wide_string_length,
                        nullptr, nullptr);

    return cp1251str;
}
#endif
//-------------------------------------------------------------------------------------------------

int hex_char_to_int(TCHAR c)
{
    switch (c) {
    default:
    case ('0'):
        return 0;
    case ('1'):
        return 1;
    case ('2'):
        return 2;
    case ('3'):
        return 3;
    case ('4'):
        return 4;
    case ('5'):
        return 5;
    case ('6'):
        return 6;
    case ('7'):
        return 7;
    case ('8'):
        return 8;
    case ('9'):
        return 9;
    case ('a'):
    case ('A'):
        return 10;
    case ('b'):
    case ('B'):
        return 11;
    case ('c'):
    case ('C'):
        return 12;
    case ('d'):
    case ('D'):
        return 13;
    case ('e'):
    case ('E'):
        return 14;
    case ('f'):
    case ('F'):
        return 15;
    }
}
//-------------------------------------------------------------------------------------------------

const wchar_t *create_new_wide_string(const char *c)
{
    const size_t length = strlen(c) + 1;
    wchar_t     *wc     = new wchar_t[length];
    mbstowcs(wc, c, length);

    return wc;
}
//-------------------------------------------------------------------------------------------------

std::wstring get_current_working_directory()
{
    // Get the current working directory as a wide-character string.
    return std::filesystem::current_path().wstring();
}
//-------------------------------------------------------------------------------------------------
// TODO: use exception instead of boolean.
bool change_working_directory(const std::wstring &wide_path)
{
    DEBUG_LOG_FUNC();
    DEBUG_LOG("Changing working directory to: " << narrow(wide_path));
    
    try {
        // Set the current working directory to the wide-character path.
        std::filesystem::current_path(wide_path);
        DEBUG_LOG("Successfully changed working directory");
        DEBUG_LOG_FUNC_EXIT();
        return true;  // Success
    } catch (const std::filesystem::filesystem_error &e) {
        DEBUG_LOG("Failed to change working directory: " << e.what());
        system_error(L"Failed to change working directory to " + wide_path);
        DEBUG_LOG_FUNC_EXIT();
        return false;  // Error
    }
}
//-------------------------------------------------------------------------------------------------

std::wstring concatenate_paths(const std::wstring &part1, const std::wstring &part2)
{
    if (part1.empty())
        return part2;
    if (part2.empty())
        return part1;

    // Check if the first part already ends with the path delimiter.
    if (part1.back() == PATH_DELIMITER && part2.front() != PATH_DELIMITER) {
        return part1 + part2;
    } else if (part1.back() != PATH_DELIMITER && part2.front() == PATH_DELIMITER) {
        return part1 + part2;
    } else
        // If the second part starts with the path delimiter, remove it.
        if (part1.back() == PATH_DELIMITER && part2.front() == PATH_DELIMITER) {
            return part1 + part2.substr(1);
        } else {
            return part1 + PATH_DELIMITER + part2;
        }
}
//-------------------------------------------------------------------------------------------------

std::wstring extract_domain(const std::wstring &url)
{
    // Regular expression to match the domain name in a URL.
    std::wregex  domainRegex(LR"((http[s]?://)?([^/]+))");
    std::wsmatch match;

    if (std::regex_search(url, match, domainRegex) && match.size() > 2) {
        return match.str(2);  // The domain name is in the second capture group.
    }

    return L"";  // Return an empty string if no match is found.
}
