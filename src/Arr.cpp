#include "str.h"
#include "Arr.h"
#include "fixmath.h"

struct bbsArrBase
{
    void*  mpData;
    bbU32  mSize;
};

struct bbsVecBase
{
    void*  mpData;
    bbU16  mSize;
    bbU16  mCapacity;
};

void bbArr_Init(void* const p)
{ 
    ((struct bbsArrBase*)p)->mpData = NULL;
    ((struct bbsArrBase*)p)->mSize = 0;
}

void bbArr_Attach(void* const pTo, void* const pFrom)
{
    bbMemFree( ((struct bbsArrBase*)pTo)->mpData );
    *(struct bbsArrBase*)pTo = *(struct bbsArrBase*)pFrom;
}

void* bbArr_Detach(void* const p)
{
    void* const pTmp = ((struct bbsArrBase*)p)->mpData;
    ((struct bbsArrBase*)p)->mpData = NULL;
    ((struct bbsArrBase*)p)->mSize = 0;
    return pTmp;
}

void bbArr_Clear(void* const p)
{
    bbMemFreeNull( &((struct bbsArrBase*)p)->mpData);
    ((struct bbsArrBase*)p)->mSize = 0;
}

template<unsigned int N> static bbERR bbArr_SetSize(void* const p, const bbUINT size)
{
    if (bbEOK != bbMemRealloc(size * N, &((struct bbsArrBase*)p)->mpData))
        return bbELAST;

    ((struct bbsArrBase*)p)->mSize = size;
    return bbEOK;
}

template<unsigned int N> static void* bbArr_Grow(void* const p, const int relsize)
{
    const bbU32 oldSize = ((struct bbsArrBase*)p)->mSize;
    const bbU32 newSize = oldSize + relsize;

    if (bbEOK != bbMemRealloc(newSize * N, &((struct bbsArrBase*)p)->mpData))
        return NULL;

    ((struct bbsArrBase*)p)->mSize = newSize;        

    return (bbU8*)((struct bbsArrBase*)p)->mpData + oldSize * N;
}

template<unsigned int N> static bbERR bbArr_CopyFrom(void* const p, void* const pFrom)
{
    bbUINT const size = ((bbsArrBase*)pFrom)->mSize;

    if (bbEOK != bbArr_SetSize<N>(p, size))
        return bbELAST;

    bbMemMove(((bbsArrBase*)p)->mpData, ((bbsArrBase*)pFrom)->mpData, size*N);
    return bbEOK;
}

bbERR bbArr_SetSize1(void* const p, const bbUINT size) { return bbArr_SetSize<1>(p, size); }
bbERR bbArr_SetSize2(void* const p, const bbUINT size) { return bbArr_SetSize<2>(p, size); }
bbERR bbArr_SetSize4(void* const p, const bbUINT size) { return bbArr_SetSize<4>(p, size); }
bbERR bbArr_SetSize8(void* const p, const bbUINT size) { return bbArr_SetSize<8>(p, size); }
bbERR bbArr_SetSize12(void* const p, const bbUINT size) { return bbArr_SetSize<12>(p, size); }
bbERR bbArr_SetSize16(void* const p, const bbUINT size) { return bbArr_SetSize<16>(p, size); }
bbERR bbArr_SetSize24(void* const p, const bbUINT size) { return bbArr_SetSize<24>(p, size); }
bbERR bbArr_SetSize48(void* const p, const bbUINT size) { return bbArr_SetSize<48>(p, size); }

void* bbArr_Grow1(void* const p, const int relsize) { return bbArr_Grow<1>(p, relsize); }
void* bbArr_Grow2(void* const p, const int relsize) { return bbArr_Grow<2>(p, relsize); }
void* bbArr_Grow4(void* const p, const int relsize) { return bbArr_Grow<4>(p, relsize); }
void* bbArr_Grow8(void* const p, const int relsize) { return bbArr_Grow<8>(p, relsize); }
void* bbArr_Grow12(void* const p, const int relsize) { return bbArr_Grow<12>(p, relsize); }
void* bbArr_Grow16(void* const p, const int relsize) { return bbArr_Grow<16>(p, relsize); }
void* bbArr_Grow24(void* const p, const int relsize) { return bbArr_Grow<24>(p, relsize); }
void* bbArr_Grow48(void* const p, const int relsize) { return bbArr_Grow<48>(p, relsize); }

bbERR bbArr_CopyFrom1(void* const p, void* const pFrom) { return bbArr_CopyFrom<1>(p, pFrom);}
bbERR bbArr_CopyFrom2(void* const p, void* const pFrom) { return bbArr_CopyFrom<2>(p, pFrom);}
bbERR bbArr_CopyFrom4(void* const p, void* const pFrom) { return bbArr_CopyFrom<4>(p, pFrom);}
bbERR bbArr_CopyFrom8(void* const p, void* const pFrom) { return bbArr_CopyFrom<8>(p, pFrom);}
bbERR bbArr_CopyFrom12(void* const p, void* const pFrom) { return bbArr_CopyFrom<12>(p, pFrom);}
bbERR bbArr_CopyFrom16(void* const p, void* const pFrom) { return bbArr_CopyFrom<16>(p, pFrom);}
bbERR bbArr_CopyFrom24(void* const p, void* const pFrom) { return bbArr_CopyFrom<24>(p, pFrom);}
bbERR bbArr_CopyFrom48(void* const p, void* const pFrom) { return bbArr_CopyFrom<48>(p, pFrom);}

template<unsigned int N> static bbERR bbVec_SetCapacity(void* const p, const bbUINT size)
{
    if (size >= (bbVEC_MAXSIZE+1))
        return bbErrSet(bbEFULL);

    bbU32 capacity = size ? (2UL << bbGetTopBit(size)) : 0;
    if (capacity >= (bbVEC_MAXSIZE+1))
        capacity = bbVEC_MAXSIZE;

    if (bbEOK != bbMemRealloc(capacity * N, (void**)&((bbsVecBase*)p)->mpData))
        return bbELAST;

    ((bbsVecBase*)p)->mSize = (bbU16)size;
    ((bbsVecBase*)p)->mCapacity = (bbU16)capacity;

    return bbEOK;
}

template<unsigned int N> static bbERR bbVec_CopyFrom(void* const p, void* const pFrom)
{
    bbUINT const size = ((bbsVecBase*)pFrom)->mSize;

    if (size>=((bbsVecBase*)p)->mCapacity)
    {
        if (bbEOK != bbVec_SetCapacity<N>(p, size))
            return bbELAST;
    }
    ((bbsVecBase*)p)->mSize = (bbU16)size;
    bbMemMove(((bbsVecBase*)p)->mpData, ((bbsVecBase*)pFrom)->mpData, size*N);
    return bbEOK;
}

bbERR bbVec_SetCapacity4(void* const p, const bbUINT size) { return bbVec_SetCapacity<4>(p, size); }
bbERR bbVec_SetCapacity8(void* const p, const bbUINT size) { return bbVec_SetCapacity<8>(p, size); }
bbERR bbVec_CopyFrom4(void* const p, void* const pFrom) { return bbVec_CopyFrom<4>(p, pFrom); }
bbERR bbVec_CopyFrom8(void* const p, void* const pFrom) { return bbVec_CopyFrom<8>(p, pFrom); }

struct bbsArrObjBase
{
    void** mpData;
    bbU32  mSize;
};

bbERR bbArrObj_SetSize(void* const p, const bbUINT size)
{
    bbUINT cursize = ((struct bbsArrObjBase*)p)->mSize;

    while (cursize > size)
    {
        void* pObj = ((struct bbsArrObjBase*)p)->mpData[--cursize];
        if (pObj) bbMemFree(pObj);
    }

    const bbERR err = bbMemRealloc(size * sizeof(void*), (void**)&((struct bbsArrObjBase*)p)->mpData);
    if (err != bbEOK) return err;

    const int grow = (int)size - (int)((struct bbsArrObjBase*)p)->mSize;

    if (grow > 0)
    {
        bbMemClear( ((struct bbsArrObjBase*)p)->mpData + ((struct bbsArrObjBase*)p)->mSize, grow * sizeof(void*));
    }

    ((struct bbsArrObjBase*)p)->mSize = size;

    return bbEOK;
}

bbCHAR** bbArrStrRec::Grow(const int relsize)
{
    bbUINT oldSize = mIndex.mSize;
    if (bbEOK != SetSize(mIndex.mSize + relsize))
        return NULL;
    return mIndex.GetPtr(oldSize);
}

void bbArrStrRec::Clear()
{
    bbArrObj_SetSize(this, 0);
}

bbERR bbArrStrRec::Add( const bbCHAR* const pStr)
{
    bbCHAR* const pCopy = bbStrDup(pStr);
    if (!pCopy) 
        return bbELAST;

    bbCHAR** pLast = mIndex.Grow(1);

    if (!pLast)
    {
        bbMemFree(pCopy);
        return bbErrSet(bbENOMEM);
    }

    *pLast = pCopy;

    return bbEOK;
}

bbERR bbArrStrRec::Attach(bbCHAR* const pStr)
{
    if (!mIndex.Grow(1))
        return bbELAST;
    *mIndex.GetPtrLast() = pStr;
    return bbEOK;
}

bbERR bbArrStrRec::CopyFrom(const bbArrStrRec* pFrom)
{
    bbERR err = SetSize(pFrom->GetSize());
    if (err == bbEOK)
    {
        for (bbUINT i=0; i<GetSize(); i++)
        {
            const bbUINT stringsize = (bbStrLen(pFrom->mIndex[i])+1) * sizeof(bbCHAR);
            err = bbMemRealloc(stringsize, (void**)mIndex.GetPtr(i));
            if (err != bbEOK)
                return err;
            
            bbMemMove(mIndex[i], pFrom->mIndex[i], stringsize);
        }
    }
    return err;
}

struct bbsArrArrBase
{
    struct bbsArrBase* mpData;
    bbUINT mSize;
};

bbERR bbArrArr_SetSize(void* const p, const bbUINT size)
{
    bbUINT cursize = ((struct bbsArrArrBase*)p)->mSize;

    while (cursize > size)
    {
        void* pObj = ((struct bbsArrArrBase*)p)->mpData[--cursize].mpData;
        if (pObj) bbMemFree(pObj);
    }

    const bbERR err = bbMemRealloc(size * sizeof(struct bbsArrBase), (void**)&((struct bbsArrArrBase*)p)->mpData);
    if (err != bbEOK) return err;

    const int grow = (int)size - (int)((struct bbsArrArrBase*)p)->mSize;

    if (grow > 0)
    {
        bbMemClear( ((struct bbsArrArrBase*)p)->mpData + ((struct bbsArrArrBase*)p)->mSize, grow * sizeof(struct bbsArrBase));
    }

    ((struct bbsArrArrBase*)p)->mSize = size;

    return bbEOK;
}

void bbArrArr_Clear(void* const p)
{
    bbArrArr_SetSize(p, 0);
}

