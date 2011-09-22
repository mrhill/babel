#ifndef bbBABEL_BIT_H_
#define bbBABEL_BIT_H_

/** @file
	Bit arrays and streams.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroup groupBit Bit arrays.
@{*/

/** Test for a bit in a bit array to be set.
    @param pBits  (bbU32*) Pointer to bitvector
    @param bitpos (bbUINT) Bit position
    @retval true Bit set
    @retval false Bit cleared
*/
#define bbBitTst(pBits, bitpos) (((const bbU32*)(pBits))[(bitpos)>>5]&(1UL<<((bitpos)&31)))

/** Set a bit in a bit array.
    @param pBits  (bbU32*) Pointer to bitvector
    @param bitpos (bbUINT) Bit position to be set
*/
#define bbBitSet(pBits, bitpos) ((bbU32*)(pBits))[(bitpos)>>5]|=1UL<<((bitpos)&31)

/** Clear a bit in a bit array.
    @param pBits  (bbU32*) Pointer to bitvector
    @param bitpos (bbUINT) Bit position to be cleared
*/
#define bbBitClr(pBits, bitpos) ((bbU32*)(pBits))[(bitpos)>>5]&=~(1UL<<((bitpos)&31))

/*@}*/

/** @addtogroup groupBS Bitstreams.
@{*/

/** Bitstream low-level read cursor. */
typedef struct
{
    bbU32        cache;
    int          pos;
    bbU32 const* ptr;

} bbBSR;

/** Get an unsigned integer of given bitsize from stream, inline macro.
    @param bs      (bbBSR)  Bitstream read cursor
    @param val     (bbU32)  Variable to return value
    @param bitsize (bbUINT) Size in bits (0..32)
*/
#define bbBSRGETU(bs, val, bitsize) { \
    int const bitsize_d = bitsize - (bs).pos; \
    val = (bs).cache; \
    if (bitsize_d <= 0) { \
        val ^= ((bs).cache >>= (bitsize)) << (bitsize); \
    } else { \
        (bs).cache = bbLDA32LE((bs).ptr); (bs).ptr++; \
        val |= (bs).cache << (bs).pos; \
        (bs).cache >>= bitsize_d; \
        val &= 0xFFFFFFFFUL << (bitsize); \
        (bs).pos += 32; \
    } \
    (bs).pos -= (bitsize); \
}

/** Get an unsigned integer of given bitsize from stream.
    @param pBS     Bitstream read cursor
    @param bitsize Size in bits (0..32)
    @return Value with top bits set 0
*/
bbU32 bbBSGetU(bbBSR* const pBS, bbUINT const bitsize);

/** Get a signed integer of given bitsize from stream, inline macro.
    @param bs      (bbBSR)  Bitstream read cursor
    @param val     (bbS32)  Variable to return sign-extended value
    @param bitsize (bbUINT) Size in bits (0..32)
*/
#define bbBSRGETS(bs, val, bitsize) { \
    int const bitsize_i = 32 - bitsize; \
    int const bitsize_d = bitsize - (bs).pos; \
    if (bitsize_d <= 0) { \
        val = (bbS32)((bs).cache << bitsize_i) >> bitsize_i; \
        (bs).cache >>= (bitsize); \
    } else { \
        val = (bs).cache; \
        (bs).cache = bbLDA32LE((bs).ptr); (bs).ptr++; \
        val |= (bs).cache << (bs).pos; \
        (bs).cache >>= bitsize_d; \
        val = (bbS32)(val << bitsize_i) >> bitsize_i; \
        (bs).pos += 32; \
    } \
    (bs).pos -= (bitsize); \
}

/** Get a signed integer of given bitsize from stream.
    @param pBS     Bitstream read cursor
    @param bitsize Size in bits (0..32)
    @return Value with top bits sign-extended
*/
bbS32 bbBSGetS(bbBSR* const pBS, bbUINT const bitsize);

/** Get 1 bit from stream, inline macro.
    @param bs  (bbBSR)  Bitstream read cursor
    @param val (bbUINT) Variable to return value
*/
#define bbBSRGET1(bs, val) \
    if ((bs).pos == 0) { \
        (bs).cache = bbLDA32LE((bs).ptr); (bs).ptr++; \
        (bs).pos = 32; \
    } \
    val = (bs).cache & 1; \
    (bs).cache >>= 1; \
    (bs).pos--;

/** Get 1 bit from stream.
    @param pBS     Bitstream read cursor
    @return Value with bit in bit 0.
*/
bbUINT bbBSRGet1(bbBSR* const pBS);

/** Skip given number of bits between 0 and 32 in stream, inline macro.
    @param bs      (bbBSR)  Bitstream read cursor
    @param bitsize (bbUINT) Size in bits (0..32)
*/    
#define bbBSRSKIP(bs, bitsize) \
    if ((int)(bitsize) <= (bs).pos) { \
        (bs).cache >>= (bitsize); \
    } else { \
        (bs).cache = bbLDA32LE((bs).ptr); (bs).ptr++; \
        (bs).cache >>= (bitsize) - (bs).pos; \
        (bs).pos += 32; \
    } \
    (bs).pos -= (bitsize);

/** Skip 1 bit in stream, inline macro.
    @param bs (bbBSR) Bitstream read cursor
*/
#define bbBSRSKIP1(bs) \
    if ((bs).pos == 0) { \
        (bs).cache = bbLDA32LE((bs).ptr); (bs).ptr++; \
        (bs).pos = 32; \
    } \
    (bs).cache >>= 1; \
    (bs).pos--;

/** Skip given number of bits in stream, inline macro.
    @param bs      (bbBSR) Bitstream read cursor
    @param bitsize (bbU32) Size in bits
*/
#define bbBSRSKIPFAR(bs, bitsize) { \
    int const a = (bitsize) & 31U; \
    (bs).ptr += (bitsize)>>5; \
    if (a <= (bs).pos) { \
        (bs).cache >>= a; \
    } else { \
        (bs).cache = bbLDA32LE((bs).ptr); (bs).ptr++; \
        (bs).cache >>= a - (bs).pos; \
        (bs).pos += 32; \
    } \
    (bs).pos -= a; \
}

/** Skip given number of bits in stream.
    @param pBS     Bitstream read cursor
    @param bitsize Number of bits
*/
void bbBSRSkip(bbBSR* const pBS, bbU32 const bitsize);

/** BitStream low-level write cursor. */
typedef struct
{
    bbU32* ptr;
    int    pos;

} bbBSW;

/** Write value of given bitsize to stream, inline macro.
    This call can be used for both, signed and unsigned values.
    bbBSEnsure() must be called before to ensure enough space in the bitstream buffer.
    @param bs  (bbBSW) Bitstream write cursor
    @param val (bbU32) Value to write, LSB's are written first
    @param bitsize (bbUINT) Number of bits to write from value, must be between 0 and 32.
*/
#define bbBSWPUT(bs, val, bitsize) { \
    bbU32 mask = 0xFFFFFFFFUL>>(32-(bitsize)); \
    bbU32 dst  = (bbLDA32LE((bs).ptr)&~(mask<<(bs).pos))|(((val)&mask)<<(bs).pos); \
    bbSTA32LE((bs).ptr,dst); \
    if((((bs).pos += (bitsize))>32)) { \
        (bs).pos&=31; ++(bs).ptr; mask >>= (bitsize)-(bs).pos; \
        dst = (bbLDA32LE((bs).ptr)&~mask)|((val)>>((bitsize)-(bs).pos)); \
        bbSTA32LE((bs).ptr,dst); \
    } else \
        (bs).pos&=31; \
    }

/** Write value of given bitsize to stream.
    This call can be used for both, signed and unsigned values.
    @param pBS Bitstream write cursor
    @param val Value to write, LSB's are written first
    @param bitsize Number of bits to write from value, must be between 0 and 32.
*/
void bbBSWPut(bbBSW* const pBS, bbU32 const val, bbUINT const bitsize);

/** Write a 1 bit value to stream, inline macro.
    This call can be used for both, signed and unsigned values.
    @param bs  (bbBSW) Bitstream write cursor
    @param val (bbUINT) Value to write, LSB is written
*/
#define bbBSWPUT1(bs, val) \
    bbSTA32LE((bs).ptr,(bbLDA32LE((bs).ptr)&~(1UL<<(bs).pos))|((val)<<(bs).pos));\
    if(!((++(bs).pos)&31)) ++(bs).ptr;

/** Write a 1 bit value to stream.
    @param pBS Bitstream write cursor
    @param val Value to write, LSB is written
*/
void bbBSWPut1(bbBSW* const pBS, bbUINT const val);

/** Skip given number of bits in stream, inline macro.
    @param bs  (bbBSW) Bitstream write cursor
    @param bitsize (bbU32) Number of bits to skip
*/
#define bbBSWSKIP(bs, bitsize) \
    (bs).ptr += ((bs).pos+(bitsize))>>5; \
    (bs).pos  = ((bs).pos+(bitsize))&31;

/** Skip given number of bits in stream.
    @param pBS Bitstream write cursor
    @param bitsize Number of bits to skip
*/
void bbBSWSkip(bbBSW* const pBS, bbU32 const bitsize);

/** Bitstream container with read/write access and memory management.
    This container manages memory for stream storage, and it provides
    independent read and write cursors for the stream.
*/
typedef struct
{
    bbU32* pBuf;    //!< Pointer to heap block
    bbU32* pBufEnd; //!< Pointer end of heap block
    bbBSR  bsr;     //!< Read cursor
    bbBSW  bsw;     //!< Write cursor

} bbBS;

/** Get current write position for bitstream container in bits.
    @param bs (bbBS*) Bitstream container
    @return (bbU32) Write offset from beginning of stream in number of bits
*/
#define bbBSGetPosW(pBS) (((bbU32)(bbUPTR)(pBS)->bsw.ptr-(bbU32)(bbUPTR)(pBS)->pBuf)*8+(pBS)->bsw.pos)

/** Initialise a bitstream container for read/write access.
    @param pBS Bitstream container
*/
void bbBSInit(bbBS* const pBS);

/** Destroy bitstream container.
    @param pBS Bitstream container
*/
void bbBSDestroy(bbBS* const pBS);

/** Write value of given size.
    If bbBSEnsure() was called before to ensure enough space in the bitstream buffer,
    this call will always succeed, and return error does not need to be checked.
    @param pBS Bitstream container
    @param val Value, LSB's are written first
    @param bitsize 
*/
bbERR bbBSPut(bbBS* const pBS, bbU32 const val, bbUINT const bitsize);

/** Write value of given size.
    Like bbBSPut() but without buffer size check.
    bbBSEnsure() must be called before to ensure enough space in the bitstream buffer.
    @param pBS Bitstream container
    @param val Value, LSB's are written first
    @param bitsize 
*/
void bbBSPutUnsafe(bbBS* const pBS, bbU32 const val, bbUINT const bitsize);

/** Ensure memory of given bitsize to be allocated at current write position.
    If this call returns successfully, the write cursor (\a pBS's bbBS::bsw)
    may be used directly for writing the ensured number of bits.
    @param pBS Bitstream container
    @param bitsize Number of bits to ensure
*/
bbERR bbBSEnsure(bbBS* const pBS, bbU32 bitsize);

/** Seek read cursor to bit offset.
    @param pBS Bitstream container
    @param pos Bit position relative to buffer start, must not exceed buffer size
*/
void bbBSSeekR(bbBS* pBS, bbU32 pos);

/** Seek read cursor to bit offset.
    @param pBS Bitstream container
    @param pos Bit position relative to buffer start, must not exceed buffer size
*/
void bbBSSeekW(bbBS* pBS, bbU32 pos);

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_BIT_H_ */

