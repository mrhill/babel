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

    void Attach(bbCHAR* const pStr, bbUINT len);

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

    inline bbUINT GetCapacity() const { return mCapacity; }

    /** @internal */
    inline void AddSize(bbUINT const increase) { mLen+=increase; }

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

    /** Concatenate string to end of buffer.
        Will do nothing on failure.
        @param pStr String to append
    */
    inline bbStrBuf& operator+=(const bbCHAR* const pStr) { Cat(pStr); return *this; }

    #if bbSIZEOF_CHAR != 1
    inline bbStrBuf& operator+=(const char* pStr) { Cat(pStr); return *this; }
    #endif

    bbStrBuf& operator+=(bbCHARCP cp);

    bbCHAR* PrintHex(const bbU8* pData, bbUINT size);

    /** Print formatted string.
        Existing string contents will be overwritten.
    */
    int Printf(const bbCHAR*, ...);
};

#endif /* bbSTRBUF_H_ */


