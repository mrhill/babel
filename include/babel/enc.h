#ifndef bbBABEL_ENC_H_
#define bbBABEL_ENC_H_

/** @file
    Character encoding API.
*/

#include "enc_sbc.h"
#include "enc_dbc.h"
#include "enc_utf8.h"
#include "enc_utf16.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** Maximum number of bytes per code point in any character encoding. */
#define bbENC_CP_MAXBYTES 8

/** Maximum number of bytes per code unnit in any character encoding. */
#define bbENC_CU_MAXBYTES 4

/** Special character code point for EOF or error. Has top-bit set. */
#define bbCHARCP_EOF 0xFFFFFFFFUL
/** Special character code point line return sequence. Has top-bit set. */
#define bbCHARCP_RETURN 0xFFFFFFFEUL
/** Bitmask for testing if a bbCHARCP is a special code point. */
#define bbCHARCPESC 0xF0000000UL

/** Character encoding parser state. */
struct bbENCSTATE
{
    bbU8*       pStr;       //!< [in, out] Current string pointer
    void*       user;       //!< [in]      User context
    bbU32       state;      //!< [in, out] Encoding specific data
    bbU8        cpsize;     //!< [out]     Size of last read or written code point in bytes
    bbU8        malformed;  //!< [out]     Invalid code unit sequence encountered

    //note cpsize and malformed are accessed together with *(bbU16*)&cpsize
};

/** Function pointer table for encoding runtime abstraction.
    All encoding implementations use common struct bbENCSTATE to store string scan state.
*/
typedef struct
{
    /** Get next character code point from scan position.

        Updates bbENCSTARTE::pStr and bbENCSTARTE::state with the new scan position.
        Returns number of skipped bytes in bbENCSTARTE::cpsize.

        If codeunits at the current scan positon form an invalid sequence in the
        current encoding, the first codeunit of the invalid sequence is returned
        as codepoint, and the scan position is forwarded by 1 codeunit. Note that
        this may produce codepoints outside the valid range of the current encoding.

        The buffer bbENCSTARTE::pStr points into must contain enough bytes,
        no boundary check is performed.

        @param pState String scan state, will be updated on exit
        @return Character code point
    */
    bbCHARCP (*GetCP)(bbENCSTATE* const pState);

    /** Get next character code point before scan position, reverse scanning direction.

        Updates bbENCSTARTE::pStr and bbENCSTARTE::state with the new scan position.
        Returns number of skipped bytes in bbENCSTARTE::cpsize.

        If codeunits form an invalid sequence in the current encoding,
        the last codeunit of the invalid sequence is returned as codepoint,
        and the scan position is decrease by 1 code unit. Note that
        this may produce codepoints outside the valid range of the current encoding.

        The buffer in \a pState must contain at least one complete codepoint before
        the bbENCSTATE::pStr pointer, there is no buffer start boundary check.

        @param pState String scan state, will be updated on exit
        @return Character code point
    */
    //bbCHARCP (*GetCPBack)(bbENCSTATE* const pState);

    bbCHARCP (*GetCU)(bbENCSTATE* const pState);
    bbCHARCP (*GetCUBack)(bbENCSTATE* const pState);

    void (*PutCP)(bbENCSTATE* const pState, bbCHARCP const cp);

    /** Init string scan state.
        @param pState String scan state structure to be initialized
    */
    void     (*Init)     (bbENCSTATE* const pState);

    /** Name, 0-terminated. Can be used for menus. */
    const bbCHAR* pName;

    /** Short description. Can be used for help. */
    const bbCHAR* pDescription;

    /** Byte size of a character code unit, or 0 if encoding is not byte aligned. */
    bbU8    cusize;

    /** Maximum byte size of a character code point. */
    bbU8    cpmaxsize;

    /** !=0 if encoding scheme is stateless. */
    bbU8    stateless;

    /** Code unit index into bbENCTABLE::pName pointing to the codepoint
        to use for the accelerator key in menus.
    */
    bbU8    accelKey;

} bbENCTABLE;

typedef bbCHARCP (*bbENCTABLE_FN_GETCP)    (bbENCSTATE* const);
typedef bbCHARCP (*bbENCTABLE_FN_GETCPBACK)(bbENCSTATE* const);

/** Get function pointer table for given character encoding.
    @param enc Character encoding
    @return Pointer to function pointer table or NULL on error
*/
bbENCTABLE* bbEncGetTable(bbUINT const enc);

#ifdef  __cplusplus
}
#endif

#endif

