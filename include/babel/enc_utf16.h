#ifndef bbBABEL_ENC_bbUTF16_H_
#define bbBABEL_ENC_bbUTF16_H_

/** @file
    String API for UTF16 encoding.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

bbCHARCP bbEncGetCP_utf16le(bbENCSTATE* const pState);
bbCHARCP bbEncGetCP_utf16be(bbENCSTATE* const pState);
void bbEncPutCP_utf16le(bbENCSTATE* const pState, bbCHARCP const cp);
void bbEncPutCP_utf16be(bbENCSTATE* const pState, bbCHARCP const cp);

int bbStrCmp_utf16(const bbCHAR16* pStr1, const bbCHAR16* pStr2);

/** Test if code unit is a lead surrogate, ie in rage U+D800 - U+DBFF.
    @param c (bbCHAR16) 16-bit code unit
    @return  (int)      true or false
*/
#define bbUTF16_CU_IS_LEAD(c) (((c)&0xFC00U)==0xD800U)

/** Test if code unit is a trail surrogate, ie in rage U+DC00 - U+DFFF.
    @param c (bbCHAR16) 16-bit code unit
    @return  (int)      true or false
*/
#define bbUTF16_CU_IS_TRAIL(c) (((c)&0xFC00U)==0xDC00U)

/** Test if code unit is a lead or trail surrogate, ie in rage U+D800 - U+DFFF.
    @param c (bbCHAR16) 16-bit code unit
    @return  (int)      true or false
*/
#define bbUTF16_CU_IS_SURROGATE(c) (((c)&0xF800U)==0xD800U)

/** Build a supplementary code point (range U+10000-U+10FFFF) from lead and trail surrogates.
    @param lead  (bbCHAR16) Valid lead surrogate (code point in range U+D800-U+DBFF)
    @param trail (bbCHAR16) Valid trail surrogate (code point in range U+DC00-U+DFFF)
    @return (bbCHAR32) Code point
*/
#define bbUTF16_CP_GET_SUPPL(lead, trail) \
    (((bbCHAR32)(lead)<<10)+(bbCHAR32)(trail)-((0xD800UL<<10)+0xDC00UL-0x10000UL))


/** Maximum number of 16 bit code units per Unicode code point.
    @return 2
*/
#define bbUTF16_CP_MAXCU 2

/** Get number of 16 bit code units needed to encode this Unicode code point.
    @param c Valid 32 bit code point
    @return 1 or 2
*/
#define bbUTF16_CP_LENGTH(c) (((bbU32)(c)<0x10000UL)?1:2)

/** Skip one code point in UTF16 encoded string, string relative offset.
    String must be well-formed UTF-16.

    @param pStr (bbCHAR16* const) String pointer
    @param i    (int) string offset, will be incremented
*/
#define bbUTF16_CP_FWD_1(pStr, i) if(bbUTF16_CU_IS_LEAD((pStr)[(i)++])) ++(i);

/** Skip one code point in UTF16 encoded string, string pointer.
    String must be well-formed UTF-16.

    @param pStr (bbU16* const) String pointer, will be incremented
*/
#define bbUTF16_CP_FWD_1_PTR(pStr) if(bbUTF16_CU_IS_LEAD(*(pStr++))) ++(pStr);

/** Skip \a n code points in UTF16 encoded string, string relative offset.
    String must be well-formed UTF-16.

    @param pStr (bbCHAR16* const) String pointer
    @param i    (int) string offset, will be incremented
    @param n    (bbUINT) Number of code points to skip
*/
#define bbUTF16_CP_FWD_N(pStr, i, n) { \
    bbUINT _fwd_n_ctr_ = (bbUINT)(n); \
    for(;;) { \
        if (_fwd_n_ctr_==0) break; \
        --_fwd_n_ctr_; \
        bbUTF16_CP_FWD_1(pStr, i) \
    } \
}

/** Skip one code point backwards in UTF16 encoded string, string relative offset.
    String must be well-formed UTF-16.

    @param pStr (const bbCHAR16* const) String pointer 
    @param i    (int)                String offset, will be adjusted
*/
#define bbUTF16_CP_BACK_1(pStr, i) if(bbUTF16_CU_IS_TRAIL((pStr)[--(i)])) --(i);

/** Skip \a n code points backwards in UTF16 encoded string, string relative offset.
    String must be well-formed UTF-16.

    @param pStr (const bbCHAR16* const) String pointer
    @param i    (int)                   String offset, will be adjusted
    @param n    (bbUINT)                Number of code points to skip
*/
#define bbUTF16_CP_BACK_N(pStr, i, n) { \
    bbUINT _back_n_ctr_=(bbUINT)(n); \
    for(;;) { \
        if (_fwd_n_ctr_==0) break; \
        --_fwd_n_ctr_; \
        bbUTF16_CP_BACK_1(pStr, i) \
    } \
}

/** Get a code point from a string at a given code unit offset (must be code point boundary).
    If a supplementary code point is to be read, the offset must point
    to the lead surrogate.
    String must be well-formed UTF-16.

    @param pStr (bbCHAR16* const) Pointer to string
    @param i    (const int)       String offset in character code units (bbCHAR16)
    @param cp   (bbCHAR32)        Output variable to hold read code point
*/
#define bbUTF16_CP_GET(pStr, i, cp) \
    (cp)=(pStr)[i]; if(bbUTF16_CU_IS_LEAD(cp)) (cp)=bbUTF16_CP_GET_SUPPL(cp, (pStr)[(i)+1]);

/** Get a code point from a string at a given code unit offset (must be code point boundary) and increment offset.
    If \a i points to a trail surrogate, it will be returned.
    String must be well-formed UTF-16.

    @param pStr (const bbCHAR16* const) String pointer
    @param i    (const int)         String offset in character code units, will be adjusted
    @param cp   (bbU32)             Output variable, will be filled with code point
*/
#define bbUTF16_CP_NEXT(pStr, i, cp) \
    (cp)=(pStr)[(i)++]; if(bbUTF16_CU_IS_LEAD(cp)) (cp)=bbUTF16_CP_GET_SUPPL((cp), (pStr)[(i)++]);

/** Same as #bbUTF16_CP_NEXT but uses incrementing pointer */
#define bbUTF16_CP_NEXT_PTR(pStr, cp) \
    (cp)=*((pStr)++); if(bbUTF16_CU_IS_LEAD(cp)) (cp)=bbUTF16_CP_GET_SUPPL((cp), *((pStr)++));

/** Skip one code point backwards in UTF16 encoded string and get skipped code point, string relative offset.
    If \a i points to a lead surrogate, it will be returned.
    String must be well-formed UTF-16.

    @param pStr (const bbCHAR16* const) String pointer
    @param i    (const int)         String offset in character code units, will be adjusted
    @param cp   (bbU32)             Output variable, will be filled with code point
*/
#define bbUTF16_CP_GET_BACK(pStr, i, cp) \
    (cp)=(pStr)[--(i)]; if(bbUTF16_CU_IS_TRAIL(cp)) (cp)=bbUTF16_CP_GET_SUPPL((pStr)[--(i)], (cp));

/** Same as #bbUTF16_CP_PREV but uses decrementing pointer */
#define bbUTF16_CP_PREV_PTR(pStr, cp) \
    (cp)=*(--(pStr)); if(bbUTF16_CU_IS_TRAIL(cp)) (cp)=bbUTF16_CP_GET_SUPPL(*(--(pStr)), (cp));

/** Concatenate a code point to a string in UTF16 encoding, post-incrementing offset.

    @param pStr (bbCHAR16* const) String pointer
    @param i    (int)             String offset, will be adjusted
    @param cp   (const bbCHAR32)  Code point to append, must be within valid UTF16 range, i.e. no surrogate code unit
*/
#if bbCPU == bbCPU_ARM
#define bbUTF16_CP_APPEND(pStr, i, cp) \
    if (((bbCHAR32)(cp)<0x10000UL) && !(bbUTF16_CU_IS_SURROGATE(cp))) { \
        ((pStr)[(i)++])=(bbCHAR16)(cp); \
    } else { \
        ((pStr)[(i)++])=(bbCHAR16)((bbUINT)((cp)>>10)|0xD800U); \
        ((pStr)[(i)++])=(bbCHAR16)(((bbUINT)(cp)&~0xFC00U)|0xDC00U); \
    }
#else
#define bbUTF16_CP_APPEND(pStr, i, cp) \
    if (((bbCHAR32)(cp)<0x10000UL) && !(bbUTF16_CU_IS_SURROGATE(cp))) { \
        ((pStr)[(i)++])=(bbCHAR16)(cp); \
    } else { \
        ((pStr)[(i)++])=(bbCHAR16)((bbUINT)((cp)>>10)|0xD800U); \
        ((pStr)[(i)++])=(bbCHAR16)(((bbUINT)(cp)&0x3FFU)|0xDC00U); \
    }
#endif

/** Same as #bbUTF16_CP_APPEND but uses post-incrementing pointer */
#if bbCPU == bbCPU_ARM
#define bbUTF16_CP_APPEND_PTR(pStr, cp) \
    if (((bbCHAR32)(cp)<0x10000UL) && !(bbUTF16_CU_IS_SURROGATE(cp))) { \
        *((pStr)++)=(bbCHAR16)(cp); \
    } else { \
        *((pStr)++)=(bbCHAR16)((bbUINT)((cp)>>10)|0xD800U); \
        *((pStr)++)=(bbCHAR16)(((bbUINT)(cp)&~0xFC00U)|0xDC00U); \
    }
#else
#define bbUTF16_CP_APPEND_PTR(pStr, cp) \
    if (((bbCHAR32)(cp)<0x10000UL) && !(bbUTF16_CU_IS_SURROGATE(cp))) { \
        *((pStr)++)=(bbCHAR16)(cp); \
    } else { \
        *((pStr)++)=(bbCHAR16)((bbUINT)((cp)>>10)|0xD800U); \
        *((pStr)++)=(bbCHAR16)(((bbUINT)(cp)&0x3FFU)|0xDC00U); \
    }
#endif

/** Adjust a code unit offset to the previous code point boundary.
    If the given offset points to a trail surrogate, it will
    be adjusted to the lead surrogate.
    String must be well-formed UTF-16.

    @param pStr (const bbU16* const) String pointer
    @param i    (int)                String offset, will be adjusted
*/
#define bbUTF16_CP_SETSTART(pStr, i) if(bbUTF16_IS_TRAIL((pStr)[i])) --(i);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_ENC_bbUTF16_H_ */
