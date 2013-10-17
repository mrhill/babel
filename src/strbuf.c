#include "strbuf.h"
#include "mem.h"
#include "str.h"

void bbStrBufInit(bbStrBuf* p)
{
    p->mpStr = p->mBuf;
    bbStrBufClear(p);
}

void bbStrBufInitStr(bbStrBuf* p, const bbCHAR* pStr)
{
    p->mpStr = p->mBuf;
    bbStrBufClear(p);
    bbStrBufCat(p, pStr);
}

bbCHAR* bbStrBufAssign(bbStrBuf* p, const bbCHAR* pStr)
{
    bbStrBufClear(p);
    return bbStrBufCat(p, pStr);
}

#if bbSIZEOF_CHAR != 1
void bbStrBufInitCStr(bbStrBuf* p, const char* pStr)
{
    p->mpStr = p->mBuf;
    bbStrBufClear(p);
    bbStrBufCat(p, pStr);
}
#endif

bbCHAR* bbStrBufInitLen(bbStrBuf* p, bbUINT len)
{
    bbStrBufInit(p);
    return bbStrBufSetLen(p, len);
}

void bbStrBufClear(bbStrBuf* p)
{
    if (p->mpStr != p->mBuf)
        bbMemFree(p->mpStr);

    p->mpStr = p->mBuf;
    p->mLen = 0;
    p->mBuf[0] = 0;
    p->mCapacity = bbARRSIZE(p->mBuf);
}

void bbStrBufInitStrN(bbStrBuf* p, const bbCHAR* pStr, bbUINT len)
{
    p->mpStr = p->mBuf;
    bbStrBufClear(p);
    bbStrBufCatN(p, pStr, len);
}

bbCHAR* bbStrBufAssignN(bbStrBuf* p, const bbCHAR* pStr, bbUINT len)
{
    bbStrBufClear(p);
    return bbStrBufCatN(p, pStr, len);
}

bbCHAR* bbStrBufEnsure(bbStrBuf* p, bbUINT str_len)
{
    str_len++; // 0-term

    if ((p->mCapacity - p->mLen) < str_len)
    {
        bbUINT capacity = p->mCapacity;
        do { capacity<<=1; } while ((capacity - p->mLen) < str_len);

        if (p->mpStr == p->mBuf)
        {
            bbCHAR* pNew = (bbCHAR*)bbMemAlloc(capacity * sizeof(bbCHAR));
            if (!pNew)
                return NULL;
            p->mpStr = pNew;
            bbMemMove(pNew, p->mBuf, p->mLen * sizeof(bbCHAR));
        }
        else
        {
            if (bbEOK != bbMemRealloc(capacity * sizeof(bbCHAR), (void**)&p->mpStr))
                return NULL;
        }

        p->mCapacity = capacity;
    }

    return p->mpStr + p->mLen;
}

bbCHAR* bbStrBufSetLen(bbStrBuf* p, bbUINT str_len)
{
    bbCHAR* buf;
    bbStrBufClear(p);
    if ((buf = bbStrBufEnsure(p, str_len)))
        p->mLen = str_len;
    return buf;
}

void bbStrBufAttach(bbStrBuf* p, bbCHAR* const pStr, bbUINT capacity, int len)
{
    bbStrBufClear(p);

    if (pStr)
    {
        p->mpStr = pStr;
        p->mCapacity = capacity;
        p->mLen = len<0 ? bbStrLen(pStr) : (bbUINT)len;
    }
}

bbCHAR* bbStrBufDetach(bbStrBuf* p)
{
    bbCHAR* pStr;

    if (p->mpStr == p->mBuf)
    {
        if ((pStr = bbStrAlloc(p->mLen)) != NULL)
        {
            bbMemMove(pStr, p->mBuf, p->mLen * sizeof(bbCHAR));
            pStr[p->mLen] = 0;
        }
    }
    else
        pStr = p->mpStr;

    p->mpStr = p->mBuf;
    bbStrBufClear(p);

    return pStr;
}

bbCHAR* bbStrBufCat(bbStrBuf* p, const bbCHAR* pStr)
{
    if (pStr)
    {
        bbUINT str_len = bbStrLen(pStr);
        bbCHAR* pInsert = bbStrBufEnsure(p, str_len);
        if (pInsert)
        {
            p->mLen += str_len;
            bbStrCpy(pInsert, pStr);
        }
        return pInsert;
    }
    else
    {
        return p->mpStr + p->mLen;
    }
}

bbCHAR* bbStrBufCatN(bbStrBuf* p, const bbCHAR* pStr, bbUINT len)
{
    if (pStr && len)
    {
        bbCHAR* pDst = bbStrBufEnsure(p, len);
        if (pDst)
        {
            bbUINT copied = 0;
            while (copied < len)
            {
                if ((pDst[copied] = pStr[copied]) == 0)
                    break;
                copied++;
            }
            pDst[copied] = 0;
            p->mLen += copied;
        }

        return pDst;
    }
    else
    {
        return p->mpStr + p->mLen;
    }
}

#if bbSIZEOF_CHAR != 1
bbCHAR* bbStrBufCatCStr(bbStrBuf* p, const char* pStr)
{
    bbCHAR* pInsert = p->mpStr + p->mLen;
    if (pStr)
    {
        bbUINT strLen = bbStrLen_sbc(pStr);

        pInsert = bbStrBufEnsure(p, strLen);
        if (pInsert)
        {
            p->mLen+=strLen;
            bbCHAR* pDst = pInsert;
            while((*pDst++=*pStr++)!=0) {}
        }
    }
    return pInsert;
}
#endif

void bbStrBufCatCP(bbStrBuf* p, bbCHARCP cp)
{
    bbUINT str_len = bbCP_LENGTH(cp);
    bbCHAR* pStr = bbStrBufEnsure(p, str_len);
    if (pStr)
    {
        p->mLen += str_len;
        bbCP_APPEND_PTR(pStr, cp);
        *pStr = 0;
    }
}

void bbStrBufCatChar(bbStrBuf* pStrBuf, char c)
{
    bbCHAR* pStr = bbStrBufEnsure(pStrBuf, 1);
    if (pStr)
    {
        pStrBuf->mLen++;
        pStr[0] = c;
        pStr[1] = 0;
    }
}

bbCHAR* bbStrBufPrintHex(bbStrBuf* p, const bbU8* pData, bbUINT size)
{
    bbCHAR* const pInsert = bbStrBufEnsure(p, size*5);
    if (pInsert)
    {
        bbCHAR* pStr = pInsert;
        const bbU8* const pDataEnd = pData + size;
        while(pData < pDataEnd)
        {
            pStr += bbSprintf(pStr, bbT("0x%02X "), *pData++);
        }
        *(pStr-1) = 0;
        p->mLen += size*5-1;
    }
    return pInsert;
}

int bbStrBufVPrintf(bbStrBuf* p, const bbCHAR* pFmt, bbVALIST args)
{
    int size;
    int retried = 0;

    bbStrBufClear(p);
    for(;;)
    {
        bbVALIST tmp_args;
        bbVACOPY(tmp_args, args);
        size = bbVsnprintf(p->mpStr, p->mCapacity, pFmt, tmp_args);
        bbVAEND(tmp_args);

        if ((size<0 || size>=(int)p->mCapacity) && (retried<4))
        {
            *p->mpStr = 0;
            p->mLen = 0;
            if (!bbStrBufEnsure(p, (p->mCapacity<<2)-1))
                return -1;
            retried++;
        }
        else
        {
            break;
        }
    }

    p->mLen = size;
    return size;
}

int bbStrBufVCatf(bbStrBuf* p, const bbCHAR* pFmt, bbVALIST args)
{
    int size;
    int retried = 0;

    for(;;)
    {
        bbVALIST tmp_args;
        bbVACOPY(tmp_args, args);
        size = bbVsnprintf(p->mpStr+p->mLen, p->mCapacity-p->mLen, pFmt, tmp_args);
        bbVAEND(tmp_args);

        if ((size<0 || size>=(int)(p->mCapacity-p->mLen)) && (retried<4))
        {
            if (!bbStrBufEnsure(p, (p->mCapacity<<2)-1))
                return -1;
            retried++;
        }
        else
        {
            break;
        }
    }

    p->mLen += size;
    return p->mLen;
}

int bbStrBufPrintf(bbStrBuf* p, const bbCHAR* pFmt, ...)
{
    int ret;
    bbVALIST args;
    bbVASTART(args, pFmt);
    ret = bbStrBufVPrintf(p, pFmt, args);
    bbVAEND(args);
    return ret;
}

int bbStrBufCatf(bbStrBuf* p, const bbCHAR* pFmt, ...)
{
    int ret;
    bbVALIST args;
    bbVASTART(args, pFmt);
    ret = bbStrBufVCatf(p, pFmt, args);
    bbVAEND(args);
    return ret;
}

int bbStrBufCatNumber(bbStrBuf* p, bbU64 n)
{
    return bbStrBufCatf(p, bbT("%I64u"), n);
}

