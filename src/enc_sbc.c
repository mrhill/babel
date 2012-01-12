#include "str.h"
#include "mem.h"

#ifdef _MSC_VER
#include <ctype.h>
static int strncasecmp(const char* s1, const char* s2, size_t n)
{
  const unsigned char *ucs1 = (const unsigned char *) s1;
  const unsigned char *ucs2 = (const unsigned char *) s2;
  int d = 0;
  for ( ; n != 0; n--)
    {
      const int c1 = tolower(*ucs1++);
      const int c2 = tolower(*ucs2++);
      if (((d = c1 - c2) != 0) || (c2 == '\0'))
        break;
    }
  return d;
}

char * strcasestr(s, find) const char *s, *find;
{
	char c, sc;
	size_t len;

	if ((c = *find++) != 0) {
		c = tolower((unsigned char)c);
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while ((char)tolower((unsigned char)sc) != c);
		} while (strncasecmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}
#endif

#if defined(bbNO_STRCMP)
int bbStrcmp_sbc(const bbCHAR8* pStr1, const bbCHAR8* pStr2)
{
    for(;;)
    {
        register const unsigned char c1 = (unsigned char) *(pStr1++);
        register const unsigned char c2 = (unsigned char) *(pStr2++);

        if (c1<c2) return -1;
        if (c1>c2) return 1;
        if (c1==0) break;
    }

    return 0;
}
#endif /* bbNO_STRCMP */

#if defined(bbNO_STRICMP)
int bbStricmp_sbc(const bbCHAR8* pStr1, const bbCHAR8* pStr2)
{
    for(;;)
    {
        register const unsigned char c1 = (unsigned char) *(pStr1++) & 0xDF; /* XXX quick hack */
        register const unsigned char c2 = (unsigned char) *(pStr2++) & 0xDF;

        if (c1<c2) return -1;
        if (c1>c2) return 1;
        if (c1==0) break;
    }

    return 0;
}
#endif /* bbNO_STRICMP */


#if defined(bbNO_SPRINTF)
int bbSprintf_sbc(bbCHAR8* pbuf, const bbCHAR8* fmt, ...)
{
#if FIXME == 0
#endif
    return 0;
}
#endif  /* bbNO_SPRINTF */
