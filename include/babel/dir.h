#ifndef bbBABEL_DIR_H_
#define bbBABEL_DIR_H_

/** @file
	Directory enumeration.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** Directory handle.
    @see bbDirOpen
*/
typedef void* bbDIRH;

/** Invalid directory handle.
    @see bbDIRH
*/
#define bbDIRH_INVALID NULL

#define bbDIRENTRY_DIR    0x01U
#define bbDIRENTRY_SYSTEM 0x02U

/** Open directory for enumeration.
    Sets bbgErr on failure.
    @param pPath Path to directory, 0-terminated
    @returns handle to opened directory on success, bbDIRH_INVALID on failure.
*/
bbDIRH bbDirOpen( const bbCHAR* pPath);

/** Get next entry from opened directory.
    Sets bbgErr on failure.
    @param handle Handle to opened directory
    @param ppEntry Returns pointer to 0-terminated filename. Must be
                   freed with bbMemFree. Parameter can be NULL.
    @param pFlags  Returns fileflags. Parameter can be NULL.
                   <table>
                   <tr><td>bbDIRENTRY_DIR   </td><td>Directory entry is a directory</td></tr>
                   <tr><td>bbDIRENTRY_SYSTEM</td><td>Directory entry is a system file</td></tr>
                   </table>
    @retval bbEOK  Entry returned successfully.
    @retval bbEEND End of enumeration, no entry is returned but *ppEntry is set to NULL.
*/
bbERR bbDirNext(bbDIRH const handle, bbCHAR** const ppEntry, bbUINT* const pFlags);

/** Close opened directory.
    @param pHandle Handle to opened directory, 
                   if bbDIRH_INVALID is passed, no action is performed.
*/
void bbDirClose(bbDIRH const handle);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_DIR_H_ */
