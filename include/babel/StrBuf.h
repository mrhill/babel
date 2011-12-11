#ifndef bbSTRBUF_H_
#define bbSTRBUF_H_

#include "defs.h"
#include "str.h"

#define bbSTRBUF_MINSIZE 64

class bbStrBuf
{
    bbCHAR* mpStr;
    bbUINT  mLen;
    bbUINT  mCapacity;
    bbCHAR  mBuf[bbSTRBUF_MINSIZE];

    int VPrintf(const bbCHAR* pFmt, bbVALIST args);
    int VCatf(const bbCHAR* pFmt, bbVALIST args);

public:
    /** Construct 0 length and 0-terminated string.
        Always succeeds.
    */
    bbStrBuf();

    /** Construct string buffer by copying referenced string.
        If memory allocation fails, an empty string is constructed.
        Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
    */
    bbStrBuf(const bbCHAR* pStr);

    #if bbSIZEOF_CHAR != 1
    bbStrBuf(const char* pStr);
    #endif

    /** Construct string buffer by copying at most \a len chacters from referenced string.
        If memory allocation fails, an empty string is constructed.
        Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
        @param pStr 0-terminated string to copy
        @param len Maximum number of characters to copy, not counting the 0-terminator
    */
    bbStrBuf(const bbCHAR* pStr, bbUINT len);

    ~bbStrBuf() { Clear(); }

    /** Attach heap buffer to string, replacing any existing string.
        @param pStr Heap buffer containing 0-terminated string
        @param capacity Size of heap buffer in number of characters
        @param len String length without 0-terminator, can be -1 to detect string length
    */
    void Attach(bbCHAR* const pStr, bbUINT capacity, int len);

    /** Detach string and clear buffer object.
        The string object will be cleared regardless of success or failure.
        @return Detached string in heap block, or NULL on failure. Must be freed externally.
    */
    bbCHAR* Detach();

    /** Ensure capacity to append or insert given number of characters.
        @param strlen Number of characters to ensure.
        @return Pointer to end of string, or NULL on failure.
    */
    bbCHAR* Ensure(bbUINT strlen);

    /** Clear string and reset capacity. */
    void Clear();

    /** Get pointer to string buffer.
        @return Pointer to 0-terminated string.
    */
    inline bbCHAR* GetPtr() { return mpStr; }

    inline bbUINT GetLen() const { return mLen; }
    inline bool empty() const { return mLen==0; }

    inline bbUINT GetCapacity() const { return mCapacity; }

    /** @internal */
    inline void AddSize(bbUINT const increase) { mLen+=increase; }

    /** Assign string buffer by copying from referenced string.
        Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
        @param pStr 0-terminated string to copy, can be NULL to produce an empty string.
    */
    bbCHAR* Assign(const bbCHAR* const pStr);

    /** Assign string buffer by copying at most \a len chacters from referenced string.
        If the source string is less than \a len characters, it will be copied only
        until its 0-terminator. At any case the buffer capacity will be set to hold \a len characters.
        Guaranted to succeed, if referenced string is less than bbSTRBUF_MINSIZE characters.
        @param pStr 0-terminated string to copy
        @param len Maximum number of characters to copy, not counting the 0-terminator
    */
    bbCHAR* Assign(const bbCHAR* pStr, bbUINT len);

    /** Concatenate string to end of buffer.
        @param pStr String to append
        @return Pointer to start of inserted string in buffer, or NULL on failure 
    */
    bbCHAR* Cat(const bbCHAR* const pStr);

    #if bbSIZEOF_CHAR != 1
    /** Concatenate string to end of buffer.
        @param pStr String to append
        @return Pointer to start of inserted string in buffer, or NULL on failure 
    */
    bbCHAR* Cat(const char* pStr);
    #endif

    inline bbStrBuf& operator=(const bbCHAR* const pStr) { Assign(pStr); return *this; }

    /** Concatenate string to end of buffer.
        Will do nothing on failure.
        @param pStr String to append
    */
    inline bbStrBuf& operator+=(const bbCHAR* const pStr) { Cat(pStr); return *this; }

    bbStrBuf& operator+=(bbCHARCP cp);

    #if bbSIZEOF_CHAR != 1
    inline bbStrBuf& operator+=(const char* pStr) { Cat(pStr); return *this; }
    inline bbStrBuf& operator+=(char cp) { return operator+=((bbCHARCP)cp); }
    #endif

    bbCHAR* PrintHex(const bbU8* pData, bbUINT size);

    /** Print formatted string.
        Existing string contents will be overwritten.
        @return New length of string
    */
    int Printf(const bbCHAR*, ...);

    /** Concatenate formatted string to end of buffer.
        @return New length of complete string
    */
    int Catf(const bbCHAR*, ...);

    /** Concatenate number as unsigned decimal. 
        @return New length of complete string
    */
    int CatN(bbU64 n);
};

#endif /* bbSTRBUF_H_ */


