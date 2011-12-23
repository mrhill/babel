#include "str.h"
#include "mem.h"

#define bbCEENCMASK 0x3FU
#define bbCEMODPOS 6U
#define bbCEMODMASK 3U

bbU8 bbgCUSizes[] = bbENCCUSIZES;

/*
int bbSprintf( bbCHAR* pDst, const bbCHAR* pFmt, ...)
{
    int ret;
    va_list args;
    va_start(args, pFmt);
    ret = bbVsprintf( pDst, pFmt, args );
    va_end(args);
    return ret;
}

int bbPrintf( const bbCHAR* pFmt, ...)
{
    int ret;
    va_list args;
    va_start(args, pFmt);
    ret = bbVprintf( pFmt, args );
    va_end(args);
    return ret;
}

*/

bbCHAR* bbStrDup(const bbCHAR* const pStr)
{
    if (!pStr)
    {
        return NULL;
    }
    else
    {
        bbCHAR* const pTmp = (bbCHAR*)bbMemAlloc((bbStrLen(pStr) + 1) * sizeof(bbCHAR));

        if (!pTmp)
            return NULL;

        return bbStrCpy(pTmp, pStr);
    }
}

bbCHAR* bbStrDupEx(const bbCHAR* const pStr, bbUINT const extralen)
{
    bbCHAR* const pTmp = (bbCHAR*)bbMemAlloc((bbStrLen(pStr) + extralen + 1) * sizeof(bbCHAR));

    if (!pTmp)
        return NULL;

    return bbStrCpy(pTmp, pStr);
}

bbCHAR* bbStrCpySBC(bbCHAR* pDst, const char* pSrc)
{
    bbUINT c;
    do
    {
        c = *pSrc++;
        *pDst++ = (bbCHAR)c;
    } while(c);
    return pDst - 1;
}

bbU32 bbStrHash(const bbCHAR* const pStr)
{
    bbU32 hash=0;
    bbUINT i=0;
    for(;;)
    {
        bbUINT c = pStr[i++];
        if (!c)
            break;
        
        hash += (bbU32)c << ((i&3)<<3);
    }
    return hash;
}

bbCHAR* bbStrSkipSpace(const bbCHAR* pStr)
{
    const bbCHAR* pStr2;
    bbCHARCP cp;

    for(;;)
    {
        pStr2 = pStr;
        bbCP_NEXT_PTR(pStr2, cp);
        if ((cp!=' ')&&(cp!=9))
            break;
        pStr = pStr2;
    }
    return (bbCHAR*)pStr;
}

bbCHAR* bbStrSkip(const bbCHAR* pStr, bbCHARCP const sepChar, bbCHAR** ppWordEnd)
{
    bbCHARCP cp;
    const bbCHAR* pWordEnd = NULL;
    const bbCHAR* pStr2;

    for(;;)
    {
        pStr2 = pStr;
        bbCP_NEXT_PTR(pStr2, cp);

        if ((cp==0) || (cp == sepChar))
            break;

        if ((cp!=' ') && (cp!=9))
        {
            pWordEnd = NULL;
        }
        else 
        {
            if (sepChar==0)
                break;
            if (!pWordEnd)
                pWordEnd = pStr;
        }

        pStr = pStr2;
    }

    if (ppWordEnd)
        *ppWordEnd = (bbCHAR*)(pWordEnd ? pWordEnd : pStr);
    return (bbCHAR*)pStr;
}

bbCHAR** bbStrSplit(const bbCHAR* pStr, bbU32 opt, bbUINT* const pCount)
{
    bbCHARCP cp;
    bbCHARCP const sepChar = opt & bbSTRSPLIT_CHARCPMASK;
    bbUINT   count = 1;
    bbUINT   strln;
    bbUINT   i;
    bbCHAR** pList;
    bbCHAR** pEntry;
    bbCHAR*  pDst = (bbCHAR*)pStr;
    bbCHAR*  pWordEnd;
    const bbCHAR* pWordStart;

    if (!pStr)
    {
        bbErrSet(bbEBADPARAM);
        return NULL;
    }

    //
    // Count sub-strings and source string len
    //
    if (opt & bbSTRSPLIT_STRIPSPACE)
        pDst = bbStrSkipSpace(pDst);
    for(;;)
    {
        bbCP_NEXT_PTR(pDst, cp);
        if (!cp) break;

        if (!sepChar)
        {
            if ((cp==' ')||(cp==9))
            {
                count++;
                if (opt & bbSTRSPLIT_STRIPSPACE)
                    pDst = bbStrSkipSpace(pDst);
            }
        }
        else
        {
            if (cp == sepChar)
                count++;
        }
    }
    strln = pDst - pStr - 1;
    bbASSERT(strln == bbStrLen(pStr));

    //
    // Prepare memory
    //
    i = (count + 2) * sizeof(bbCHAR*);  // sub-string index + 1 terminating NULL pointer + 1 endptr

    if (!(opt & bbSTRSPLIT_INPLACE))    // copy of source string
        i += (strln + 1) * sizeof(bbCHAR);

    if ((pList = (bbCHAR**)bbMemAlloc(i)) == NULL)
        return NULL;

    //
    // Split the shit
    //
    pEntry = pList;
    if (opt & bbSTRSPLIT_INPLACE)
    {
        for(;;)
        {
            if (opt & bbSTRSPLIT_STRIPSPACE)
                pStr = bbStrSkipSpace(pStr);

            *pEntry++ = (bbCHAR*)pStr; // start of split substring

            pStr = bbStrSkip(pStr, sepChar, &pWordEnd);
            cp = *pStr;

            if (opt & bbSTRSPLIT_STRIPSPACE)
                *pWordEnd = 0; // terminate substring early
            else
                *(bbCHAR*)pStr = 0; // terminate substring

            if (cp == 0)
                break;

            pStr++;
        }
    }
    else
    {
        pDst = (bbCHAR*)(pList + (count+2));

        for(;;)
        {
            if (opt & bbSTRSPLIT_STRIPSPACE)
                pStr = bbStrSkipSpace(pStr);

            pWordStart = pStr;
            pStr = bbStrSkip(pStr, sepChar, &pWordEnd);
            if (!(opt & bbSTRSPLIT_STRIPSPACE))
                pWordEnd = (bbCHAR*)pStr;

            *pEntry++ = pDst;

            strln = (bbU32)(bbUPTR)pWordEnd - (bbU32)(bbUPTR)pWordStart;
            bbMemCpy(pDst, pWordStart, strln);
            pDst = (bbCHAR*)((bbU8*)pDst + strln);
            *pDst++ = 0; // terminate substring

            if (*pStr == 0)
                break;
            pStr++;
        }
    }

    bbASSERT(pEntry == (pList + count));
    pEntry[0] = NULL; // terminate index
    pEntry[1] = (bbCHAR*)pStr; // input string end pointer

    if (pCount)
        *pCount = count;
    return pList;
}

bbU32 bbStrToU32(const bbCHAR* pStr, bbUINT opt)
{
    bbERR err;
    bbS64 num;

    bbErrSet(bbEOK);
    err = bbStrToS64((bbCHAR*)pStr, NULL, &num, opt);
    if (bbErrGet() != bbEOK)
        return 0;

    if ((bbU64)num > 0xFFFFFFFFUL)
    {
        bbErrSet(bbEOVERFLOW);
        return 0;
    }

    return (bbU32)num;
}

bbS32 bbStrToS32(const bbCHAR* pStr, bbUINT opt)
{
    bbERR err;
    bbS64 num;
    bbS32 num32;

    bbErrSet(bbEOK);
    err = bbStrToS64((bbCHAR*)pStr, NULL, &num, opt|bbSTROPT_MINUS);
    if (bbErrGet() != bbEOK)
        return 0;

    num32 = (bbS32)num;
    if (num32 != num)
    {
        bbErrSet(bbEOVERFLOW);
        return 0;
    }

    return num32;
}

bbERR bbStrToS64(bbCHAR* pStr, bbCHAR** ppStr, bbS64* pNum, bbUINT opt)
{
    #define bbSTRTONUM_FMTHEX 3
    #define bbSTRTONUM_FMTDEC 2
    #define bbSTRTONUM_FMTOCT 1
    #define bbSTRTONUM_FMTBIN 0

    bbUINT i = 0, j;
    bbCHARCP cp;
    bbUINT fmt = 0xF;   /* bit0 dec, bit1 hex, bit2 bin, bit3 oct */
    bbUINT minus = 0;
    bbU32  val;
    bbCHARCP bin_endcp = 0;
    bbS64  num[4]={0,0,0,0};

    *pNum = 0;

    if (opt & bbSTROPT_MODECP)
    {
        bbCP_NEXT(pStr, i, cp);

        if (cp==0)
            return bbErrSet(bbENOTFOUND); /* leave ppStr as is */

        if ((opt & bbSTROPT_ESC) && (cp == '\\'))
        {
            j = i;
            bbCP_GET(pStr, j, cp);
            if (cp != '\\')
                goto bbStrToS64_numeral;
            i = j;
        }

        *pNum = (bbS64)cp;
        if (ppStr) *ppStr = pStr + i;
        return bbEOK;
    }

    bbStrToS64_numeral:

    /* skip leading stuff */
    for(;;)
    {
        j = i;
        bbCP_NEXT(pStr, i, cp);

        if (cp==0)
        {
            if (ppStr) *ppStr = pStr + j;
            return bbErrSet(bbENOTFOUND);
        }
        if ((opt & bbSTROPT_MODECP) || minus) break; // no space skipping
        if ((cp!=32)&&(cp!=9)) break;
    }

    if ((opt & bbSTROPT_MINUS)  && (cp == '-'))
    {
        if (minus)
        {
            if (ppStr) *ppStr = pStr + j;
            return bbErrSet(bbESYNTAX);
        }
        minus = 1;
        goto bbStrToS64_numeral;
    }
    
    if ((opt & bbSTROPT_HEXDOLLAR)  && (cp == '$')) fmt=1<<bbSTRTONUM_FMTHEX;
    if ((opt & bbSTROPT_HEXAMP)     && (cp == '&')) fmt=1<<bbSTRTONUM_FMTHEX;
    if ((opt & bbSTROPT_BINPERCENT) && (cp == '%')) fmt=1<<bbSTRTONUM_FMTBIN;
    if ((opt & bbSTROPT_BINB)       && ((cp &~ 32UL) == 'B')) fmt=1<<bbSTRTONUM_FMTBIN;

    if (cp == '0')
    {
        bbCP_NEXT(pStr, i, cp);

        if ((opt & bbSTROPT_HEX0X) && ((cp &~ 32UL) == 'X'))
            fmt=1<<bbSTRTONUM_FMTHEX;
        if ((opt & bbSTROPT_OCT) && (((bbU32)cp - '0') <= ('9'-'0')))
            fmt=1<<bbSTRTONUM_FMTOCT;
    }

    if (fmt != 0xF)
    {
        /* a prefix was found, don't look for suffixes */
        opt &= ~bbSTROPT_ALLSUF;
    }
    else
    {
        i=j; /* at this point i points to start of digit string */

        /* If no suffix format specs requested, speed-up by scanning only 1 number format */
        if (!(opt & bbSTROPT_ALLSUF)) fmt = 1<<bbSTRTONUM_FMTDEC;
    }

    fmt |= fmt<<4; /* save org format mask */
    do
    {
        j = i;
        bbCP_NEXT(pStr, i, cp);

        if ((val = (bbU32)cp - '0') > ('9'-'0'))
        {
            if ((val = ((bbU32)cp&0xDF) - 'A') <= ('F'-'A'))
                val+=10;
            else
                break;
        }

        if (fmt & (1<<bbSTRTONUM_FMTHEX))
        {
            fmt |= 0x100 << bbSTRTONUM_FMTHEX;
            num[bbSTRTONUM_FMTHEX] = (num[bbSTRTONUM_FMTHEX]<<4) | val;
        }
        if (fmt & (1<<bbSTRTONUM_FMTDEC))
        {
            if (val > 9)
                fmt &= ~(1<<bbSTRTONUM_FMTDEC);
            else
            {
                fmt |= 0x100 << bbSTRTONUM_FMTDEC;
                num[bbSTRTONUM_FMTDEC] = num[bbSTRTONUM_FMTDEC]*10 + val;
            }
        }
        if (fmt & (1<<bbSTRTONUM_FMTOCT))
        {
            if (val > 7)
                fmt &= ~(1<<bbSTRTONUM_FMTOCT);
            else
            {
                fmt |= 0x100 << bbSTRTONUM_FMTOCT;
                num[bbSTRTONUM_FMTOCT] = (num[bbSTRTONUM_FMTOCT]<<3) | val;
            }
        }
        if (fmt & (1<<bbSTRTONUM_FMTBIN))
        {
            if (val > 1)
            {
                fmt &= ~(1<<bbSTRTONUM_FMTBIN);
                bin_endcp = cp;
            }
            else
            {
                fmt |= 0x100 << bbSTRTONUM_FMTBIN;
                num[bbSTRTONUM_FMTBIN] = (num[bbSTRTONUM_FMTBIN]<<1) | val;
            }
        }
    } while (fmt & 0xF);

    if ((fmt & (0x10 << bbSTRTONUM_FMTHEX)) && (opt & bbSTROPT_HEXSUFH))
    {
        if ((cp &~ 32UL) == 'H')
            cp = 32;
        else
            fmt &= ~(0x10 << bbSTRTONUM_FMTHEX); /* we expected suffix, but didn't find one */
    }

    if ((fmt & (0x10 << bbSTRTONUM_FMTBIN)) && (opt & bbSTROPT_BINSUFB))
    {
        if ((bin_endcp &~ 32UL) != 'B')
            fmt &= ~(0x10 << bbSTRTONUM_FMTBIN); /* we expected suffix, but didn't find one */
        else if (bin_endcp == cp)
            cp = 32;
    }

    /* skip trailing stuff */
    if (!(opt & bbSTROPT_MODECP))
    {
        while ((cp==32)||(cp==9))
        {
            j = i;
            bbCP_NEXT(pStr, i, cp);
        }
    }

    if (ppStr) *ppStr = pStr + j;
    j=3, i=0x80;
    do
    {
        if (fmt & i)
        {
            if ((fmt & (i<<4)) == 0)
                return bbErrSet(bbENOTFOUND); /* no digits found */

            *pNum = minus ? 0-num[j] : num[j];
            break;
        }
        i>>=1;
    } while ((int)--j >= 0);

    if (((opt & bbSTROPT_STOPONUKCHAR)==0) && (cp != 0)) /* 0-term was required */
    {
        return bbErrSet(bbESYNTAX);
    }

    return bbEOK;
}

bbU64* bbStrToArrU64(bbCHAR** ppStr, bbUINT const opt, bbUINT* const pSize)
{
    bbERR err;
    bbU64* pArrU64 = NULL;
    bbUINT arrsize, size = 0;

    if ((pArrU64 = (bbU64*)bbMemAlloc(sizeof(bbU64) * (arrsize=4))) == NULL)
        return NULL;

    for (;;)
    {
        err = bbStrToS64(*ppStr, ppStr, (bbS64*)pArrU64 + size, opt|bbSTROPT_STOPONUKCHAR);

        if ((err == bbENOTFOUND) && (**ppStr == 0)) // success, end of string
        {
            *pSize = size;
            return pArrU64;
        }

        if (err != bbEOK)
        {
            if (err == bbESYNTAX)
                bbErrSet(bbESYNTAX);
            goto bbStrToArrU64_err;
        }

        if (++size >= arrsize)
        {
            if (bbMemRealloc(sizeof(bbU64) * (arrsize<<=1), (void**)&pArrU64) != bbEOK)
                goto bbStrToArrU64_err;
        }
    }

    bbStrToArrU64_err:
    bbMemFree(pArrU64);
    return NULL;
}

static int bbCpg_Get0(bbCHARCP const cp)
{
    return 0;
}

static const bbCPGTABLE gCPGTab_UNICODE =
{
    bbCpgUnicode_IsWide,
    bbCpgUnicode_IsNonspace,
    bbCPGPROP_HASWIDE | bbCPGPROP_HASNONSPACE
};

static const bbCPGTABLE gCPGTab_ISO8859_1 =
{
    bbCpg_Get0,
    bbCpg_Get0,
    0
};

static const bbCPGTABLE* const gCpgTabLookUp[] =
{
    #if bbCPG_ASCII != 0
    #error
    #endif
    &gCPGTab_ISO8859_1, // Latin 1 is superset of ASCII

    #if bbCPG_UNICODE != 1
    #error
    #endif
    &gCPGTab_UNICODE,

    #if bbCPG_UCS != 2
    #error
    #endif
    &gCPGTab_UNICODE, // UNICODE is superset of UCS2

    #if bbCPG_ISO8859_1 != 3
    #error
    #endif
    &gCPGTab_ISO8859_1
};

const bbCPGTABLE* bbCpgGetTable(bbUINT const codepage)
{
    bbASSERT(codepage < (sizeof(gCpgTabLookUp) / sizeof(bbCPGTABLE*)));
    return gCpgTabLookUp[codepage];
}


#if bbSTRCONV==bbSTRCONV_INTERNAL

bbSTRCONVH bbStrConvOpen( const bbUINT encto, const bbUINT encfrom, const bbUINT flags)
{
    bbSTRCONVH const handle = (bbSTRCONVH) bbMemAlloc( sizeof(struct bbsSTRCONVH));

    if (handle)
    {
        handle->encfrom = encfrom;
        handle->encto   = encto;
        handle->first   = 1;
    }

    return handle;
}

void bbStrConvClose( bbSTRCONVH const handle)
{
    if (handle != bbSTRCONVH_INVALID)
    {
        bbMemFree(handle);
    }
}

typedef bbERR (*bbfCConvEncodeCP)( bbU32 cp, bbU8** const ppOut, bbUINT* const pOutLeft);

static bbERR bbStrConvEncodeCP_SBC( bbU32 cp, bbU8** const ppOut, bbUINT* const pOutLeft)
{
    if (*pOutLeft == 0) return bbEFULL;
    --*pOutLeft;

    *((*ppOut)++) = (bbU8) cp;

    return bbEOK;
}

static bbERR bbStrConvEncodeCP_DBCLE( bbU32 cp, bbU8** const ppOut, bbUINT* const pOutLeft)
{
    register bbU8* pOut;
    if (*pOutLeft < 2) return bbEFULL;
    *pOutLeft-=2;

    pOut = *ppOut;
    *(pOut++) = (bbU8) cp;
    *(pOut++) = (bbU8) (cp>>8);
    *ppOut = pOut;

    return bbEOK;
}

static bbERR bbStrConvEncodeCP_DBCBE( bbU32 cp, bbU8** const ppOut, bbUINT* const pOutLeft)
{
    register bbU8* pOut;

    if (*pOutLeft < 2) return bbEFULL;
    *pOutLeft-=2;

    pOut = *ppOut;
    *(pOut++) = (bbU8) (cp>>8);
    *(pOut++) = (bbU8) cp;
    *ppOut = pOut;

    return bbEOK;
}

static bbERR bbStrConvEncodeCP_UTF16LE( bbU32 cp, bbU8** const ppOut, bbUINT* const pOutLeft)
{
    register bbU8* pOut = *ppOut;
    register bbUINT cu;

    if (cp < 0x10000UL)
    {
        if (*pOutLeft < 2) return bbEFULL;
        *pOutLeft -= 2;

        *(pOut++) = (bbU8) cp;
        *(pOut++) = (bbU8) (cp>>8);
        *ppOut = pOut;

        return bbEOK;
    }

    if (*pOutLeft < 4) return bbEFULL;
    *pOutLeft -= 4;

    cp -= 0x10000UL;
    cu = (cp &~ 0xFC00U) | 0xD800U;
    *(pOut++) = (bbU8) cu;
    *(pOut++) = (bbU8) (cu>>8);
    cu = (cp >> 10) | 0xDC00U;
    *(pOut++) = (bbU8) cu;
    *(pOut++) = (bbU8) (cu>>8);

    *ppOut = pOut;

    return bbEOK;
}

static bbERR bbStrConvEncodeCP_UTF16BE( bbU32 cp, bbU8** const ppOut, bbUINT* const pOutLeft)
{
    register bbU8* pOut = *ppOut;
    register bbUINT cu;

    if (cp < 0x10000UL)
    {
        if (*pOutLeft < 2) return bbEFULL;
        *pOutLeft -= 2;

        *(pOut++) = (bbU8) (cp>>8);
        *(pOut++) = (bbU8) cp;
        *ppOut = pOut;

        return bbEOK;
    }

    if (*pOutLeft < 4) return bbEFULL;
    *pOutLeft -= 4;

    cp -= 0x10000UL;
    cu = (cp &~ 0xFC00U) | 0xD800U;
    *(pOut++) = (bbU8) (cu>>8);
    *(pOut++) = (bbU8) cu;
    cu = (cp >> 10) | 0xDC00U;
    *(pOut++) = (bbU8) (cu>>8);
    *(pOut++) = (bbU8) cu;

    *ppOut = pOut;

    return bbEOK;
}

bbERR bbStrConvDo( bbSTRCONVH const handle, const bbU8** const ppIn, bbUINT* const pInLeft, bbU8** const ppOut, bbUINT* const pOutLeft)
{
    bbERR err = bbEOK;

    const bbU8* pIn = *ppIn;
    const bbU8* pInEnd = pIn + *pInLeft;

    bbfCConvEncodeCP encode;

    switch (handle->encto & bbCEENCMASK)
    {
    case bbENC_SBC: encode = bbStrConvEncodeCP_SBC; break;
    case bbENC_DBC:
        if (handle->encto & (bbENCMOD_BE<<bbCEMODPOS))
            encode = bbStrConvEncodeCP_DBCBE; 
        else
            encode = bbStrConvEncodeCP_DBCLE;
        break;
    case bbENC_UTF16:
        if (handle->encto & (bbENCMOD_BE<<bbCEMODPOS))
        {
            encode = bbStrConvEncodeCP_UTF16BE; 
        }
        else
        {
            encode = bbStrConvEncodeCP_UTF16LE;

            if (handle->first && ((handle->encto & (bbCEMODMASK<<bbCEMODPOS)) == (bbENCMOD_NONE<<bbCEMODPOS)))
            {
                /* if writing to UTF16 we default to UTF16LE and write a BOM */

                if ((err = bbStrConvEncodeCP_UTF16LE( 0xFEFFU, ppOut, pOutLeft)) != bbEOK)
                {
                    return bbErrSet(err);
                }

                handle->first = 0;
            }
        }
        break;
    default:
        return bbErrSet(bbEUK);
    }

    switch (handle->encfrom & bbCEENCMASK)
    {
    case bbENC_SBC:
        while (pIn < pInEnd)
        {
            const bbU32 cp = (bbU32) *pIn;

            err = encode( cp, ppOut, pOutLeft);
            if (err != bbEOK) break;

            pIn++;
        }
        break;

    case bbENC_DBC:
        pInEnd--;
        if (handle->encfrom & (bbENCMOD_BE<<bbCEMODPOS))
        {
            while (pIn < pInEnd)
            {
                const bbU32 cp = bbLD16BE(pIn);

                err = encode( cp, ppOut, pOutLeft);
                if (err != bbEOK) break;

                pIn += 2;
            }
        }
        else
        {
            while (pIn < pInEnd)
            {
                const bbU32 cp = bbLD16LE(pIn);

                err = encode( cp, ppOut, pOutLeft);
                if (err != bbEOK) break;

                pIn += 2;
            }
        }

        pInEnd++;
        break;

    case bbENC_UTF8:
        while (pIn < pInEnd)
        {
            bbU32 cp = *pIn++;
            if ((cp-0xC0U) < 0x35U)
            {
                bbUINT const trailByteCount = (bbUINT) bbgUTF8_CountTrailBytes[cp >> 3];

                if ((pIn + trailByteCount) > pInEnd)
                {
                    --pIn;
                    break;
                }

                cp = bbUTF8_CU_MASK_LEAD(cp, trailByteCount);
                switch(trailByteCount)
                {
                case 3: cp=(cp<<6)|(*pIn++&0x3FU);
                case 2: cp=(cp<<6)|(*pIn++&0x3FU);
                case 1: cp=(cp<<6)|(*pIn++&0x3FU);
                break;
                }
            }

            err = encode( cp, ppOut, pOutLeft);
            if (err != bbEOK) break;
        }
        break;

    case bbENC_UTF16:

        if ((handle->encfrom & (bbCEMODMASK<<bbCEMODPOS)) == (bbENCMOD_NONE<<bbCEMODPOS))
        {
            /* get BOM, if no BOM is found, little endian is assumed */

            register bbUINT firstcu;

            if ((pIn+2) > pInEnd) break;

            firstcu = bbLD16LE(pIn);

            if (firstcu == 0xFFFEU)
            {
                /* detected UTF16BE */
                handle->encfrom |= (bbENCMOD_BE<<bbCEMODPOS);
                pIn += 2;
            }
            else if (firstcu == 0xFEFFU)
            {
                /* detected UTF16LE */
                pIn += 2;
            }
        }

        pInEnd--;

        if (handle->encfrom & (bbENCMOD_BE<<bbCEMODPOS))
        {
            for (;;)
            {
                register bbUINT cu;

                if (pIn >= pInEnd) break;

                cu = bbLD16BE(pIn);

                if ((cu & 0xFC00U) == 0xD800U) /* is lead surrogate? */
                {
                    /* get trail surrogate and merge both to code point */
                    /* does not check if second code unit is a valid trail, ie in range U+DC00 - U+DFFF */

                    register bbU32 cp;

                    if ((pIn+2) >= pInEnd) break;

                    cp = ((bbU32)cu<<10) + bbLD16BE(pIn+2) - ((0xD800UL<<10)+0xDC00UL-0x10000UL);

                    err = encode( cp, ppOut, pOutLeft);
                    if (err != bbEOK) break;

                    pIn += 4;
                }
                else
                {
                    err = encode( (bbU32)cu, ppOut, pOutLeft);
                    if (err != bbEOK) break;

                    pIn += 2;
                }
            }
        }
        else
        {
            for (;;)
            {
                register bbUINT cu;

                if (pIn >= pInEnd) break;

                cu = bbLD16LE(pIn);

                if ((cu & 0xFC00U) == 0xD800U) /* is lead surrogate? */
                {
                    /* get trail surrogate and merge both to code point */
                    /* does not check if second code unit is a valid trail, ie in range U+DC00 - U+DFFF */

                    register bbU32 cp;

                    if ((pIn+2) >= pInEnd) break;

                    cp = ((bbU32)cu<<10) + bbLD16LE(pIn+2) - ((0xD800UL<<10)+0xDC00UL-0x10000UL);

                    err = encode( cp, ppOut, pOutLeft);
                    if (err != bbEOK) break;

                    pIn += 4;
                }
                else
                {
                    err = encode( (bbU32)cu, ppOut, pOutLeft);
                    if (err != bbEOK) break;

                    pIn += 2;
                }
            }
        }

        pInEnd++;

        break;

    default:
        return bbErrSet(bbEUK);
    }

    *ppIn = pIn;
    *pInLeft = (bbUINT)(bbUPTR)pInEnd - (bbUINT)(bbUPTR)pIn;

    return bbErrSet(err);
}

#elif bbSTRCONV==bbSTRCONV_WIN

#error not implemented

#endif /* bbSTRCONV */

bbU8* bbStrConvMemTo( const bbUINT encoding, const bbUINT flags, const bbCHAR* pSrc)
{
    bbERR err;

    bbUINT dstsize;
    bbUINT dstleft;
    bbUINT srclen;

    bbU8*  pDst = NULL;
    bbU8*  pDstPos = NULL;

    bbSTRCONVH convh = bbStrConvOpen( encoding, bbCE_INTERNAL, flags);

    if (convh != bbSTRCONVH_INVALID)
    {
        srclen = bbStrLen(pSrc);
        dstleft = dstsize = (srclen + 2) * bbgCUSizes[ encoding & bbCEENCMASK ];
        srclen = srclen * sizeof(bbCHAR);

        do
        {
            const bbU32 offs = (bbU32)(bbUPTR)pDstPos - (bbU32)(bbUPTR)pDst;
            err = bbMemRealloc( dstsize, (void**)&pDst);
            pDstPos = pDst + offs;

            if (err == bbEOK)
            {
                err = bbStrConvDo( convh, (const bbU8**)&pSrc, &srclen, &pDstPos, &dstleft);

                if (err == bbEOK)
                {
                    /* if conversion finished then append 0-terminator */

                    const bbUINT termsize = bbgCUSizes[ encoding & bbCEENCMASK ];

                    if (dstleft < termsize)
                    {
                        const bbU32 offs = (bbU32)(bbUPTR)pDstPos - (bbU32)(bbUPTR)pDst;
                        err = bbMemRealloc( dstsize+termsize, (void**)&pDst);
                        pDstPos = pDst + offs;
                    }

                    if (err == bbEOK) bbMemSet( pDstPos, 0, termsize);
                }
                else
                {
                    dstleft += dstsize>>1;
                    dstsize += dstsize>>1;
                }
            }

        } while (err==bbEFULL);

        if (err != bbEOK) bbMemFreeNull( (void**)&pDst);

        bbStrConvClose(convh);
    }

    return pDst;
}

bbCHAR* bbStrConvMemFrom( const bbUINT encoding, const bbUINT flags, const bbU8* pSrc)
{
    bbERR err;

    bbUINT dstsize;
    bbUINT dstleft;
    bbUINT srclen = 0;

    bbCHAR* pDst = NULL;
    bbCHAR* pDstPos = NULL;

    bbSTRCONVH convh = bbStrConvOpen( bbCE_INTERNAL, encoding, flags);

    if (convh != bbSTRCONVH_INVALID)
    {
        /* get length of source string in bytes */

        switch (bbgCUSizes[ encoding & bbCEENCMASK ])
        {
        case 1: srclen = bbStrLen_sbc(pSrc);               break;
        case 2: while (bbLD16(pSrc+srclen*2)!=0) srclen++; break;
        case 4: while (bbLD32(pSrc+srclen*4)!=0) srclen++; break;
        default: err = bbErrSet(bbEUK); goto bbStrConvMemFrom_err;
        }

        dstleft = dstsize = (srclen + 2) * sizeof(bbCHAR);
        srclen = srclen * bbgCUSizes[ encoding & bbCEENCMASK ];

        /* convert */

        do
        {
            const bbU32 offs = (bbU32)(bbUPTR)pDstPos - (bbU32)(bbUPTR)pDst;
            err = bbMemRealloc( dstsize, (void**)&pDst);
            pDstPos = pDst + offs;

            if (err == bbEOK)
            {
                err = bbStrConvDo( convh, &pSrc, &srclen, (bbU8**)&pDstPos, &dstleft);

                if (err == bbEOK)
                {
                    /* if conversion finished then append 0-terminator */

                    if (dstleft < sizeof(bbCHAR))
                    {
                        const bbU32 offs = (bbU32)(bbUPTR)pDstPos - (bbU32)(bbUPTR)pDst;
                        err = bbMemRealloc( dstsize+sizeof(bbCHAR), (void**)&pDst);
                        pDstPos = pDst + offs;
                    }

                    if (err == bbEOK) *(bbCHAR*)pDstPos = 0;
                }
                else
                {
                    dstleft += dstsize>>1;
                    dstsize += dstsize>>1;
                }
            }
        } while (err==bbEFULL);

        if (err != bbEOK) bbMemFreeNull( (void**)&pDst);

        bbStrConvMemFrom_err:

        bbStrConvClose(convh);
    }

    return pDst;
}
