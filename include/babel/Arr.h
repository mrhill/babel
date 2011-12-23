#ifndef bbARR_H_
#define bbARR_H_
#include <stdlib.h> /* for NULL definition */
#include "mem.h"

/** Macro to declare a typed array.
    @param T    Type to declare array for
    @param name Name of the array class
    @param elsize Bytesize of an element,
                  this must be a constant value but not an expression such as \a sizeof(T).
*/
#define bbDECLAREARR(T,name,elsize) struct name##Rec {\
    T*     mpData;\
    bbU32  mSize;\
    inline bbUINT    GetElementSize() const { return elsize; }\
    inline void      Init() { bbArr_Init(this); }\
    inline void      Destroy() { bbArr_Clear(this); }\
    inline void      Clear() { bbArr_Clear(this); }\
    inline void      Attach(name##Rec* const pFrom) { bbArr_Attach(this, pFrom); }\
    inline T *       Detach() { return (T*)bbArr_Detach(this); }\
    inline bbUINT    GetSize() const { return mSize; }\
    inline bbERR     SetSize(const bbUINT size) { return bbArr_SetSize##elsize (this, size); }\
    inline bbERR     CopyFrom(const name##Rec* pFrom) { return bbArr_CopyFrom##elsize(this, (void*)pFrom); }\
    inline T *       Grow(const int relsize) { return (T*)bbArr_Grow##elsize (this, relsize); }\
    inline T *       Append(T const item) { T* p=Grow(1); if(p) *p=item; return p; }\
    inline void      Del(const bbUINT index) { bbMemMove(mpData+index, mpData+index+1, (mSize-index-1)*elsize); Grow(-1); }\
    inline T *       GetPtr() { return (T*)mpData; }\
    inline T const * GetPtr() const { return (T const*)mpData; }\
    inline T *       GetPtr(const bbUINT idx) { bbASSERT(idx<mSize); return (T*)mpData + idx; }\
    inline T const * GetPtr(const bbUINT idx) const { bbASSERT(idx<mSize); return (T const*)mpData + idx; }\
    inline T *       GetPtrLast() { return (T*)mpData + mSize - 1; }\
    inline T const * GetPtrLast() const { return (T const*)mpData + mSize - 1; }\
    inline T *       GetPtrEnd() { return (T*)mpData + mSize; }\
    inline T const * GetPtrEnd() const { return (T const*)mpData + mSize; }\
    inline T *       GetPtrCheck(const bbUINT idx) {  return (idx<mSize) ? (T*)mpData + idx : NULL; }\
    inline T const * GetPtrCheck(const bbUINT idx) const {  return (idx<mSize) ? (T const*)mpData + idx : NULL; }\
    inline T &       operator[](const bbUINT idx) { bbASSERT(idx<mSize); return ((T*)mpData)[idx]; }\
    inline T const & operator[](const bbUINT idx) const { bbASSERT(idx<mSize); return ((T const*)mpData)[idx]; }\
};\
struct name : name##Rec {\
    name() { bbArr_Init(this); } \
    ~name() { bbMemFreeNull( (void**)&mpData); } \
}

#define bbVEC_MAXSIZE 65535U

#define bbDECLAREVEC(T,name,elsize) struct name##Rec {\
    T*     mpData;\
    bbU16  mSize;\
    bbU16  mCapacity;\
    inline bbUINT    GetElementSize() const { return elsize; }\
    inline void      Init() { bbArr_Init(this);  }\
    inline void      Destroy() { bbArr_Clear(this); }\
    inline void      Clear() { bbArr_Clear(this); }\
    inline void      Attach(name##Rec* const pFrom) { bbArr_Attach(this, pFrom); }\
    inline T *       Detach() { return (T*)bbArr_Detach(this); }\
    inline bbERR     CopyFrom(const name##Rec* pFrom) { return bbVec_CopyFrom##elsize(this, (void*)pFrom); }\
    inline bbUINT    GetCapacity() const { return mCapacity; }\
    inline bbUINT    GetSize() const { return mSize; }\
    inline bbERR     SetSize(const bbUINT size) {\
        if (size>=mCapacity)\
            return SetCapacity(size);\
        mSize=(bbU16)size;\
        return bbEOK;\
    }\
    inline bbERR     SetCapacity(const bbUINT size) { return bbVec_SetCapacity##elsize(this, size); }\
    inline T *       Grow(const int relsize) {\
        bbUINT oldSize = mSize;\
        bbUINT newSize = (bbUINT)mSize + relsize;\
        if ((int)newSize >= (int)mCapacity) {\
            if (bbEOK != SetCapacity(newSize)) return NULL;\
        }\
        mSize=(bbU16)newSize;\
        return mpData + oldSize;\
    }\
    inline T *       Append(T const item) { T* p=Grow(1); if(p) *p=item; return p; }\
    inline T *       GetPtr() { return (T*)mpData; }\
    inline T const * GetPtr() const { return (T const*)mpData; }\
    inline T *       GetPtr(const bbUINT idx) { bbASSERT(idx<mSize); return (T*)mpData + idx; }\
    inline T const * GetPtr(const bbUINT idx) const { bbASSERT(idx<mSize); return (T const*)mpData + idx; }\
    inline T *       GetPtrLast() { return (T*)mpData + mSize - 1; }\
    inline T const * GetPtrLast() const { return (T const*)mpData + mSize - 1; }\
    inline T *       GetPtrEnd() { return (T*)mpData + mSize; }\
    inline T const * GetPtrEnd() const { return (T const*)mpData + mSize; }\
    inline T *       GetPtrCheck(const bbUINT idx) {  return (idx<mSize) ? (T*)mpData + idx : NULL; }\
    inline T const * GetPtrCheck(const bbUINT idx) const {  return (idx<mSize) ? (T const*)mpData + idx : NULL; }\
    inline T &       operator[](const bbUINT idx) { bbASSERT(idx<mSize); return ((T*)mpData)[idx]; }\
    inline T const & operator[](const bbUINT idx) const { bbASSERT(idx<mSize); return ((T const*)mpData)[idx]; }\
};\
struct name : name##Rec {\
    name() { Init(); } \
    ~name() { Destroy(); } \
}


/** Macro to declare a typed pointer array
    See bbDECLAREARR for parameter descriptions
*/
#if bbSIZEOF_UPTR == 8
#define bbDECLAREARRPTR(T,name) bbDECLAREARR(T,name,8)
#define bbDECLAREVECPTR(T,name) bbDECLAREVEC(T,name,8)
#else
#define bbDECLAREARRPTR(T,name) bbDECLAREARR(T,name,4)
#define bbDECLAREVECPTR(T,name) bbDECLAREVEC(T,name,4)
#endif

void  bbArr_Init(void* const p);
void  bbArr_Attach(void* const p, void* const pFrom);
void* bbArr_Detach(void* const p);
void  bbArr_Clear(void* const p);
bbERR bbArr_SetSize1(void* const p, const bbUINT size);
bbERR bbArr_SetSize2(void* const p, const bbUINT size);
bbERR bbArr_SetSize4(void* const p, const bbUINT size);
bbERR bbArr_SetSize8(void* const p, const bbUINT size);
bbERR bbArr_SetSize12(void* const p, const bbUINT size);
bbERR bbArr_SetSize16(void* const p, const bbUINT size);
bbERR bbArr_SetSize24(void* const p, const bbUINT size);
bbERR bbArr_SetSize48(void* const p, const bbUINT size);
void* bbArr_Grow1(void* const p, const int relsize);
void* bbArr_Grow2(void* const p, const int relsize);
void* bbArr_Grow4(void* const p, const int relsize);
void* bbArr_Grow8(void* const p, const int relsize);
void* bbArr_Grow12(void* const p, const int relsize);
void* bbArr_Grow16(void* const p, const int relsize);
void* bbArr_Grow24(void* const p, const int relsize);
void* bbArr_Grow48(void* const p, const int relsize);
bbERR bbArr_CopyFrom1(void* const p, void* const pFrom);
bbERR bbArr_CopyFrom2(void* const p, void* const pFrom);
bbERR bbArr_CopyFrom4(void* const p, void* const pFrom);
bbERR bbArr_CopyFrom8(void* const p, void* const pFrom);
bbERR bbArr_CopyFrom12(void* const p, void* const pFrom);
bbERR bbArr_CopyFrom16(void* const p, void* const pFrom);
bbERR bbArr_CopyFrom24(void* const p, void* const pFrom);
bbERR bbArr_CopyFrom48(void* const p, void* const pFrom);
bbERR bbArrObj_SetSize(void* const p, const bbUINT size);
bbERR bbArrArr_SetSize(void* const p, const bbUINT size);
void  bbArrArr_Clear(void* const p);

bbERR bbVec_SetCapacity1(void* const p, const bbUINT size);
bbERR bbVec_SetCapacity2(void* const p, const bbUINT size);
bbERR bbVec_SetCapacity4(void* const p, const bbUINT size);
bbERR bbVec_SetCapacity8(void* const p, const bbUINT size);
bbERR bbVec_CopyFrom1(void* const p, void* const pFrom);
bbERR bbVec_CopyFrom2(void* const p, void* const pFrom);
bbERR bbVec_CopyFrom4(void* const p, void* const pFrom);
bbERR bbVec_CopyFrom8(void* const p, void* const pFrom);

bbDECLAREARR(bbU8, bbArrU8, 1);
bbDECLAREARR(bbU16, bbArrU16, 2);
bbDECLAREARR(bbU32, bbArrU32, 4);
#if bbUSE_LONGLONG==1
bbDECLAREARR(bbU64, bbArrU64, 8);
bbDECLAREVEC(bbU64, bbVecU64, 8);
#endif
bbDECLAREARR(double, bbArrF64, 8);
bbDECLAREARRPTR(bbU8*, bbArrPU8);
bbDECLAREARRPTR(bbU16*, bbArrPU16);
bbDECLAREARRPTR(bbU32*, bbArrPU32);
bbDECLAREARRPTR(bbCHAR*, bbArrPCHAR);
bbDECLAREARR(bbArrPU8, bbArrArrPU8, 8);

#if bbOS == bbOS_WIN64
#error todo: check int size
bbDECLAREARR(bbHEAPH, bbArrHEAPH, 8);
#elif bbOS == bbOS_PALMOS
bbDECLAREARR(bbHEAPH, bbArrHEAPH, 2);
#else
bbDECLAREARR(bbHEAPH, bbArrHEAPH, 4);
#endif

#if bbSIZEOF_CHAR == 1
bbDECLAREARR(bbCHAR, bbArrCHAR, 1);
#elif bbSIZEOF_CHAR == 2
bbDECLAREARR(bbCHAR, bbArrCHAR, 2);
#elif bbSIZEOF_CHAR == 4
bbDECLAREARR(bbCHAR, bbArrCHAR, 4);
#else
#error Cannot detect sizeof(bbCHAR)
#endif

bbDECLAREVEC(bbU8, bbVecU8, 1);

/** Array of strings.
    This class maintains an array of 0-terminated strings,
    each string is held in a heap block.
*/
struct bbArrStrRec
{
    bbArrPCHARRec mIndex; //!< Array of pointers to 0-terminated strings forming the string array.

    /** Construct 0-sized array. */
    inline void Init() { mIndex.Init(); }

    /** Destruct array and free memory. */
    inline void Destroy() { Clear(); }

    /** Get number of strings in array.
        @return Number of elements
    */
    inline bbUINT GetSize() const { return mIndex.mSize; }

    /** Get pointer to string index.
        @return Pointer to string pointer array
    */
    inline bbCHAR* const* GetPtr() const { return mIndex.GetPtr(); }

    inline bbCHAR** GetPtr(bbUINT idx) { return mIndex.GetPtr(idx); }

    /** Set array to given size.
        If the array is shrunken, memory for clipped objects is freed.
        If the array is enlarged, NULL pointers will be appended.
        @param size New size in number of elements.
    */
    inline bbERR SetSize(const bbUINT size) { return bbArrObj_SetSize(this, size); }

    /** Grow or shrink array by given amount.
        If the array is shrunken, memory for clipped objects is freed.
        If the array is enlarged, NULL pointers will be appended.
        @param relsize Relative size in number of elements.
        @return NULL on failure, !=NULL on success, in case of enlargement pointer to
                the start of the newly allocated elements is returned
    */
    bbCHAR** Grow(const int relsize);

    /** Clear array and set size to 0. */
    void Clear();

    /** Append a string to end of list.
        The string will be copied to a new heap block, and 0-terminated.
        @param pStr 0-terminated string to append
        @return bbEOK on success, or value of #bbgErr on failure
    */
    bbERR Add(const bbCHAR* pStr);

    /** Insert copy of string to given position in list.
        The string will be copied to a new heap block, and 0-terminated.
        @param pStr 0-terminated string to append
        @param idx Index to insert string at, can be -1 for end of list.
        @return bbEOK on success, or value of #bbgErr on failure
    */
    bbERR Insert(const bbCHAR* pStr, bbUINT idx);

    /** Attach a string to given position in list.
        The string must be in a heap block, control will be passed over to bbArrStr.
        @param pStr 0-terminated string to append
        @param idx Index to insert string at, can be -1 for end of list.
        @return bbEOK on success, or value of #bbgErr on failure
    */
    bbERR Attach(bbCHAR* pStr, bbUINT idx = (bbUINT)-1);

    inline bbCHAR* operator[](bbUINT idx) { return mIndex[idx]; }
    inline const bbCHAR* operator[](bbUINT idx) const { return mIndex[idx]; }

    bbERR CopyFrom(const bbArrStrRec* pFrom);
};

struct bbArrStr : bbArrStrRec
{
    bbArrStr() { Init(); }
    ~bbArrStr() { Destroy(); }
};

#endif /* bbARR_H_ */
