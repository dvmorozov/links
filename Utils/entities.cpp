/*	Copyright 2012, 2016 Christoph Gärtner
	Distributed under the Boost Software License, Version 1.0
*/

#include "entities.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define UNICODE_MAX 0x10FFFFul

static const TCHAR *const NAMED_ENTITIES[][2] = {
	{ _T("AElig;"), _T("Æ") },
	{ _T("Aacute;"), _T("Á") },
	{ _T("Acirc;"), _T("Â") },
	{ _T("Agrave;"), _T("À") },
	{ _T("Alpha;"), _T("Α") },
	{ _T("Aring;"), _T("Å") },
	{ _T("Atilde;"), _T("Ã") },
	{ _T("Auml;"), _T("Ä") },
	{ _T("Beta;"), _T("Β") },
	{ _T("Ccedil;"), _T("Ç") },
	{ _T("Chi;"), _T("Χ") },
	{ _T("Dagger;"), _T("‡") },
	{ _T("Delta;"), _T("Δ") },
	{ _T("ETH;"), _T("Ð") },
	{ _T("Eacute;"), _T("É") },
	{ _T("Ecirc;"), _T("Ê") },
	{ _T("Egrave;"), _T("È") },
	{ _T("Epsilon;"), _T("Ε") },
	{ _T("Eta;"), _T("Η") },
	{ _T("Euml;"), _T("Ë") },
	{ _T("Gamma;"), _T("Γ") },
	{ _T("Iacute;"), _T("Í") },
	{ _T("Icirc;"), _T("Î") },
	{ _T("Igrave;"), _T("Ì") },
	{ _T("Iota;"), _T("Ι") },
	{ _T("Iuml;"), _T("Ï") },
	{ _T("Kappa;"), _T("Κ") },
	{ _T("Lambda;"), _T("Λ") },
	{ _T("Mu;"), _T("Μ") },
	{ _T("Ntilde;"), _T("Ñ") },
	{ _T("Nu;"), _T("Ν") },
	{ _T("OElig;"), _T("Œ") },
	{ _T("Oacute;"), _T("Ó") },
	{ _T("Ocirc;"), _T("Ô") },
	{ _T("Ograve;"), _T("Ò") },
	{ _T("Omega;"), _T("Ω") },
	{ _T("Omicron;"), _T("Ο") },
	{ _T("Oslash;"), _T("Ø") },
	{ _T("Otilde;"), _T("Õ") },
	{ _T("Ouml;"), _T("Ö") },
	{ _T("Phi;"), _T("Φ") },
	{ _T("Pi;"), _T("Π") },
	{ _T("Prime;"), _T("″") },
	{ _T("Psi;"), _T("Ψ") },
	{ _T("Rho;"), _T("Ρ") },
	{ _T("Scaron;"), _T("Š") },
	{ _T("Sigma;"), _T("Σ") },
	{ _T("THORN;"), _T("Þ") },
	{ _T("Tau;"), _T("Τ") },
	{ _T("Theta;"), _T("Θ") },
	{ _T("Uacute;"), _T("Ú") },
	{ _T("Ucirc;"), _T("Û") },
	{ _T("Ugrave;"), _T("Ù") },
	{ _T("Upsilon;"), _T("Υ") },
	{ _T("Uuml;"), _T("Ü") },
	{ _T("Xi;"), _T("Ξ") },
	{ _T("Yacute;"), _T("Ý") },
	{ _T("Yuml;"), _T("Ÿ") },
	{ _T("Zeta;"), _T("Ζ") },
	{ _T("aacute;"), _T("á") },
	{ _T("acirc;"), _T("â") },
	{ _T("acute;"), _T("´") },
	{ _T("aelig;"), _T("æ") },
	{ _T("agrave;"), _T("à") },
	{ _T("alefsym;"), _T("ℵ") },
	{ _T("alpha;"), _T("α") },
	{ _T("amp;"), _T("&") },
	{ _T("and;"), _T("∧") },
	{ _T("ang;"), _T("∠") },
	{ _T("apos;"), _T("'") },
	{ _T("aring;"), _T("å") },
	{ _T("asymp;"), _T("≈") },
	{ _T("atilde;"), _T("ã") },
	{ _T("auml;"), _T("ä") },
	{ _T("bdquo;"), _T("„") },
	{ _T("beta;"), _T("β") },
	{ _T("brvbar;"), _T("¦") },
	{ _T("bull;"), _T("•") },
	{ _T("cap;"), _T("∩") },
	{ _T("ccedil;"), _T("ç") },
	{ _T("cedil;"), _T("¸") },
	{ _T("cent;"), _T("¢") },
	{ _T("chi;"), _T("χ") },
	{ _T("circ;"), _T("ˆ") },
	{ _T("clubs;"), _T("♣") },
	{ _T("cong;"), _T("≅") },
	{ _T("copy;"), _T("©") },
	{ _T("crarr;"), _T("↵") },
	{ _T("cup;"), _T("∪") },
	{ _T("curren;"), _T("¤") },
	{ _T("dArr;"), _T("⇓") },
	{ _T("dagger;"), _T("†") },
	{ _T("darr;"), _T("↓") },
	{ _T("deg;"), _T("°") },
	{ _T("delta;"), _T("δ") },
	{ _T("diams;"), _T("♦") },
	{ _T("divide;"), _T("÷") },
	{ _T("eacute;"), _T("é") },
	{ _T("ecirc;"), _T("ê") },
	{ _T("egrave;"), _T("è") },
	{ _T("empty;"), _T("∅") },
	{ _T("emsp;"), _T("\xE2\x80\x83") },
	{ _T("ensp;"), _T("\xE2\x80\x82") },
	{ _T("epsilon;"), _T("ε") },
	{ _T("equiv;"), _T("≡") },
	{ _T("eta;"), _T("η") },
	{ _T("eth;"), _T("ð") },
	{ _T("euml;"), _T("ë") },
	{ _T("euro;"), _T("€") },
	{ _T("exist;"), _T("∃") },
	{ _T("fnof;"), _T("ƒ") },
	{ _T("forall;"), _T("∀") },
	{ _T("frac12;"), _T("½") },
	{ _T("frac14;"), _T("¼") },
	{ _T("frac34;"), _T("¾") },
	{ _T("frasl;"), _T("⁄") },
	{ _T("gamma;"), _T("γ") },
	{ _T("ge;"), _T("≥") },
	{ _T("gt;"), _T(">") },
	{ _T("hArr;"), _T("⇔") },
	{ _T("harr;"), _T("↔") },
	{ _T("hearts;"), _T("♥") },
	{ _T("hellip;"), _T("…") },
	{ _T("iacute;"), _T("í") },
	{ _T("icirc;"), _T("î") },
	{ _T("iexcl;"), _T("¡") },
	{ _T("igrave;"), _T("ì") },
	{ _T("image;"), _T("ℑ") },
	{ _T("infin;"), _T("∞") },
	{ _T("int;"), _T("∫") },
	{ _T("iota;"), _T("ι") },
	{ _T("iquest;"), _T("¿") },
	{ _T("isin;"), _T("∈") },
	{ _T("iuml;"), _T("ï") },
	{ _T("kappa;"), _T("κ") },
	{ _T("lArr;"), _T("⇐") },
	{ _T("lambda;"), _T("λ") },
	{ _T("lang;"), _T("〈") },
	{ _T("laquo;"), _T("«") },
	{ _T("larr;"), _T("←") },
	{ _T("lceil;"), _T("⌈") },
	{ _T("ldquo;"), _T("“") },
	{ _T("le;"), _T("≤") },
	{ _T("lfloor;"), _T("⌊") },
	{ _T("lowast;"), _T("∗") },
	{ _T("loz;"), _T("◊") },
	{ _T("lrm;"), _T("\xE2\x80\x8E") },
	{ _T("lsaquo;"), _T("‹") },
	{ _T("lsquo;"), _T("‘") },
	{ _T("lt;"), _T("<") },
	{ _T("macr;"), _T("¯") },
	{ _T("mdash;"), _T("—") },
	{ _T("micro;"), _T("µ") },
	{ _T("middot;"), _T("·") },
	{ _T("minus;"), _T("−") },
	{ _T("mu;"), _T("μ") },
	{ _T("nabla;"), _T("∇") },
	{ _T("nbsp;"), _T("\xC2\xA0") },
	{ _T("ndash;"), _T("–") },
	{ _T("ne;"), _T("≠") },
	{ _T("ni;"), _T("∋") },
	{ _T("not;"), _T("¬") },
	{ _T("notin;"), _T("∉") },
	{ _T("nsub;"), _T("⊄") },
	{ _T("ntilde;"), _T("ñ") },
	{ _T("nu;"), _T("ν") },
	{ _T("oacute;"), _T("ó") },
	{ _T("ocirc;"), _T("ô") },
	{ _T("oelig;"), _T("œ") },
	{ _T("ograve;"), _T("ò") },
	{ _T("oline;"), _T("‾") },
	{ _T("omega;"), _T("ω") },
	{ _T("omicron;"), _T("ο") },
	{ _T("oplus;"), _T("⊕") },
	{ _T("or;"), _T("∨") },
	{ _T("ordf;"), _T("ª") },
	{ _T("ordm;"), _T("º") },
	{ _T("oslash;"), _T("ø") },
	{ _T("otilde;"), _T("õ") },
	{ _T("otimes;"), _T("⊗") },
	{ _T("ouml;"), _T("ö") },
	{ _T("para;"), _T("¶") },
	{ _T("part;"), _T("∂") },
	{ _T("permil;"), _T("‰") },
	{ _T("perp;"), _T("⊥") },
	{ _T("phi;"), _T("φ") },
	{ _T("pi;"), _T("π") },
	{ _T("piv;"), _T("ϖ") },
	{ _T("plusmn;"), _T("±") },
	{ _T("pound;"), _T("£") },
	{ _T("prime;"), _T("′") },
	{ _T("prod;"), _T("∏") },
	{ _T("prop;"), _T("∝") },
	{ _T("psi;"), _T("ψ") },
	{ _T("quot;"), _T("\"") },
	{ _T("rArr;"), _T("⇒") },
	{ _T("radic;"), _T("√") },
	{ _T("rang;"), _T("〉") },
	{ _T("raquo;"), _T("»") },
	{ _T("rarr;"), _T("→") },
	{ _T("rceil;"), _T("⌉") },
	{ _T("rdquo;"), _T("”") },
	{ _T("real;"), _T("ℜ") },
	{ _T("reg;"), _T("®") },
	{ _T("rfloor;"), _T("⌋") },
	{ _T("rho;"), _T("ρ") },
	{ _T("rlm;"), _T("\xE2\x80\x8F") },
	{ _T("rsaquo;"), _T("›") },
	{ _T("rsquo;"), _T("’") },
	{ _T("sbquo;"), _T("‚") },
	{ _T("scaron;"), _T("š") },
	{ _T("sdot;"), _T("⋅") },
	{ _T("sect;"), _T("§") },
	{ _T("shy;"), _T("\xC2\xAD") },
	{ _T("sigma;"), _T("σ") },
	{ _T("sigmaf;"), _T("ς") },
	{ _T("sim;"), _T("∼") },
	{ _T("spades;"), _T("♠") },
	{ _T("sub;"), _T("⊂") },
	{ _T("sube;"), _T("⊆") },
	{ _T("sum;"), _T("∑") },
	{ _T("sup1;"), _T("¹") },
	{ _T("sup2;"), _T("²") },
	{ _T("sup3;"), _T("³") },
	{ _T("sup;"), _T("⊃") },
	{ _T("supe;"), _T("⊇") },
	{ _T("szlig;"), _T("ß") },
	{ _T("tau;"), _T("τ") },
	{ _T("there4;"), _T("∴") },
	{ _T("theta;"), _T("θ") },
	{ _T("thetasym;"), _T("ϑ") },
	{ _T("thinsp;"), _T("\xE2\x80\x89") },
	{ _T("thorn;"), _T("þ") },
	{ _T("tilde;"), _T("˜") },
	{ _T("times;"), _T("×") },
	{ _T("trade;"), _T("™") },
	{ _T("uArr;"), _T("⇑") },
	{ _T("uacute;"), _T("ú") },
	{ _T("uarr;"), _T("↑") },
	{ _T("ucirc;"), _T("û") },
	{ _T("ugrave;"), _T("ù") },
	{ _T("uml;"), _T("¨") },
	{ _T("upsih;"), _T("ϒ") },
	{ _T("upsilon;"), _T("υ") },
	{ _T("uuml;"), _T("ü") },
	{ _T("weierp;"), _T("℘") },
	{ _T("xi;"), _T("ξ") },
	{ _T("yacute;"), _T("ý") },
	{ _T("yen;"), _T("¥") },
	{ _T("yuml;"), _T("ÿ") },
	{ _T("zeta;"), _T("ζ") },
	{ _T("zwj;"), _T("\xE2\x80\x8D") },
	{ _T("zwnj;"), _T("\xE2\x80\x8C") }
};

static int cmp(const void *key, const void *value)
{
	return _tcsncmp((const TCHAR *)key, *(const TCHAR *const *)value,
		_tcslen(*(const TCHAR *const *)value));
}

static const TCHAR *get_named_entity(const TCHAR *name)
{
	const TCHAR *const *entity = (const TCHAR *const *)bsearch(name,
		NAMED_ENTITIES, sizeof NAMED_ENTITIES / sizeof *NAMED_ENTITIES,
		sizeof *NAMED_ENTITIES, cmp);

	return entity ? entity[1] : NULL;
}

static size_t putc_utf8(unsigned long cp, TCHAR *buffer)
{
	TCHAR *bytes = (TCHAR *)buffer;

	if(cp <= 0x007Ful)
	{
		bytes[0] = (TCHAR)cp;
		return 1;
	}

	if(cp <= 0x07FFul)
	{
		bytes[1] = (TCHAR)((2 << 6) | (cp & 0x3F));
		bytes[0] = (TCHAR)((6 << 5) | (cp >> 6));
		return 2;
	}

	if(cp <= 0xFFFFul)
	{
		bytes[2] = (TCHAR)(( 2 << 6) | ( cp       & 0x3F));
		bytes[1] = (TCHAR)(( 2 << 6) | ((cp >> 6) & 0x3F));
		bytes[0] = (TCHAR)((14 << 4) |  (cp >> 12));
		return 3;
	}

	if(cp <= 0x10FFFFul)
	{
		bytes[3] = (TCHAR)(( 2 << 6) | ( cp        & 0x3F));
		bytes[2] = (TCHAR)(( 2 << 6) | ((cp >>  6) & 0x3F));
		bytes[1] = (TCHAR)(( 2 << 6) | ((cp >> 12) & 0x3F));
		bytes[0] = (TCHAR)((30 << 3) |  (cp >> 18));
		return 4;
	}

	return 0;
}

static bool parse_entity(
	const TCHAR *current, TCHAR **to, const TCHAR **from)
{
	const TCHAR *end = _tcschr(current, ';');
	if(!end) return 0;

	if(current[1] == '#')
	{
		TCHAR *tail = NULL;
		int errno_save = errno;
		bool hex = current[2] == 'x' || current[2] == 'X';

		errno = 0;
		unsigned long cp = _tcstoul(
			current + (hex ? 3 : 2), &tail, hex ? 16 : 10);

		bool fail = errno || tail != end || cp > UNICODE_MAX;
		errno = errno_save;
		if(fail) return 0;

		*to += putc_utf8(cp, *to);
		*from = end + 1;

		return 1;
	}
	else
	{
		const TCHAR *entity = get_named_entity(&current[1]);
		if(!entity) return 0;

		size_t len = _tcslen(entity);
		memcpy(*to, entity, len);

		*to += len;
		*from = end + 1;

		return 1;
	}
}

size_t decode_html_entities_utf8(TCHAR *dest, const TCHAR *src)
{
	if(!src) src = dest;

	TCHAR *to = dest;
	const TCHAR *from = src;

	for(const TCHAR *current; (current = _tcschr(from, _T('&')));)
	{
		memmove(to, from, (size_t)(current - from));
		to += current - from;

		if(parse_entity(current, &to, &from))
			continue;

		from = current;
		*to++ = *from++;
	}

	size_t remaining = _tcslen(from);

	memmove(to, from, remaining);
	to += remaining;
	*to = 0;

	return (size_t)(to - dest);
}
