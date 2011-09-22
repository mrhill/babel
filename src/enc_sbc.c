#include "str.h"
#include "mem.h"

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
