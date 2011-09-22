#ifndef bbBABEL_ENC_DBC_H_
#define bbBABEL_ENC_DBC_H_

/** @file
    String API for DBC (double byte character) encoding.
*/

#include "defs.h"
#include <wchar.h>

#if (bbOS == bbOS_WIN32)||(bbOS == bbOS_WINCE)
#include <shlwapi.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

#if defined(bbNO_WCSLEN)
int bbStrlen_dbc(const bbCHAR16* pStr);
#else
#define bbStrlen_dbc(pStr) wcslen(pStr)
#endif

#if defined(bbNO_WCSCPY)
bbCHAR16* bbStrCpy_dbc(bbCHAR16* pDst, const bbCHAR16* pSrc);
#else
#define bbStrCpy_dbc(pDst, pSrc) (bbCHAR16*)wcscpy(pDst, pSrc)
#endif

#if defined(bbNO_WCSNCPY)
bbCHAR16* bbStrNCpy_dbc(bbCHAR16* pDst, const bbCHAR16* pSrc, bbUINT len);
#else
#define bbStrNCpy_dbc(pDst, pSrc, len) (bbCHAR16*)wcsncpy(pDst, pSrc, len)
#endif

#if defined(bbNO_STRCMP)
int bbStrCmp_dbc(const bbCHAR16* pStr1, const bbCHAR16* pStr2);
#else
#define bbStrCmp_dbc(pStr1, pStr2) wcscmp(pStr1, pStr2)
#endif

#if defined(bbNO_STRICMP)
int bbStrICmp_dbc(const bbCHAR16* pStr1, const bbCHAR16* pStr2);
#else
#define bbStrICmp_dbc(pStr1, pStr2) _wcsicmp(pStr1, pStr2)
#endif

#if defined(bbNO_STRNCMP)
int bbStrNCmp_dbc(const bbCHAR16* pStr1, const bbCHAR16* pStr2, bbUINT len);
#else
#define bbStrNCmp_dbc(pStr1, pStr2, len) wcsncmp(pStr1, pStr2, len)
#endif

#if defined(bbNO_STRNICMP)
int bbStrNICmp_dbc(const bbCHAR16* pStr1, const bbCHAR16* pStr2, bbUINT len);
#else
#define bbStrNICmp_dbc(pStr1, pStr2, len) _wcsnicmp(pStr1, pStr2, len)
#endif

#if defined(bbNO_STRSTR)
bbCHAR* bbStrStr_dbc(const bbCHAR16* pStr1, const bbCHAR16* pStr2);
#else
#define bbStrStr_dbc(pStr1, pStr2) wcsstr(pStr1, pStr2)
#endif

#if defined(bbNO_STRISTR)
bbCHAR* bbStrIStr_dbc(const bbCHAR16* pStr1, const bbCHAR16* pStr2);
#else
#define bbStrIStr_dbc(pStr1, pStr2) StrStrI(pStr1, pStr2)
#endif

#if defined(bbNO_SWPRINTF)
int bbSprintf_dbc(const bbCHAR16*, ...);
#else
#define bbSprintf_dbc swprintf
#endif

#if defined(bbNO_VSWPRINTF)
int bbVsnprintf_dbc(bbCHAR16*, bbUINT, const bbCHAR16*, bbVALIST);
#else
#define bbVsnprintf_dbc _vsnwprintf
#endif

#if defined(bbNO_WPRINTF)
int bbPrintf_dbc(const bbCHAR16*, ...);
#else
#define bbPrintf_dbc wprintf
#endif

#if defined(bbNO_VWPRINTF)
int bbVprintf_dbc(const bbCHAR16*, bbVALIST);
#else
#define bbVprintf_dbc vwprintf
#endif

bbCHARCP bbEncGetCP_dbc(bbENCSTATE* const pState);
bbCHARCP bbEncGetCPBack_dbc(bbENCSTATE* const pState);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_ENC_DBC_H_ */
