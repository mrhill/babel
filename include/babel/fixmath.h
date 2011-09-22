#ifndef bbBABEL_FIXMATH_H_
#define bbBABEL_FIXMATH_H_

/** @file
	Fix-point math routines
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroupFixmath Fix-point math routines
@{*/

/** Initialize fixmath module.
    Sets bbgErr on failure.
    @return bbEOK on success or error code of failure
*/
bbERR bbInitFixMath(void);

/* Destroy fixmath module.
   Save to call multiple times and also if bbInitFixMath was not called before.
*/
void bbDestroyFixMath(void);

/** Size of division table #bbgpDivTab. */
#define bbDIVTABSIZE 2048
/** Fix-point precission of division table #bbgpDivTab. */
#define bbDIVTABPREC 16
/** Fix-point division table with precission #bbDIVTABPREC and size #bbDIVTABSIZE. */
extern bbU16* bbgpDivTab;

/** Log2 of size of period in sine table #bbgpSinTab. */
#define bbSINTABPERLOG2 12
/** Size of sine table #bbgpSinTab. */
#define bbSINTABSIZE ((1U<<bbSINTABPERLOG2)+(1U<<(bbSINTABPERLOG2-2)))
/** Size of period in sine table #bbgpSinTab. */
#define bbSINTABPER (1U<<bbSINTABPERLOG2)
/** Fix-point precission of sine table #bbgpSinTab. */
#define bbSINTABPREC 10
/** Fix-point sine table with precission #bbSINTABPREC, 
    size #bbSINTABSIZE and periodic size #bbSINTABPER. */
extern short* bbgpSinTab;

/** Fast integer square root.
    @param x Value
    @return Integer square root of x
*/
bbU32 bbSqrFast(bbU32 x);

/** Get pytagoras.
    @return sqr(dx^2 + dy^2)
*/
bbU32 bbPyt(long dx, long dy);

/** Count set bits in 32 bit value.
    @param val 32 bit value 
    @return Number of set bits
*/
bbUINT bbCountSetBits(bbU32 val);

/** Get position of top bit (higehst bit set).
    @param val 32 bit value 
    @return Bit position 0-31, returns 0 for \val = 0.
*/
bbUINT bbGetTopBit(bbU32 val);

/** Test if integer is power of 2.
    @param x Number to test
    @return true if power of 2, false otherwise
*/
#define bbIsPwr2(x) (((x)&((x)-1))==0)

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_FIXMATH_H_ */
