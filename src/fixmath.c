#include "mem.h"
#include "fixmath.h"
#include <math.h>

bbU16* bbgpDivTab = NULL;
bbS16* bbgpSinTab = NULL;

bbERR bbInitFixMath(void)
{
    bbUINT i;
    //long   sx, sy, pos;

    if ((bbgpDivTab = (bbU16*) bbMemAlloc( bbDIVTABSIZE*sizeof(bbU16))) == NULL)
    {
        return bbENOMEM;
    }

    if ((bbgpSinTab = (bbS16*) bbMemAlloc( (bbSINTABSIZE+1)*sizeof(bbS16))) == NULL)
    {
        bbMemFreeNull((void**)&bbgpDivTab);
        return bbENOMEM;
    }

    bbgpDivTab[0] = 0;
    bbgpDivTab[1] = 0xFFFFU;
    for (i=2; i<bbDIVTABSIZE; ++i)
    {
        bbgpDivTab[i] = (bbU16) ((1UL<<bbDIVTABPREC) / i);
    }

    // These parameters result in 822 inner loops for 256 sine values.

    #if 0

    #define bbSINAMP 67460L
    #define bbSINSTEP 819L
    #define bbSINSHIFT1 4
    #define bbSINSHIFT2 12

    sx  = 0;
    sy  = 4096;
    pos = 0;
    i   = 0;
    do
    {
        const long sin = (sx * (((1L<<bbSINTABPREC)<<16)/bbSINAMP)) >> 16;

        bbgpSinTab[i] = (short) sin;
        bbgpSinTab[bbSINTABPER-i] = (short)(-sin);

        i++;
        pos += bbSINSTEP;

        while (pos >= (bbSINTABPER>>1))
        {
            pos -= bbSINTABPER>>1;
            sx += sy>>bbSINSHIFT1;
            sy -= sx>>bbSINSHIFT2;
        }
    } while (i <= (bbSINTABPER>>1));

    #else
    {
        double w = 0;
        double step = 6.283185307179586476925286766559f / (double)bbSINTABPER;
        for (i=0; i<bbSINTABPER; ++i)
        {
            double sinValD = sin(w++ * step) * (double)((1L<<bbSINTABPREC)-1);
            long sinVal = (long)sinValD;
            bbASSERT((sinVal <= 32687) && (sinVal>=-32768));
            bbgpSinTab[i] = (short)sinVal;
        }
    }
    #endif

    for (i=0; i<=(bbSINTABPER>>2); i++) /* repeat index 0 at the end */
    {
        bbgpSinTab[bbSINTABPER+i] = bbgpSinTab[i];
    }

    return bbEOK;
}

void bbDestroyFixMath(void)
{
    bbMemFreeNull((void**)&bbgpSinTab);
    bbMemFreeNull((void**)&bbgpDivTab);
}

bbU32 bbSqrFast(bbU32 x)
{
    bbU32 xroot = 0; 
    bbU32 m2 = 1L<<30;

    do { 
        const bbU32 _x2 = xroot + m2; 
        xroot >>= 1; 
        if (_x2 <= x)  x-=_x2, xroot+=m2;
    } while ((m2=(m2>>2))!=0); 
    
    if (xroot<x)  xroot+=1;

    return xroot;
}

bbU32 bbPyt(long dx, long dy)
{
    bbU32 result;
    bbUINT shift = 0;

    if (dx<0) dx=-dx; 
    if (dy<0) dy=-dy;
    while ((dx & 0xffff8000UL) || (dy & 0xffff8000UL)) shift++, dx>>=1, dy>>=1;

    result = bbSqrFast(dx*dx + dy*dy);
 
    return shift ? (result << shift) + (1 << (shift - 1)) : result;
}

bbUINT bbCountSetBits(bbU32 x)
{
    /*
    MOV R3,#&33
    ORR R3,R3,R3,LSL #8
    ORR R3,R3,R3,LSL #16    ; 0x33333333
    EOR R5,R3,R3,LSL #1     ; 0x55555555
    AND R1,R5,R0,LSR #1
    SUB R0,R0,R1            ; x = x - ((x >> 1) & 0x55555555);
    AND R1,R3,R0,LSR #2     ; t = ((x >> 2) & 0x33333333);
    AND R0,R0,R3
    ADD R0,R0,R1            ; x = (x & 0x33333333) + t;
    ADD R0,R0,R0,LSR #4
    MOV R1,#&0F
    ORR R1,R1,R1,LSL #8
    ORR R1,R1,R1,LSL #16    ; 0x0F0F0F0F
    AND R0,R0,R1            ; x = (x + (x >> 4)) & 0x0F0F0F0F;
    ADD R0,R0,R0,LSL #8
    ADD R0,R0,R0,LSL #16
    MOV R0,R0,LSR #24       ; return x >> 24;
    */

    #if bbCPU==bbCPU_ARM

    register bbU32 c3;
    register bbU32 cf = 0x0F;
    cf = cf | (cf<<8);
    cf = cf | (cf<<16);
    c3 = cf & (cf>>4);
    c3 = c3 | (c3<<4);

    x = x - ((x >> 1) & (c3^(c3<<1)));
    x = (x & c3) + ((x >> 2) & c3);
    x = (x + (x >> 4)) & cf;
    x = x + (x << 8);
    x = x + (x << 16);
    return (bbUINT)(x >> 24);

    #else

    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x << 8);
    x = x + (x << 16);
    return (bbUINT)(x >> 24);

    #endif
}

bbUINT bbGetTopBit(bbU32 val)
{
    register bbUINT pos = 0;

    if (val >= 0x10000UL) pos = 16, val >>= 16;
    if (val & 0xFF00U)    pos |= 8, val >>= 8;
    if (val & 0xF0U)      pos |= 4, val >>= 4;
    if (val & 0xCU)       pos |= 2, val >>= 2;
    if (val & 0x2)        pos |= 1;

    return pos;
}

/*
unsigned char flip[] = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
unsigned long turnbits( unsigned long n ) {
  unsigned long f, o;
  for( o=28,f=0;n;n>>=4,o+=4) f|=flip[n&15]<<o;
}

        pix = *(bbU32*)&buf[4];
        pix = ((pix&0xAAAAAAAA)>>1) | ((pix&0x55555555)<<1);
        pix = ((pix&0xCCCCCCCC)>>2) | ((pix&0x33333333)<<2);
        pix = ((pix&0xF0F0F0F0)>>4) | ((pix&0x0F0F0F0F)<<4);
        pix = ((pix&0xFF00FF00)>>8) | ((pix&0x00FF00FF)<<8);
        pix = (pix<<16) | (pix>>16);                        
        pTmp[1] = pix;

[18:12] *eRdgEiSt* hmm, man koennte ooch return ((n>>0)&15]<<28)+(f[(n>>4)&15]<<24)+(f[(n>>8)&15]<<20)+(f[(n>>12)&15]<<1
6)+(f[(n>>16)&15]<<12)+(f[(n>>20)&15]<<8)+(f[(n>>24)&15]<<4)+(f[(n>>28)&15]<<0)
*/
