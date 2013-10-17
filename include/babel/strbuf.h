#ifndef bbSTRBUF_H_
#define bbSTRBUF_H_

#include "defs.h"
#include "str.h"

#define bbSTRBUF_MINSIZE 64

struct bbStrBufRec
{
    bbCHAR* mpStr;
    bbUINT  mLen;
    bbUINT  mCapacity;
    bbCHAR  mBuf[bbSTRBUF_MINSIZE];
};

#ifdef  __cplusplus
extern "C" {
#endif

struct bbStrBuf;

#ifndef  __cplusplus
typedef struct bbStrBufRec bbStrBuf;
#endif

/** Construct 0 length and 0-terminated string, always succeeds.
    @param pStrBuf String buffer object to initialize
*/
void bbStrBufInit(bbStrBuf* pStrBuf);

/** Construct string buffer by copying referenced string.
    If memory allocation fails, an empty string is constructed.
    Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
    @param pStrBuf String buffer object to initialize
*/
void bbStrBufInitStr(bbStrBuf* pStrBuf, const bbCHAR* pStr);

#if bbSIZEOF_CHAR != 1
void bbStrBufInitCStr(bbStrBuf* pStrBuf, const char* pStr);
#else
#define bbStrBufInitCStr(p, s) bbStrBufInitStr(p, s)
#endif

/** Construct string buffer by copying at most \a len chacters from referenced string.
    If memory allocation fails, an empty string is constructed.
    Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
    @param pStrBuf String buffer object to initialize
    @param pStr 0-terminated string to copy
    @param len Maximum number of characters to copy, not counting the 0-terminator
*/
void bbStrBufInitStrN(bbStrBuf* pStrBuf, const bbCHAR* pStr, bbUINT len);

/** Construct string and allocate length to given number of characters.
    Capacity will be set to at least len + 1 (0-terminator) characters.
    String contents will be uninitialized.
    @param pStrBuf String buffer object to initialize
    @param strlen Number of characters to ensure.
    @return Pointer to start of string, or NULL on failure.
*/
bbCHAR* bbStrBufInitLen(bbStrBuf* pStrBuf, bbUINT len);

/** Destroy string.
    @param pStrBuf String buffer object to destroy
*/
#define bbStrBufDestroy(pStrBuf) bbStrBufClear(pStrBuf)

/** Clear string object to a 0-size string, and reset capacity.
    @param pStrBuf String buffer object to clear
*/
void bbStrBufClear(bbStrBuf* pStrBuf);

/** Attach heap buffer to string, replacing any existing string.
    @param pStr Heap buffer containing 0-terminated string
    @param capacity Size of heap buffer in number of characters
    @param len String length without 0-terminator, can be -1 to detect string length
*/
void bbStrBufAttach(bbStrBuf* pStrBuf, bbCHAR* const pStr, bbUINT capacity, int len);

/** Detach string and clear buffer object.
    The string object will be cleared regardless of success or failure.
    @return Detached string in heap block, or NULL on failure. Must be freed externally.
*/
bbCHAR* bbStrBufDetach(bbStrBuf* pStrBuf);

/** Assign string buffer by copying from referenced string.
    Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
    The difference to bbStrBufInitStr() is that this function will first clear the string object.
    @param pStr 0-terminated string to copy, can be NULL to produce an empty string.
    @return Pointer to start of inserted string in buffer, or NULL on failure
*/
bbCHAR* bbStrBufAssign(bbStrBuf* pStrBuf, const bbCHAR* pStr);

/** Assign string buffer by copying at most \a len chacters from referenced string.
    If the source string is less than \a len characters, it will be copied only
    until its 0-terminator. At any case the buffer capacity will be set to hold \a len characters.
    Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
    @param pStr 0-terminated string to copy
    @param len Maximum number of characters to copy, not counting the 0-terminator
*/
bbCHAR* bbStrBufAssignN(bbStrBuf* pStrBuf, const bbCHAR* pStr, bbUINT len);

/** Ensure capacity to append or insert given number of characters + 0-terminator.
    @param strlen Number of characters to ensure, not including 0-terminator
    @return Pointer to end of string, or NULL on failure.
*/
bbCHAR* bbStrBufEnsure(bbStrBuf* pStrBuf, bbUINT strlen);

/** Set string length to given number of characters.
    Capacity will be set to at least len + 1 (0-terminator) characters.
    String contents will be uninitialized.
    @param pStrBuf String buffer object
    @param strlen Number of characters to ensure.
    @return Pointer to start of string, or NULL on failure.
*/
bbCHAR* bbStrBufSetLen(bbStrBuf* pStrBuf, bbUINT strlen);

/** Get pointer to string buffer.
    @return (bbCHAR*) Pointer to 0-terminated string.
*/
#define bbStrBufGetPtr(pStrBuf) ((pStrBuf)->mpStr)

#define bbStrBufGetLen(pStrBuf) ((pStrBuf)->mLen)

#define bbStrBufIsEmpty(pStrBuf) (!(pStrBuf)->mLen)

#define bbStrBufGetCapacity(pStrBuf) ((pStrBuf)->mCapacity)

/** Concatenate string to end of buffer.
    @param pStr String to append, can be NULL
    @return Pointer to start of inserted string in buffer, or NULL on failure
*/
bbCHAR* bbStrBufCat(bbStrBuf* pStrBuf, const bbCHAR* pStr);

/** Concatenate string by copying at most \a len chacters from referenced string.
    If the source string is less than \a len characters, it will be copied only
    until its 0-terminator. At any case the buffer capacity will be set to hold \a len characters.
    Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
    @param pStr 0-terminated string to copy
    @param len Maximum number of characters to copy, not counting the 0-terminator
*/
bbCHAR* bbStrBufCatN(bbStrBuf* pStrBuf, const bbCHAR* pStr, bbUINT len);

#if bbSIZEOF_CHAR != 1
/** Concatenate string to end of buffer.
    @param pStr String to append
    @return Pointer to start of inserted string in buffer, or NULL on failure
*/
bbCHAR* bbStrBufCatCStr(bbStrBuf* pStrBuf, const char* pStr);
#else
#define bbStrBufCatCStr(p, s) bbStrBufCat(p, s)
#endif

void bbStrBufCatCP(bbStrBuf* pStrBuf, bbCHARCP cp);
void bbStrBufCatChar(bbStrBuf* pStrBuf, char c);

bbCHAR* bbStrBufPrintHex(bbStrBuf* pStrBuf, const bbU8* pData, bbUINT size);

/** Concatenate number as unsigned decimal.
    @return New length of complete string
*/
int bbStrBufCatNumber(bbStrBuf* pStrBuf, bbU64 n);

int bbStrBufVPrintf(bbStrBuf* pStrBuf, const bbCHAR* pFmt, bbVALIST args);
int bbStrBufVCatf(bbStrBuf* pStrBuf, const bbCHAR* pFmt, bbVALIST args);

/** Print formatted string.
    Existing string contents will be overwritten.
    @return New length of string
*/
int bbStrBufPrintf(bbStrBuf* pStrBuf, const bbCHAR*, ...);

/** Concatenate formatted string to end of buffer.
    @return New length of complete string
*/
int bbStrBufCatf(bbStrBuf* pStrBuf, const bbCHAR*, ...);


#ifdef  __cplusplus
struct bbStrBuf : bbStrBufRec
{
    bbStrBuf() { bbStrBufInit(this); }
    bbStrBuf(const bbCHAR* pStr) { bbStrBufInitStr(this, pStr); }
    bbStrBuf(const bbCHAR* pStr, bbUINT len) { bbStrBufInitStrN(this, pStr, len); }
    #if bbSIZEOF_CHAR != 1
    bbStrBuf(const char* pStr) { bbStrBufInitCStr(this, pStr); }
    #endif
    ~bbStrBuf() { bbStrBufClear(this); }
    inline void Clear() { bbStrBufClear(this); }
    inline void Attach(bbCHAR* const pStr, bbUINT capacity, int len) { bbStrBufAttach(this, pStr, capacity, len); }
    inline bbCHAR* Detach() { return bbStrBufDetach(this); }
    inline bbCHAR* Assign(const bbCHAR* const pStr) { return bbStrBufAssign(this, pStr); }
    inline bbCHAR* Assign(const bbCHAR* pStr, bbUINT len) { return bbStrBufAssignN(this, pStr, len); }
    inline bbCHAR* Ensure(bbUINT strlen) { return bbStrBufEnsure(this, strlen); }
    inline bbCHAR* SetLen(bbUINT strlen) { return bbStrBufSetLen(this, strlen); }
    inline bbCHAR* GetPtr() { return mpStr; }
    inline bbUINT GetLen() const { return mLen; }
    inline bbUINT size() const { return mLen; }
    inline bool empty() const { return mLen==0; }
    inline bbUINT GetCapacity() const { return mCapacity; }

    inline bbCHAR* Cat(const bbCHAR* pStr) { return bbStrBufCat(this, pStr); }
    inline bbCHAR* Cat(const bbCHAR* pStr, bbUINT len) { return bbStrBufCatN(this, pStr, len); }
    #if bbSIZEOF_CHAR != 1
    inline bbCHAR* Cat(const char* pStr) { return bbStrBufCatCStr(this, pStr); }
    #endif

    inline void CatCP(bbCHARCP cp) { bbStrBufCatCP(this, cp); }
    inline void CatChar(char c) { bbStrBufCatChar(this, c); }

    inline bbStrBuf& operator=(const bbCHAR* const pStr) { Assign(pStr); return *this; }

    /** Concatenate string to end of buffer, noop on failure. */
    inline bbStrBuf& operator+=(const bbCHAR* pStr) { Cat(pStr); return *this; }
    inline bbStrBuf& operator+=(bbCHARCP cp) { CatCP(cp); return *this; }

    #if bbSIZEOF_CHAR != 1
    inline bbStrBuf& operator+=(const char* pStr) { Cat(pStr); return *this; }
    inline bbStrBuf& operator+=(char cp) { return operator+=((bbCHARCP)cp); }
    #endif

    inline bbCHAR operator[](int i) const { return mpStr[i]; }
    inline operator const bbCHAR*() const { return mpStr; }

    inline bbCHAR* PrintHex(const bbU8* pData, bbUINT size) { return bbStrBufPrintHex(this, pData, size); }
    inline int CatNumber(bbU64 n) { return bbStrBufCatNumber(this, n); }

    inline int VPrintf(const bbCHAR* pFmt, bbVALIST args) { return bbStrBufVPrintf(this, pFmt, args); }
    inline int VCatf(const bbCHAR* pFmt, bbVALIST args) { return bbStrBufVCatf(this, pFmt, args); }
    int Printf(const bbCHAR*, ...);
    int Catf(const bbCHAR*, ...);
};

}
#endif

#endif /* bbSTRBUF_H_ */


