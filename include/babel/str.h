#ifndef bbBABEL_STR_H_
#define bbBABEL_STR_H_

/** @file
    String and character encoding API.

    In babel land a character encoding is a pair of encoding and a codepage.

    All API calls are mapped according to the currently configured
    internal character encoding.
*/

#include "enc.h"
#include "cpg_unicode.h"

#if bbOS==bbOS_QT && defined(__cplusplus)
class QString;
bbCHAR* bbStrFromQt(const QString& str);
#endif

#if bbSTRCONV==bbSTRCONV_WIN
#include <iconv.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/** Function pointer table for codepage runtime abstraction. */
typedef struct
{
    /** Test if codepoint is wide char.
        @param cp Code point
        @retval !=0 Widechar
        @retval 0 Char with normal width
    */
    int (*IsWide)(bbCHARCP const cp);

    /** Test if codepoint is non-spacing (0-width) character.
        @param cp Code point
        @retval !=0 Non-spacing
        @retval 0 Normal spacing
    */
    int (*IsNonspace)(bbCHARCP const cp);

    /** Get codepoint property bitmask.
        @param cp Codepoint
        @return Bitmask with property flags, see bbCPPROP_*
    */
    /*bbUINT (*GetCPProp)(bbCHARCP const cp);*/

    bbU8 CpgProp;  /**< Bitmask with codepage properties, see bbCPGPROP_* */

} bbCPGTABLE;

typedef int (*bbCPGTABLE_FN_IS)(bbCHARCP const cp);

#define bbCPPROP_CTRL  0x00000001U
#define bbCPPROP_NUM   0x00000002U
#define bbCPPROP_ALPHA 0x00000004U

/** bbCPGTABLE::CpgProp flagbit: Codepage has wide characters. */
#define bbCPGPROP_HASWIDE 0x1U

/** bbCPGTABLE::CpgProp flagbit: Codepage has non-spacing characters. */
#define bbCPGPROP_HASNONSPACE 0x2U

/** Get function pointer table for given character encoding.
    @param codepage Codepage specifier, must be valid
    @return Pointer to function pointer table
*/
const bbCPGTABLE* bbCpgGetTable(bbUINT const codepage);


#if defined(bbDOXYGEN)

/** Get length of a 0-terminated string in character code units.
    @param pStr Pointer to 0-terminated string in internal encoding.
    @return Length in character code units without 0-terminator.
*/
bbUINT bbStrLen(const bbCHAR* pStr);

/** Copy 0-terminated string.
    The string will be copied including the 0-terminator.
    @param pDst Pointer to destination to copy to. Enough memory must be ensured.
    @param pSrc Pointer to 0-teminated string
    @return Value of parameter \a pDst
*/
bbCHAR* bbStrCpy(bbCHAR* pDst, const bbCHAR* pSrc);

/** Copy 0-terminated SBC string to internal encoding.
    The string will be copied including the 0-terminator.
    @param pDst Pointer to destination to copy to. Enough memory must be ensured.
    @param pSrc Pointer to 0-teminated string in SBC
    @return Pointer to written 0-terminator on destination.
*/
bbCHAR* bbStrCpySBC(bbCHAR* pDst, const char* pSrc);

/** Copy max N character code units of 0-terminated string.
    The string will be copied including the 0-terminator. If the source
    string length in character code units plus the 0-terminator exceeds
    \a len, the target string is truncated at \a len character code units,
    and no 0-terminator is appended.
    @param pDst Pointer to destination to copy to. Enough memory must be ensured.
    @param pSrc Pointer to 0-teminated string
    @param len  Maximum number of character code units to copy.
    @return Value of parameter \a pDst
*/
bbCHAR* bbStrNCpy(bbCHAR* pDst, const bbCHAR* pSrc, const bbUINT len);

/** Compare two 0-terminated strings.
    Comparison is case-sensitive based on the code-point values.
    @pStr1 Pointer to first string
    @pStr2 Pointer to second string
    @retval -1 pStr < pStr2
    @retval  0 pStr = pStr2
    @retval  1 pStr > pStr2
*/
int bbStrCmp(const bbCHAR* pStr1, const bbCHAR* pStr2);

/** Case-insensitively compare two 0-terminated strings.
    Comparison is case-insensitive.
    @pStr1 Pointer to first string
    @pStr2 Pointer to second string
    @retval -1 pStr < pStr2
    @retval  0 pStr = pStr2
    @retval  1 pStr > pStr2
*/
int bbStrICmp(const bbCHAR* pStr1, const bbCHAR* pStr2);

/** Find substring in string.
    @param pStr String to search in
    @param pSubStr Substring to search for
    @returns Returns Pointer to first occurance of substring, or NULL if not found.
*/
bbCHAR* bbStrStr(const bbCHAR* pStr, const bbCHAR* pSubStr);

/** The maximum number of code units per code point.
    @return (bbUINT) Number of bbCHAR code units
*/
#define bbCP_MAXCU

/** How many code units are used to encode this code point?
    The result is not defined if c is not a valid code point.
    @param c (bbCHARCP) Character code point
    @return (bbUINT) Number of bbCHAR code units
*/
#define bbCP_LENGTH(c)

/** Advance the string offset from one code point boundary to the next.
    (Post-incrementing iteration.)
    @param pStr (bbCHAR* const) String pointer
    @param i    (int) string offset, will be adjusted
*/
#define bbCP_FWD_1(pStr, i)

/** Advance the string offset from one code point boundary to the n-th next one,
    i.e., move forward by n code points.
    (Post-incrementing iteration.)
    @param pStr (const bbCHAR* const) String pointer
    @param i    (int)                String offset, will be adjusted
    @param n    (const bbUINT)         Number of code points to skip
*/
#define bbCP_FWD_N(pStr, i, n)

/** Move the string offset from one code point boundary to the previous one.
    (Pre-decrementing backward iteration.)
    The input offset may be the same as the string length.
    @param pStr (const bbCHAR* const) String pointer 
    @param i    (int)                String offset, will be adjusted
*/
#define bbCP_BACK_1(pStr, i)

/** Move the string offset from one code point boundary to the n-th one before it,
    i.e., move backward by n code points.
    (Pre-decrementing backward iteration.)
    The input offset may be the same as the string length.

    @param pStr (const bbCHAR* const) String pointer
    @param i    (int)                String offset, will be adjusted
    @param n    (const bbUINT)         Number of code points to skip
*/
#define bbCP_BACK_N(s, i, n)

/** Get a code point from a string at a random-access offset, without changing the offset.
    The offset must point to the lead surrogate unit for a
    supplementary code point, in which case the macro will read
    the adjacent matching surrogate as well.
    The result is undefined if the offset points to a single, unpaired surrogate.

    @param pStr (const bbCHAR* const) Pointer to string
    @param i    (const int)          String offset in character code units (bbCHAR)
    @param cp   (bbCHARCP)              Output variable to hold read code point
*/
#define bbCP_GET(pStr, i, cp)

/** Get code point from string, post-incrementing string pointer (forward parse string).
    @param pStr (const bbCHAR* const) String pointer
    @param i    (const int)          String offset in character code units, will be adjusted
    @param cp   (bbCHARCP)           Output variable, will be filled with code point
*/
#define bbCP_NEXT(pStr, i, cp)

/** Same as #CP_NEXT but uses incrementing pointer */
#define bbCP_NEXT_PTR(pStr, cp)

/** Get code point from string, pre-decrementing string pointer (reverse parse string).
    @param pStr (const bbCHAR* const) String pointer
    @param i    (const int)          String offset in character code units, will be adjusted
    @param cp   (bbCHARCP)              Output variable, will be filled with code point
*/
#define bbCP_PREV(pStr, i, cp)

/** Same as #bbCP_PREV but uses decrementing pointer */
#define bbCP_PREV_PTR(pStr, cp)

/** Append code point to string, post incrementing string pointer.
    This will overwrite 1 or 2 code units, and possibly invalidates surrogate pairs.
    @param pStr (bbCHAR* const) String pointer
    @param i    (int)          String offset to append from, will be adjusted
    @param cp   (const bbCHARCP)  Character code point to write
*/
#define bbCP_APPEND(pStr, i, cp)

/** Same as #bbCP_APPEND but uses incrementing pointer */
#define bbCP_APPEND_PTR(pStr, cp)

/** Adjust a random-access offset to a code point boundary at the start of a code point.
    If the offset points to the trail surrogate of a surrogate pair,
    then the offset is decremented.
    Otherwise, it is not modified.

    @param pStr (const bbCHAR* const) String pointer
    @param i    (int)                String offset, will be adjusted
*/
#define bbCP_SETSTART(pStr, i)

#elif bbENC==bbENC_SBC

#define bbStrLen   bbStrLen_sbc
#define bbStrCpy   bbStrCpy_sbc
#define bbStrNCpy  bbStrNCpy_sbc
#define bbStrCmp   bbStrCmp_sbc
#define bbStrICmp  bbStrICmp_sbc
#define bbStrNCmp  bbStrNCmp_sbc
#define bbStrNICmp bbStrNICmp_sbc
#define bbStrStr   bbStrStr_sbc
#define bbStrIStr  bbStrIStr_sbc

#define bbSprintf  bbSprintf_sbc
#define bbVsnprintf bbVsnprintf_sbc
#define bbPrintf   bbPrintf_sbc
#define bbVprintf  bbVprintf_sbc

#define bbCP_MAXCU 1
#define bbCP_LENGTH(c)  1
#define bbCP_FWD_1(pStr, i) ++(i);
#define bbCP_FWD_1_PTR(pStr) ++(pStr);
#define bbCP_FWD_N(pStr, i, n) (i)+=(n);
#define bbCP_BACK_1(pStr, i)  --(i);
#define bbCP_BACK_N(pStr, i, n) (i)-=(n);
#define bbCP_GET(pStr, i, cp) cp=(pStr)[i];
#define bbCP_NEXT(pStr, i, cp) cp=(pStr)[(i)++];
#define bbCP_NEXT_PTR(pStr, cp) cp=*((pStr)++);
#define bbCP_PREV(pStr, i, cp) cp=(pStr)[--(i)];
#define bbCP_PREV_PTR(pStr, cp) cp=*(--(pStr));
#define bbCP_APPEND(pStr, i, cp) (pStr)[(i)++]=cp;
#define bbCP_APPEND_PTR(pStr, cp) *(pStr++)=cp;
#define bbCP_SETSTART(pStr, i)

#elif bbENC==bbENC_DBC

#define bbStrLen   bbStrlen_dbc
#define bbStrCpy   bbStrCpy_dbc
#define bbStrNCpy  bbStrNCpy_dbc
#define bbStrCmp   bbStrCmp_dbc
#define bbStrICmp  bbStrICmp_dbc
#define bbStrNCmp  bbStrNCmp_dbc
#define bbStrNICmp bbStrNICmp_dbc
#define bbStrStr   bbStrStr_dbc
#define bbStrIStr   bbStrIStr_dbc

#define bbSprintf  bbSprintf_dbc
#define bbVsnprintf bbVsnprintf_dbc
#define bbPrintf   bbPrintf_dbc
#define bbVprintf  bbVprintf_dbc

#define bbCP_MAXCU 1
#define bbCP_LENGTH(c)  1
#define bbCP_FWD_1(pStr, i) ++(i);
#define bbCP_FWD_1_PTR(pStr) ++(pStr);
#define bbCP_FWD_N(pStr, i, n) (i)+=(n);
#define bbCP_BACK_1(pStr, i)  --(i);
#define bbCP_BACK_N(pStr, i, n) (i)-=(n);
#define bbCP_GET(pStr, i, cp) cp=(pStr)[i];
#define bbCP_NEXT(pStr, i, cp) cp=(pStr)[(i)++];
#define bbCP_NEXT_PTR(pStr, cp) cp=*((pStr)++);
#define bbCP_PREV(pStr, i, cp) cp=(pStr)[--(i)];
#define bbCP_PREV_PTR(pStr, cp) cp=*(--(pStr));
#define bbCP_APPEND(pStr, i, cp) (pStr)[(i)++]=cp;
#define bbCP_APPEND_PTR(pStr, cp) *(pStr++)=cp;
#define bbCP_SETSTART(pStr, i)

#elif bbENC==bbENC_UTF8

#define bbStrLen   bbStrLen_sbc
#define bbStrCpy   bbStrCpy_sbc
#define bbStrCmp   bbStrCmp_sbc
#define bbStrICmp  bbStrICmp_sbc
#define bbStrNCmp  bbStrCmp_sbc
#define bbStrNICmp bbStrICmp_sbc
#define bbStrStr   bbStrStr_sbc
#define bbStrIStr  bbStrIStr_sbc

#define bbSprintf  bbSprintf_sbc
#define bbVsnprintf bbVsnprintf_sbc
#define bbPrintf   bbPrintf_sbc
#define bbVprintf  bbVprintf_sbc

#define bbCP_MAXCU                 bbUTF8_CP_MAXCU
#define bbCP_LENGTH(c)             bbUTF8_CP_LENGTH(c)
#define bbCP_FWD_1(pStr, i)        bbUTF8_CP_FWD_1(pStr, i)
#define bbCP_FWD_1_PTR(pStr)       bbUTF8_CP_FWD_1_PTR(pStr)
#define bbCP_FWD_N(pStr, i, n)     bbUTF8_CP_FWD_N(pStr, i, n)
#define bbCP_BACK_1(pStr, i)       bbUTF8_CP_BACK_1(pStr, i)
#define bbCP_BACK_N(pStr, i, n)    bbUTF8_CP_BACK_N(pStr, i, n)
#define bbCP_GET(pStr, i, cp)      bbUTF8_CP_GET(pStr, i, cp)
#define bbCP_NEXT(pStr, i, cp)     bbUTF8_CP_NEXT(pStr, i, cp)
#define bbCP_NEXT_PTR(pStr, cp)    bbUTF8_CP_NEXT_PTR(pStr, cp)
#define bbCP_PREV(pStr, i, cp)     bbUTF8_CP_PREV(pStr, i, cp)
#define bbCP_PREV_PTR(pStr, cp)    bbUTF8_CP_PREV_PTR(pStr, cp)
#define bbCP_APPEND(pStr, i, cp)   bbUTF8_CP_APPEND(pStr, i, cp)
#define bbCP_APPEND_PTR(pStr, cp)  bbUTF8_CP_APPEND_PTR(pStr, cp)
#define bbCP_SETSTART(pStr, i)     bbUTF8_CP_SETSTART(pStr, i)

#elif bbENC==bbENC_UTF16

#define bbStrLen   bbStrlen_dbc
#define bbStrCpy   bbStrCpy_dbc
#define bbStrNCpy  bbStrNCpy_dbc
#define bbStrCmp   bbStrCmp_dbc //xxx
#define bbStrICmp  bbStrICmp_dbc //xxx
#define bbStrNCmp  bbStrNCmp_dbc //xxx
#define bbStrNICmp bbStrNICmp_dbc //xxx
#define bbStrStr   bbStrStr_dbc //xxx
#define bbStrIStr  bbStrIStr_dbc //xxx

#define bbSprintf  bbSprintf_dbc
#define bbVsnprintf bbVsnprintf_dbc
#define bbPrintf   bbPrintf_dbc
#define bbVprintf  bbVprintf_dbc

#define bbCP_MAXCU                 bbUTF16_CP_MAXCU
#define bbCP_LENGTH(c)             bbUTF16_CP_LENGTH(c)
#define bbCP_FWD_1(pStr, i)        bbUTF16_CP_FWD_1(pStr, i)
#define bbCP_FWD_1_PTR(pStr)       bbUTF16_CP_FWD_1_PTR(pStr)
#define bbCP_FWD_N(pStr, i, n)     bbUTF16_CP_FWD_N(pStr, i, n)
#define bbCP_BACK_1(pStr, i)       bbUTF16_CP_BACK_1(pStr, i)
#define bbCP_BACK_N(pStr, i, n)    bbUTF16_CP_BACK_N(pStr, i, n)
#define bbCP_GET(pStr, i, cp)      bbUTF16_CP_GET(pStr, i, cp)
#define bbCP_NEXT(pStr, i, cp)     bbUTF16_CP_NEXT(pStr, i, cp)
#define bbCP_NEXT_PTR(pStr, cp)    bbUTF16_CP_NEXT_PTR(pStr, cp)
#define bbCP_PREV(pStr, i, cp)     bbUTF16_CP_PREV(pStr, i, cp)
#define bbCP_PREV_PTR(pStr, cp)    bbUTF16_CP_PREV_PTR(pStr, cp)
#define bbCP_APPEND(pStr, i, cp)   bbUTF16_CP_APPEND(pStr, i, cp)
#define bbCP_APPEND_PTR(pStr, cp)  bbUTF16_CP_APPEND_PTR(pStr, cp)
#define bbCP_SETSTART(pStr, i)     bbUTF16_CP_SETSTART(pStr, i)

#endif /* bbENC */

/** bbStrTo64 and bbStrToArrU64 parsing options. */
typedef enum
{
    bbSTROPTMODEMASK = 1,
    /** Parsing mode: Numerals only, leading and trailing spaces and tabs are skipped (default). */
    bbSTROPT_MODENUM = 0,
    /** Parsing mode: Character code points. */
    bbSTROPT_MODECP = 1,

    /** Stop on unknown char instead of 0-terminator. */
    bbSTROPT_STOPONUKCHAR = 0x0010,
    /** Recognize 0x or 0X as hex prefix. */
    bbSTROPT_HEX0X = 0x0020,
    /** Recognize $ as hex prefix. */
    bbSTROPT_HEXDOLLAR = 0x0040,
    /** Recognize & as hex prefix. */
    bbSTROPT_HEXAMP = 0x0080,
    /** Recognize h or H as hex suffix. */
    bbSTROPT_HEXSUFH = 0x0100,
    /** Recognize 0 followed by number as octal. */
    bbSTROPT_OCT = 0x0200,
    /** Recognize % as bin prefix. */
    bbSTROPT_BINPERCENT = 0x0400,
    /** Recognize b or B as bin prefix. */
    bbSTROPT_BINB = 0x0800,
    /** Recognize b or B as bin suffix. */
    bbSTROPT_BINSUFB = 0x1000,
    /** Recognize - negative sign. */
    bbSTROPT_MINUS = 0x2000,

    /** in bbSTROPT_MODECP support \-escaped numerals. */
    bbSTROPT_ESC = 0x4000,

    /** Flag combination: Recognize all hex prefixes and suffixes. */
    bbSTROPT_ALLHEX = (bbSTROPT_HEX0X|bbSTROPT_HEXDOLLAR|bbSTROPT_HEXAMP|bbSTROPT_HEXSUFH),
    /** Flag combination: Recognize all suffixes. */
    bbSTROPT_ALLSUF = (bbSTROPT_BINSUFB|bbSTROPT_HEXSUFH),
    /** Flag combination: Recognize all formats. */
    bbSTROPT_ALLFMT = (bbSTROPT_ALLHEX|bbSTROPT_OCT|bbSTROPT_BINPERCENT|bbSTROPT_BINB|bbSTROPT_BINSUFB)

} bbSTROPT;

/** Parse number from string.
    The parser recognizes:
    - signed decimal integers, starting with 0-9 or -
    - unsigned hexadecimal integers, starting with 0x, &, or $
    - unsigned binary integers, starting with % or b or B

    if #bbSTROPT_STOPONUKCHAR is set, parsing will stop on the first unknown
    character, otherwise the string must be terminated by 0.

    Leading and trailing spaces and tabs will be skipped.

    @param pStr     Pointer to string to be parsed
    @param ppStrOut Returns where in the input string parsing stopped, can be NULL
    @param pNum  Returns parsed number on success, or 0 on failure
    @param opt   Flag bitmask, see bbSTROPT
    @return The following return codes are defined, bbErrSet() is called for any code except bbEOK.
            - bbEOK       String parsed completely, *pNum valid, *ppStr points to the terminating code unit
            - bbENOTFOUND String parsed completely, no chars found that form a number, *pNum undefined, *ppStr points to the terminating code unit
            - bbESYNTAX   Syntax error in tring, *pNum undefined, *ppStr points to the failure location
            Any other return value indicates error.
            *ppStr and *pNum are undefined in case of error.
*/
bbERR bbStrToS64(bbCHAR* pStr, bbCHAR** ppStrOut, bbS64* pNum, bbUINT opt);

#define bbStrToU64(pStr, ppStrOut, pNum, opt) bbStrToS64(pStr, ppStrOut, (bbS64*)pNum, opt)

/** Parse array of numbers from string.
    See bbStrToS64 for information of how individual numbers are parsed.

    A malformed string is handled as error, bbgErr will be set to bbESYNTAX, and
    and *ppStr will point to the character causing the parse error. For any other error,
    *ppStr will be undefined on exit.

    @param ppStr Pointer to pointer to string to be parsed, will be updated on exit
    @param opt   Flag bitmask, see bbSTROPT
    @param pSize Size of returned array in number of elements, can be 0, *pSize not updated on error
    @return Pointer to array of bbU64 values in heap block, must be freed externally, or NULL on error.
            Note that even for array size 0 a heap block is returned, which must be freed.
*/
bbU64* bbStrToArrU64(bbCHAR** ppStr, bbUINT const opt, bbUINT* const pSize);

bbU32 bbStrToU32(const bbCHAR* pStr, bbUINT opt);
bbS32 bbStrToS32(const bbCHAR* pStr, bbUINT opt);

/** Duplicate 0-terminated string in new heap block.
    @param pStr Pointer to 0-terminated string in internal encoding
    @return Pointer to heap block, containing 0-terminated string, must be freed externally,
            or NULL on out of memory
*/
bbCHAR* bbStrDup(const bbCHAR* const pStr);

/** Duplicate 0-terminated string in new heap block with extra space at end.
    @param pStr Pointer to 0-terminated string in internal encoding
    @param extralen Number of extra characters to reserve buffer with, 0-terminator not included.
    @return Pointer to heap block, containing 0-terminated string, must be freed externally,
            or NULL on out of memory
*/
bbCHAR* bbStrDupEx(const bbCHAR* const pStr, bbUINT const extralen);

/** Calculate 32 bit has for string.
    @param pStr String to hash
    @return Hash value
*/
bbU32 bbStrHash(const bbCHAR* const pStr);

/** Skip whitespace (char 32 and 9).
    @param pStr String to scan
    @return Advanced string pointer
*/
bbCHAR* bbStrSkipSpace(const bbCHAR* pStr);

/** Skip string until separator char or 0-terminator is reached.
    @param pStr      String to scan
    @param sepChar   Character code point of separator char, 0 to skip until next whitespace.
    @param ppWordEnd Returns pointer to whitespace after last word end, 
                     or same as return value if no whitespace between wordend and separator. Can be NULL.
    @return Advanced string pointer
*/
bbCHAR* bbStrSkip(const bbCHAR* pStr, bbCHARCP const sepChar, bbCHAR** ppWordEnd);

/** bbStrSplit options. */
typedef enum
{
    bbSTRSPLIT_INPLACE    = 0x80000000UL, //!< Split string in place, will insert 0-terminators into source string.
    bbSTRSPLIT_STRIPSPACE = 0x40000000UL, //!< Split leading and trailing whitespace from substrings
    bbSTRSPLIT_CHARCPMASK = 0x00FFFFFFUL  //!< Bitmask to isolate codepoint from \a opt parameter

} bbSTRSPLIT;

/** Split string on separator char.

    If substrings are split in place (bbSTRSPLIT_INPLACE), the returned string pointers will
    point into the source string pStr, and the source string will be changed by inserting 0-terminators.

    If substrings are copied, the required memory will be allocated in the returned heap block,
    thus freeing these substring is done by freeing the string index block.

    @param pStr String to split, 0-terminated
    @param opt  Character code point used as substring separator character,
                0 to split on whitespace, ORed with flag bits see bbSTRSPLIT
                @param pCount Returns number of split substrings, can be NULL
    @param pCount Returns number of split substrings, at least 1 on success. \a pCount can be NULL.
    @return Returns List of substrings in heap block, terminated by a NULL pointer.
            The [count+1]'s  entry in the returned string index will pointer to the end of the input string.
            Heap block must be freed externally.
            Returns NULL on failure.
*/
bbCHAR** bbStrSplit(const bbCHAR* pStr, bbU32 opt, bbUINT* const pCount);

#if defined(bbDOXYGEN)

    /** Handle to a character encoding conversion session.
        @see bbStrConvOpen. bbStrConvClose, bbStrConvDo
    */
    typedef void* bbSTRCONVH;

    /** Invalid character encoding conversion handle.
        @see bbCONVH
    */
    #define bbSTRCONVH_INVALID NULL

#elif bbSTRCONV==bbSTRCONV_INTERNAL

    typedef struct bbsSTRCONVH
    {
        bbUINT encto;
        bbUINT encfrom;
        bbU8   first;

    }* bbSTRCONVH;

    #define bbSTRCONVH_INVALID NULL

#elif bbSTRCONV==bbSTRCONV_WIN

    typedef struct
    {
        bbUINT wincp_to;
        bbUINT wincp_from;
    }* bbSTRCONVH;

    #define bbSTRCONVH_INVALID NULL

#elif bbSTRCONV==bbSTRCONV_ICONV

    typedef iconv_t bbSTRCONVH;
    #define bbSTRCONVH_INVALID NULL

#else

    #error unknown setting for bbSTRCONV

#endif

#define bbSTRCONV_IGNORE   0x2
#define bbSTRCONV_TRANSLIT 0x4

/** Create a descriptor for character encoding conversion.
    Sets bbgErr on failure.
    @param encto   ID of character encoding to encode to. Use #bbCE_INTERNAL for internal encoding. See bbCE_*
    @param encfrom ID of character encoding to decode from. Use #bbCE_INTERNAL for internal encoding. See bbCE_*
    @param flags Combination of flag bits.
                 <table>
                 <tr><td>bbSTRCONV_IGNORE           </td><td>Drop characters code points that cannot be represented in destination encoding</td></tr>
                 <tr><td>bbSTRCONV_TRANSLIT         </td><td>Transliterate characters code points that cannot be represented in destination encoding</td></tr>
                 </table>
    @return Handle on success or bbSTRCONVH_INVALID on failure
    @see bbStrConvFrom, bbStrConvTo, bbStrConvCLose
*/
bbSTRCONVH bbStrConvOpen( const bbUINT encto, const bbUINT encfrom, const bbUINT flags);

/** Convert a number of characters from source encoding to internal encoding.
    Input and output strings are not 0-terminated.
    Sets bbgErr on failure.
    @param handle    Stateful conversion handle
    @param ppIn      Pointer to pointer to source data.
                     Will be forwarded on exit by the number of bytes consumed.
    @param pInLeft   Pointer to value specifying number of bytes left in source data.
                     Will be decreased on exit by the number of bytes read from the source data.
    @param ppOut     Pointer to pointer to destination data.
                     Will be forwarded on exit by the number of bytes written.
    @param pOutLeft  Pointer to value specifying number of free bytes left in destination buffer.
                     Will be decreased on exit by the number of bytes written.
    @retval bbEOK    All data was converted successfully.
    @retval bbEFULL  Destination buffer full.
    @retval bbENOMEM Out of memory for internal temp data.
*/
bbERR bbStrConvDo( bbSTRCONVH const handle, const bbU8** const ppIn, bbUINT* const pInLeft, bbU8** const ppOut, bbUINT* const pOutLeft);

/** Reset conversion state for handle.
    @param handle   Stateful conversion handle
*/
void bbConvReset( bbSTRCONVH const handle);

/** Close a conversion handle.
    @param handle Conversion handle. Can be bbSTRCONVH_INVALID for no action.
*/
void bbStrConvClose( bbSTRCONVH const handle);

/** Convert string in given encoding to internal encoding and return result in an allocated heap block.
    Sets bbgErr on failure.
    @param encoding ID of character encoding of source data (see bbCE_*)
    @param flags xxx
    @param pSrc Pointer to 0-terminated source string in encoding specified by \a encoding.
    @return Pointer to allocated heap-block containing 0-terminated string in
            internal encoding on success or NULL on failure. Memory must be freed externally.
*/
bbCHAR* bbStrConvMemFrom( const bbUINT encoding, const bbUINT flags, const bbU8* pSrc);

/** Convert string in internal encoding to given encoding and return result in an allocated heap block.
    Sets bbgErr on failure.
    @param encoding ID of character encoding to convert to (see bbCE_*)
    @param flags see bbStrConvOpen()
    @param pSrc Pointer to 0-terminated source string in internal encoding.
    @return Pointer to allocated heap-block containing 0-terminated string in
            encoding specified by \a encoding on success, or NULL on failure. 
            Memory must be freed externally.
*/
bbU8* bbStrConvMemTo( const bbUINT encoding, const bbUINT flags, const bbCHAR* pSrc);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_STR_H_ */
