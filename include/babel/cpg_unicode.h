#ifndef bbBABEL_CPG_UNICODE_H_
#define bbBABEL_CPG_UNICODE_H_

/** @file
    Codepage Unicode.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include "cpg_unicode_blocks.inl"

/** Test if unicode codepoint is wide char.
    @param cp Code point
    @retval !=0 Widechar
    @retval 0 Char with normal width
*/
int bbCpgUnicode_IsWide(bbCHARCP const cp);

/** Test if unicode codepoint is non-spacing (0-width) character.
    @param cp Code point
    @retval !=0 Non-spacing
    @retval 0 Normal spacing
*/
int bbCpgUnicode_IsNonspace(bbCHARCP const cp);

/** Upcase unicode codepoint.
    @param (bbCHARCP) cp Code point
    @return (bbCHARCP) Upcased version of character
*/
#define bbCpgUnicode_Upcase(cp) ((bbU32)((cp)-'a')<=(bbU32)('z'-'a') ? ((cp)&0xDF) : (cp))

/** Get block ID for codepoint.
    @param cp Code point
    @return Block id or (bbUINT)-1 if no block
*/
bbUINT bbCpgUnicode_GetBlock(bbCHARCP const cp);

/** Get range for Unicode block ID.
    @param blockID Unicode block ID, must be valid
    @param pSize Returns Size of block in number of codepoints
    @return Codepoint the block starts at
*/
bbCHARCP bbCpgUnicode_GetBlockRange(bbUINT const blockID, bbU32* const pSize);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_CPG_UNICODE_H_ */

