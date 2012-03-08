#ifndef bbBABEL_ENC_SBC_H_
#define bbBABEL_ENC_SBC_H_

/** @file
    String API for SBC (single byte character) encoding.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

#if defined(bbNO_STRLEN)
int bbStrLen_sbc(const bbCHAR8* pStr);
#else
#if bbOS == bbOS_PALMOS
#define bbStrLen_sbc(pStr) StrLen((const Char*)pStr)
#else
#define bbStrLen_sbc(pStr) strlen(pStr)
#endif
#endif

#if defined(bbNO_STRCPY)
bbCHAR8* bbStrCpy_sbc(bbCHAR8* pDst, const bbCHAR8* pSrc);
#else
#if bbOS == bbOS_PALMOS
#define bbStrCpy_sbc(pDst, pSrc) (bbCHAR8*)StrCopy((Char*)pDst, (const Char*)pSrc)
#else
#define bbStrCpy_sbc(pDst, pSrc) (bbCHAR8*)strcpy((char*)pDst, (const char*)pSrc)
#endif
#endif

#if defined(bbNO_STRNCPY)
bbCHAR8* bbStrNCpy_sbc(bbCHAR8* pDst, const bbCHAR8* pSrc, bbUINT const len);
#else
#if bbOS == bbOS_PALMOS
#define bbStrNCpy_sbc(pDst, pSrc, len) (bbCHAR8*)StrNCopy((Char*)pDst, (const Char*)pSrc, len)
#else
#define bbStrNCpy_sbc(pDst, pSrc, len) (bbCHAR8*)strncpy((char*)pDst, (const char*)pSrc, len)
#endif
#endif

#if defined(bbNO_STRCMP)
int bbStrCmp_sbc(const bbCHAR8* pStr1, const bbCHAR8* pStr2);
#else
#if bbOS == bbOS_PALMOS
#define bbStrCmp_sbc(pStr1, pStr2) StrCompare((const Char*)pStr1, (const Char*)pStr2)
#else
#define bbStrCmp_sbc(pStr1, pStr2) strcmp((const char*)pStr1, (const char*)pStr2)
#endif
#endif

#if defined(bbNO_STRICMP)
int bbStrICmp_sbc(const bbCHAR8* pStr1, const bbCHAR8* pStr2);
#else
#if bbOS == bbOS_PALMOS 
#define bbStrICmp_sbc(pStr1, pStr2) StrCaselessCompare((const Char*)pStr1, (const Char*)pStr2)
#elif bbOS == bbOS_POSIX
#define bbStrICmp_sbc(pStr1, pStr2) strcasecmp((const char*)pStr1, (const char*)pStr2)
#else
#ifdef _MSC_VER
#define bbStrICmp_sbc(pStr1, pStr2) _stricmp((const char*)pStr1, (const char*)pStr2)
#else
#define bbStrICmp_sbc(pStr1, pStr2) strcasecmp((const char*)pStr1, (const char*)pStr2)
#endif
#endif
#endif

#if defined(bbNO_STRSTR)
bbCHAR* bbStrStr_sbc(const bbCHAR8* pStr1, const bbCHAR8* pStr2);
#else
#define bbStrStr_sbc(pStr1, pStr2) (bbCHAR8*)strstr((const char*)pStr2, (const char*)pStr1)
#endif

#ifdef _MSC_VER
char* strcasestr(const char *s, const char *find);
#endif
#define bbStrIStr_sbc(pStr1, pStr2) strcasestr(pStr1, pStr2)

#if defined(bbNO_SPRINTF)
int bbSprintf_sbc(bbCHAR8*, const bbCHAR8*, ...);
#else
#if bbOS == bbOS_PALMOS 
#define bbSprintf_sbc StrPrintF
#else
#define bbSprintf_sbc sprintf
#endif
#endif

#if defined(bbNO_VSPRINTF)
int bbVsnprintf_sbc(bbCHAR8*, bbUINT, const bbCHAR8*, bbVALIST);
#else
#if bbOS == bbOS_PALMOS 
#define bbVsnprintf_sbc StrVNPrintF
#else
#define bbVsnprintf_sbc vsnprintf
#endif
#endif

#if defined(bbNO_PRINTF)
int bbPrintf_sbc(const bbCHAR8*, ...);
#else
#if bbOS == bbOS_PALMOS
#define bbPrintf_sbc(...)
#else
#define bbPrintf_sbc printf
#endif
#endif

#if defined(bbNO_VPRINTF)
int bbVprintf_sbc(const bbCHAR8*, bbVALIST);
#else
#if bbOS == bbOS_PALMOS
#define bbVprintf_sbc(...)
#else
#define bbVprintf_sbc vprintf
#endif
#endif

bbCHARCP bbEncGetCP_sbc(bbENCSTATE* const pState);
bbCHARCP bbEncGetCUBack_sbc(bbENCSTATE* const pState);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_ENC_SBC_H_ */
