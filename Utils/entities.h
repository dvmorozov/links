/*	Copyright 2012 Christoph Gärtner
	Distributed under the Boost Software License, Version 1.0
*/

#ifndef DECODE_HTML_ENTITIES_UTF8_
#define DECODE_HTML_ENTITIES_UTF8_

#include <stddef.h>
#include "../main.h"

size_t decode_html_entities_utf8(TCHAR *dest, const TCHAR *src);
/*	Takes input from <src> and decodes into <dest>, which should be a buffer
	large enough to hold <strlen(src) + 1> characters.

	If <src> is <NULL>, input will be taken from <dest>, decoding
	the entities in-place.

	The function returns the length of the decoded string.
*/
void decode_url(TCHAR *dest, TCHAR *src, unsigned char delete_spaces);

//  Working with URL-files.
extern const std::wstring ExtUtf8;
extern const std::wstring ParamURL;
extern const std::wstring ParamName;

std::wstring create_url_file(const TCHAR *name, const TCHAR *url, const TCHAR *folderName = nullptr);

#endif
