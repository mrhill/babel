#ifndef bbBABEL_TIME_H_
#define bbBABEL_TIME_H_

/** @file
	Time related functions.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroup groupTime Time
@{*/

/** Get current tickcount in miliseconds. 
    @return Tickcount in miliseconds
*/
#if (bbOS == bbOS_PALMOS)
bbU32 bbGetTickCount(void);
#else
#define bbGetTickCount() (bbU32)(clock()/(CLOCKS_PER_SEC/1000U))
#endif

/** Get current time in seconds since midnight (00:00:00), January 1, 1970.
    @return Time in seconds since 1970.
*/
#if (bbOS == bbOS_PALMOS)
bbU32 bbGetTime(void);
#else
#define bbGetTime() (bbU32)(time(NULL))
#endif

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_TIME_H_ */
