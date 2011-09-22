#ifndef bbBABEL_ENC_UTF8_H_
#define bbBABEL_ENC_UTF8_H_

/** @file
    String API for UTF8 encoding.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

bbCHARCP bbEncGetCP_utf8(bbENCSTATE* const pState);
void bbEncPutCP_utf8(bbENCSTATE* const pState, bbCHARCP const cp);

/** Maximum number of 8 bit code units per Unicode code point.
    @return 4
*/
#define bbUTF8_CP_MAXCU 4

/** Array to look up number of trail bytes from a lead byte.
    Use trailbyte >> 3 as index into this array. If a trailbyte
    is used as index, 0 is returned. The table will return 3 for
    lead bytes F5, F6, F7, however RFC 3629 defined these leadbytes
    as invalid. So check these externally.
*/
extern const bbU8 bbgUTF8_CountTrailBytes[32];

/** Mask codepoint data portion from lead byte.
    @param c     Lead byte
    @param count Number of trailbytes, as looked up via bbgUTF8_CountTrailBytes
    @return Codepoint data with topbits masked off
*/
#define bbUTF8_CU_MASK_LEAD(c, count) ((bbUINT)(c)&(1<<(6-(count)))-1)

/** Get number of 8 bit code units needed to encode this Unicode code point.
    @param c Valid 32 bit code point
    @return Value between 1 and 4
*/
#define bbUTF8_CP_LENGTH(c) \
     ((bbU32)(c)<=0x7FU ? 1 : \
         ((bbU32)(c)<=0x7FFU ? 2 : \
             ((bbU32)(c)<=0xD7FFU ? 3 : \
                 ((bbU32)(c)<=0xDFFFU || (bbU32)(c)>=0x110000UL ? 0 : \
                     ((bbU32)(c)<=0xFFFFU ? 3 : 4)\
                 ) \
             ) \
         ) \
     )

/** Test if code unit describes codepoint in a single byte.
    @param c (bbCHAR8) 8-bit code unit
    @return  (int)     true or false
*/
#define bbUTF8_CU_IS_SINGLE(c) (((bbUINT)(c)&0x80U)==0)

/** Test if code unit is lead byte
    @param c (bbCHAR8) 8-bit code unit
    @return  (int)     true or false
*/
#define bbUTF8_CU_IS_LEAD(c) (((bbUINT)(c)-0xC0U)<0x35U)
//<0x3EU for 5,6 byte sequences

/** Test if code unit is trail byte
    @param c (bbCHAR8) 8-bit code unit
    @return  (int)     true or false
*/
#define bbUTF8_CU_IS_TRAIL(c) (((bbUINT)(c)&0xC0U)==0x80U)


#define bbUTF8_CP_FWD_1(pStr, i)
#define bbUTF8_CP_FWD_1_PTR(pStr)
#define bbUTF8_CP_FWD_N(pStr, i, n)
#define bbUTF8_CP_BACK_1(pStr, i)
#define bbUTF8_CP_BACK_N(pStr, i, n)
#define bbUTF8_CP_GET(pStr, i, cp)

/** Get a code point from a string at a given code unit offset (must be code point boundary) and increment offset.
    If \a i points to a trail surrogate, it will be returned.
    String must be well-formed UTF-8.

    @param pStr (const bbCHAR8* const) String pointer
    @param i    (const int)         String offset in character code units, will be adjusted
    @param cp   (bbU32)             Output variable, will be filled with code point
*/
#define bbUTF8_CP_NEXT(pStr, i, cp) { \
    (cp)=(pStr)[(i)++]; \
    if(((bbUINT)(cp)-0xC0U) < 0x35U) { \
        bbUINT const __count = (bbUINT) bbgUTF8_CountTrailBytes[(bbUINT)cp >> 3]; \
        (cp) = bbUTF8_CU_MASK_LEAD(cp, __count); \
        switch(__count) { \
        case 3: (cp)=((cp)<<6)|((pStr)[(i)++]&0x3FU); \
        case 2: (cp)=((cp)<<6)|((pStr)[(i)++]&0x3FU); \
        case 1: (cp)=((cp)<<6)|((pStr)[(i)++]&0x3FU); \
        break; \
        } \
    } \
}

/** Same as #bbUTF8_CP_NEXT but uses incrementing pointer */
#define bbUTF8_CP_NEXT_PTR(pStr, cp) { \
    (cp)=*((pStr)++); \
    if(((bbUINT)(cp)-0xC0U) < 0x35U) { \
        bbUINT const __count = (bbUINT) bbgUTF8_CountTrailBytes[(bbUINT)cp >> 3]; \
        (cp) = bbUTF8_CU_MASK_LEAD(cp, __count); \
        switch(__count) { \
        case 3: (cp)=((cp)<<6)|(*((pStr)++)&0x3FU); \
        case 2: (cp)=((cp)<<6)|(*((pStr)++)&0x3FU); \
        case 1: (cp)=((cp)<<6)|(*((pStr)++)&0x3FU); \
        break; \
        } \
    } \
}

#define bbUTF8_CP_PREV(pStr, i, cp)
#define bbUTF8_CP_PREV_PTR(pStr, cp)
#define bbUTF8_CP_APPEND(pStr, i, cp)
#define bbUTF8_CP_APPEND_PTR(pStr, cp)
#define bbUTF8_CP_SETSTART(pStr, i)


#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_ENC_UTF8_H_ */
