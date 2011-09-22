#include "StrBuf.h"
#include "mem.h"
#include "str.h"

bbStrBuf::bbStrBuf()
{
    mpStr = mBuf;
    Clear();
}

bbStrBuf::bbStrBuf(const bbCHAR* pStr)
{
    mpStr = mBuf;
    Clear();
    Cat(pStr);
}

#if bbSIZEOF_CHAR != 1
bbStrBuf::bbStrBuf(const char* pStr)
{
    mpStr = mBuf;
    Clear();
    Cat(pStr);
}
#endif

bbStrBuf::bbStrBuf(const bbCHAR* pStr, bbUINT len)
{
    mpStr = mBuf;
    Assign(pStr, len);
}

bbCHAR* bbStrBuf::Assign(const bbCHAR* pStr, bbUINT len)
{
    Clear();

    bbCHAR* pDst = Ensure(len);
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
        mLen = copied;
    }

    return pDst;
}

void bbStrBuf::Clear()
{
    if (mpStr != mBuf)
        bbMemFree(mpStr);

    mpStr = mBuf;
    mLen = 0;
    mBuf[0] = 0;
    mCapacity = bbARRSIZE(mBuf);    
}

bbCHAR* bbStrBuf::Ensure(bbUINT str_len)
{
    str_len++; // 0-term

    if ((mCapacity - mLen) < str_len)
    {
        bbUINT capacity = mCapacity;
        do { capacity<<=1; } while ((capacity - mLen) < str_len);

        if (mpStr == mBuf)
        {
            bbCHAR* pNew = (bbCHAR*)bbMemAlloc(capacity * sizeof(bbCHAR));
            if (!pNew)
                return NULL;
            /* FIXME, is following assignment correct?*/
            bbStrCpy((mpStr = pNew), mBuf);
        }
        else
        {
            if (bbEOK != bbMemRealloc(capacity * sizeof(bbCHAR), (void**)&mpStr))
                return NULL;
        }

        mCapacity = capacity;
    }

    return mpStr + mLen;
}

void bbStrBuf::Attach(bbCHAR* const pStr, bbUINT len)
{
    Clear();
    if (pStr)
    {
        mpStr = pStr;
        mLen = mCapacity = len;
    }
}

bbCHAR* bbStrBuf::Detach()
{
    bbCHAR* pStr;

    if (mpStr == mBuf)
        pStr = bbStrDup(mBuf);
    else
        pStr = mpStr;
        
    mpStr = mBuf;
    Clear();

    return pStr;
}

bbCHAR* bbStrBuf::Cat(const bbCHAR* const pStr)
{
    bbCHAR* pInsert = mpStr + mLen;
    if (pStr)
    {
        bbUINT str_len = bbStrLen(pStr);
        pInsert = Ensure(str_len);
        if (pInsert)
        {
            mLen+=str_len;
            bbStrCpy(pInsert, pStr);
        }
    }
    return pInsert;
}

#if bbSIZEOF_CHAR != 1
bbCHAR* bbStrBuf::Cat(const char* pStr)
{
    bbCHAR* pInsert = mpStr + mLen;
    if (pStr)
    {
        bbUINT strLen = bbStrLen_sbc(pStr);

        pInsert = Ensure(strLen);
        if (pInsert)
        {
            mLen+=strLen;
            bbCHAR* pDst = pInsert;
            while((*pDst++=*pStr++)!=0) {}
        }
    }
    return pInsert;
}
#endif

bbStrBuf& bbStrBuf::operator+=(const bbCHARCP cp)
{
    bbUINT str_len = bbCP_LENGTH(cp);
    bbCHAR* pStr = Ensure(str_len);
    if (pStr)
    {
        mLen+=str_len;
        bbCP_APPEND_PTR(pStr, cp);
        *pStr=0;
    }
    return *this;
}

bbCHAR* bbStrBuf::PrintHex(const bbU8* pData, bbUINT size)
{
    bbCHAR* const pInsert = Ensure(size*5);
    if (pInsert)
    {
        bbCHAR* pStr = pInsert;
        const bbU8* const pDataEnd = pData + size;
        while(pData < pDataEnd)
        {
            pStr += bbSprintf(pStr, bbT("0x%02X "), *pData++);
        }
        *(pStr-1) = 0;
        mLen += size*5-1;
    }
    return pInsert;
}

