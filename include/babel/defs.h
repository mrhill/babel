#ifndef bbBABEL_DEFS_H_
#define bbBABEL_DEFS_H_

/** @file
    Definitions and types global for babellib.

    Coding conventions:
     - all entities with prefix 'bb'
     - defines:  bbDEFINE
     - typedefs: bbTYPE
     - exported global variables: bbgpDivTab
     - exported global functions: bbFunctionName
     - classes: bbClassName
     - pointers: pPointer
*/

#ifdef  __cplusplus
extern "C" {
#endif

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#define bbCFGENC_SBC   0
#define bbCFGENC_DBC   1
#define bbCFGENC_UTF8  2
#define bbCFGENC_UTF16 3

/** Constant identifying ASCII codepage. */
#define bbCPG_ASCII 0
/** Constant identifying Unicode 4.0 codepage. */
#define bbCPG_UNICODE 1
/** Constant identifying Unicode 1.1 (ISO 10646) codepage. */           
#define bbCPG_UCS 2
/** Constant identifying ISO-8859-1 (Latin 1) codepage. */
#define bbCPG_ISO8859_1 3
#define bbCPG_ISO8859_2 4
#define bbCPG_ISO8859_3 5
#define bbCPG_ISO8859_4 6
#define bbCPG_ISO8859_5 7
#define bbCPG_ISO8859_7 8
#define bbCPG_ISO8859_9 9
#define bbCPG_ISO8859_10 10
#define bbCPG_ISO8859_13 11
#define bbCPG_ISO8859_14 12
#define bbCPG_ISO8859_15 13
#define bbCPG_ISO8859_16 14
#define bbCPG_JIS_X_0213_2000 15
#define bbCPG_JIS_X_0208_1990 16
#define bbCPGCOUNT 17

#define bbCPGNAMES \
    bbT("ASCII"), \
    bbT("Unicode"), \
    bbT("UCS"), \
    bbT("ISO8859-1"), \
    bbT("ISO8859-2"), \
    bbT("ISO8859-3"), \
    bbT("ISO8859-4"), \
    bbT("ISO8859-5"), \
    bbT("ISO8859-7"), \
    bbT("ISO8859-9"), \
    bbT("ISO8859-10"), \
    bbT("ISO8859-13"), \
    bbT("ISO8859-14"), \
    bbT("ISO8859-15"), \
    bbT("ISO8859-16"), \
    bbT("JIS-X-0213-2000"), \
    bbT("JIS-X-0208-1990")

/** Internal character code page.
    Choose one from bbCPG_*
*/
#ifndef bbCPG
#define bbCPG bbCPG_UNICODE
#endif

#define bbSTRCONV_INTERNAL 0
#define bbSTRCONV_ICONV    1
#define bbSTRCONV_WIN      2

/** Select implementation of bbCCONV functions.
    Undefine to for auto-detection.
*/
#ifndef bbSTRCONV
#define bbSTRCONV bbSTRCONV_INTERNAL
#endif


/* try OS auto detection */
#define bbOS_NONE    0
#define bbOS_POSIX   1
#define bbOS_WIN32   2
#define bbOS_WINCE   3
#define bbOS_SYMBIAN 4
#define bbOS_PALMOS  5
#define bbOS_ITRON   6
#define bbOS_RISCOS  7
#define bbOS_ANDROID 8
#define bbOS_QT      9

#ifndef bbOS
#if defined(bbQT)
#define bbOS bbOS_QT
#elif defined(UNDER_CE)
#define bbOS bbOS_WINCE
#elif defined(WIN32)
#define bbOS bbOS_WIN32
#elif defined(_EPOC)
#define bbOS bbOS_SYMBIAN
#elif defined(__MWERKS__)
#define bbOS bbOS_PALMOS
#elif defined(__FreeBSD__) || defined(ANDROID)
#define bbOS bbOS_POSIX
#else
#error Could not autodetect OS
#endif
#endif /* bbOS */

#ifdef  __cplusplus
}
#endif

#if bbOS == bbOS_QT

#include <QtGlobal>
#include <stdlib.h> /* for malloc */
#include <stdio.h>  /* for printf, etc. */
#include <stdarg.h> /* for va_list */
#include <string.h> /* for memset etc. */
#include <time.h>
#if defined(UNDER_CE) || defined(WIN32) || defined(WIN64)
#include <CRTDBG.H>
#endif

#elif (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

#define WIN32_LEAN_AND_MEAN /* exclude rare stuff */
#include <windows.h>
#include <CRTDBG.H>
#include <stdlib.h> /* for malloc */
#include <stdio.h>  /* for printf, etc. */
#include <stdarg.h> /* for va_list */
#include <time.h>

#elif bbOS == bbOS_PALMOS

#include <PalmOS.h>

#elif bbOS == bbOS_POSIX

#include <stdlib.h> /* for malloc */
#include <stdio.h>  /* for printf, etc. */
#include <stdarg.h> /* for va_list */
#include <string.h> /* for memset etc. */
#include <time.h>
#ifndef NULL
#define NULL ((void*)0)
#endif

#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* try CPU auto detection */
#define bbCPU_UNDEFINED 0
#define bbCPU_X86  1
#define bbCPU_ARM  2
#define bbCPU_MIPS 3
#define bbCPU_SHX  4
#define bbCPU_68K  5

#ifndef bbCPU
#if defined(__MWERKS__)
    #if defined(__INTEL__) || defined(__i386__)
        #define bbCPU bbCPU_X86
    #else
        #define bbCPU bbCPU_68K
    #endif
#elif defined(WIN32)
    #define bbCPU bbCPU_X86
#else
    #define bbCPU bbCPU_UNDEFINED
#endif
#endif /* bbCPU */


/* try CPU endianess auto detection */
#define bbCPUE_LE 0
#define bbCPUE_BE 1

#ifndef bbCPUE
#if bbOS==bbOS_QT
  #if Q_BYTE_ORDER == Q_BIG_ENDIAN
    #define bbCPUE bbCPUE_BE
  #else
    #define bbCPUE bbCPUE_LE
  #endif
#elif bbCPU==bbCPU_68K
  #define bbCPUE bbCPUE_BE
#else
  #define bbCPUE bbCPUE_LE
#endif
#endif /* bbCPUE */


/* try sizeof() detections */
#ifndef bbSIZEOF_INT
#if defined(WIN32) || defined(__CYGWIN__) || defined(_X86_) || defined(ANDROID) || (bbOS==bbOS_QT)
#define bbSIZEOF_INT 4
#else
#error Cannot detect bbSIZEOF_INT
#endif
#endif /* bbSIZEOF_INT */

#ifndef bbSIZEOF_UPTR
#if (bbOS==bbOS_QT)
#define bbSIZEOF_UPTR QT_POINTER_SIZE
#elif defined(WIN32) || defined(__CYGWIN__) || defined(_X86_) || defined(ANDROID)
#define bbSIZEOF_UPTR 4
#elif defined(WIN64)
#define bbSIZEOF_UPTR 8
#else
#error Cannot detect bbSIZEOF_UPTR
#endif
#endif /* bbSIZEOF_UPTR */


#ifdef _MSC_VER
#define __attribute__(n)
#endif

/*@}*/

/** @name Constants and macros specifying character encoding schemes.
@{*/

/** Constant identifying SBC (single byte characters) encoding. */
#define bbENC_SBC 0
/** Constant identifying unicode UTF8 encoding. */
#define bbENC_UTF8 1
/** Constant identifying DBC (double byte characters) encoding, little endian. */
#define bbENC_DBCLE 2
/** Constant identifying DBC (double byte characters) encoding, big endian. */
#define bbENC_DBCBE 3
/** Constant identifying unicode UTF16 encoding little endian. */
#define bbENC_UTF16LE 4
/** Constant identifying unicode UTF16 encoding big endian. */
#define bbENC_UTF16BE 5
/** Constant identifying unicode UTF32 encoding little endian. */
#define bbENC_UTF32LE 6
/** Constant identifying unicode UTF32 encoding big endian. */
#define bbENC_UTF32BE 7
/** Constant identifying unicode UTF7 encoding. */
#define bbENC_UTF7 8
/** Constant identifying unicode SCSU encoding. */
#define bbENC_SCSU 9
/** Constant identifying number of known encodings. */
#define bbENCCOUNT 10

/** Array defining bytes size of a character code unit for each encoding.
    bbENC_* is used as an index in to this array. Sizes must be power of 2.
    @internal
*/
#define bbENCCUSIZES { 1, 1, 2, 2, 2, 2, 4, 4, 1, 1 }

/** Test if encoding is big-endian. Only valid for encodings with CU size bigger than 1 byte. */
#define bbENCISBE(enc) (enc&1)

#if bbCPUE==bbCPUE_LE
#define bbENC_DBC bbENC_DBCLE
#define bbENC_UTF16 bbENC_UTF16LE
#define bbENC_UTF32 bbENC_UTF32LE
#else
#define bbENC_DBC bbENC_DBCBE
#define bbENC_UTF16 bbENC_UTF16BE
#define bbENC_UTF32 bbENC_UTF32BE
#endif

/** Internal character encoding.
    Choose one from bbCFGENC_*
*/
#ifndef bbCFGENC
#if bbOS==bbOS_QT
#define bbCFGENC bbCFGENC_UTF8
#elif bbOS==bbOS_WIN32
  #ifdef UNICODE
    #define bbCFGENC bbCFGENC_UTF16
  #else
    #define bbCFGENC bbCFGENC_UTF8
  #endif
#else
#define bbCFGENC bbCFGENC_UTF8
#endif
#endif

#if bbCFGENC == bbCFGENC_SBC
#define bbENC bbENC_SBC
#elif bbCFGENC == bbCFGENC_DBC
#define bbENC bbENC_DBC
#elif bbCFGENC == bbCFGENC_UTF8
#define bbENC bbENC_UTF8
#elif bbCFGENC == bbCFGENC_UTF16
#define bbENC bbENC_UTF16
#else
#error
#endif


/** Constant, character code point encoding modifier: none. */
#define bbENCMOD_NONE 0
/** Constant, character code point encoding modifier: little endian byte order. */
#define bbENCMOD_LE 1
/** Constant, character code point encoding modifier: big endian byte order. */
#define bbENCMOD_BE 2
/** Constant, character code point encoding modifier: native byte order. */
#if bbCPUE == bbCPUE_LE
#define bbENCMOD_NE bbENCMOD_LE
#else
#define bbENCMOD_NE bbENCMOD_BE
#endif

#if bbSTRCONV==bbSTRCONV_WIN

#error not implemented

#else

/** Constant, character encoding specifier: native encoding. */
#if (bbENC == bbENC_DBC) || (bbENC == bbENC_UTF16) || (bbENC == bbENC_UTF32)
#define bbCE_INTERNAL (bbENC | (bbCPG<<8) | (bbENCMOD_NE<<6))
#else
#define bbCE_INTERNAL (bbENC | (bbCPG<<8))
#endif

#define bbCE_ASCII      (bbENC_SBC      | (bbCPG_ASCII<<8))
#define bbCE_ISO8859_1  (bbENC_SBC      | (bbCPG_ISO8859_1<<8))
#define bbCE_ISO8859_2  (bbENC_SBC      | (bbCPG_ISO8859_2<<8))
#define bbCE_ISO8859_3  (bbENC_SBC      | (bbCPG_ISO8859_3<<8))
#define bbCE_ISO8859_4  (bbENC_SBC      | (bbCPG_ISO8859_4<<8))
#define bbCE_ISO8859_5  (bbENC_SBC      | (bbCPG_ISO8859_5<<8))
#define bbCE_ISO8859_7  (bbENC_SBC      | (bbCPG_ISO8859_7<<8))
#define bbCE_ISO8859_9  (bbENC_SBC      | (bbCPG_ISO8859_9<<8))
#define bbCE_ISO8859_10 (bbENC_SBC      | (bbCPG_ISO8859_10<<8))
#define bbCE_ISO8859_13 (bbENC_SBC      | (bbCPG_ISO8859_13<<8))
#define bbCE_ISO8859_14 (bbENC_SBC      | (bbCPG_ISO8859_14<<8))
#define bbCE_ISO8859_15 (bbENC_SBC      | (bbCPG_ISO8859_15<<8))
#define bbCE_ISO8859_16 (bbENC_SBC      | (bbCPG_ISO8859_16<<8))
#define bbCE_UTF8       (bbENC_UTF8     | (bbCPG_UNICODE<<8))
#define bbCE_UTF16      (bbENC_UTF16    | (bbCPG_UNICODE<<8) | (bbENCMOD_NONE<<6))
#define bbCE_UTF16LE    (bbENC_UTF16    | (bbCPG_UNICODE<<8) | (bbENCMOD_LE<<6))
#define bbCE_UTF16BE    (bbENC_UTF16    | (bbCPG_UNICODE<<8) | (bbENCMOD_BE<<6))
#define bbCE_UTF16NE    (bbENC_UTF16    | (bbCPG_UNICODE<<8) | (bbENCMOD_NE<<6))
#define bbCE_UTF32      (bbENC_UTF32    | (bbCPG_UNICODE<<8) | (bbENCMOD_NONE<<6))
#define bbCE_UTF32LE    (bbENC_UTF32    | (bbCPG_UNICODE<<8) | (bbENCMOD_LE<<6))
#define bbCE_UTF32BE    (bbENC_UTF32    | (bbCPG_UNICODE<<8) | (bbENCMOD_BE<<6))
#define bbCE_UTF32NE    (bbENC_UTF32    | (bbCPG_UNICODE<<8) | (bbENCMOD_NE<<6))
#define bbCE_UTF7       (bbENC_UTF7     | (bbCPG_UNICODE<<8))
#define bbCE_SCSU       (bbENC_SCSU     | (bbCPG_UNICODE<<8))
#define bbCE_UCS2       (bbENC_DBC      | (bbCPG_UCS<<8) | (bbENCMOD_NONE<<6))
#define bbCE_UCS2LE     (bbENC_DBC      | (bbCPG_UCS<<8) | (bbENCMOD_LE<<6))
#define bbCE_UCS2BE     (bbENC_DBC      | (bbCPG_UCS<<8) | (bbENCMOD_BE<<6))
#define bbCE_UCS2NE     (bbENC_DBC      | (bbCPG_UCS<<8) | (bbENCMOD_NE<<6))

#endif

#define bbCE_ENCMASK 63
#define bbCE_CPGSHIFT 8

/** Get code point encoding specifier from character encoding specifier.
    @param ce (bbUINT) Character encoding
    @return (bbUINT) Code point encoding
*/
#define bbCE_GETENC(ce) ((ce)&bbCE_ENCMASK)

/** Get codepage specifier from character encoding specifier.
    @param ce (bbUINT) Character encoding
    @return (bbUINT) Codepage
*/
#define bbCE_GETCPG(ce) ((ce)>>8)

#if bbCPG == bbCPG_ASCII
#define bbCPG_CHARCOUNT 0x80U
#elif bbCPG == bbCPG_UNICODE
#define bbCPG_CHARCOUNT 0x110000UL
#elif bbCPG == bbCPG_UCS
#define bbCPG_CHARCOUNT 0x10000UL
#elif bbCPG <= bbCPG_ISO8859_16
#define bbCPG_CHARCOUNT 0x100U
#else
#error not implemented
#endif

/*@}*/

/** @addtogroup groupBasic Basic types and macros.
@{*/

/** 8 Bit character code unit. */
typedef char bbCHAR8;

/** 16 Bit character code unit. */
#if bbOS==bbOS_WIN32 || bbOS==bbOS_WINCE
typedef wchar_t bbCHAR16;
#else
typedef unsigned short bbCHAR16;
#endif

typedef signed int     bbINT;
typedef unsigned int   bbUINT;

typedef unsigned char  bbU8;
typedef unsigned short bbU16;
#if bbSIZEOF_INT==4 && !defined(_MSC_VER)
typedef unsigned int   bbU32;
typedef signed int     bbS32;
#else
typedef unsigned long  bbU32;
typedef signed long    bbS32;
#endif
typedef signed char    bbS8;
typedef signed short   bbS16;

#if defined(_MSC_VER)    /* MS VC compiler? */
typedef unsigned __int64    bbU64;
typedef __int64             bbS64;
#else
typedef unsigned long long  bbU64;
typedef long long           bbS64;
#endif

/** 32 Bit character code unit. */
typedef bbU32 bbCHAR32;

#if defined(bbDOXYGEN)
    typedef bbCHAR8  bbCHAR;    /**< Character code unit for configured internal encoding. */
    typedef bbCHAR32 bbCHARCP;  /**< Character code point for configured internal encoding. */
    #define bbSIZEOF_CHAR 2     /**< Byte size of character code unit type bbCHAR. */
    #define bbSIZEOF_CHARCP 4   /**< Byte size of character code point type bbCHARCP. */
    #define bbT(t) t            /**< Specify a static character string. */

#elif bbENC==bbENC_SBC
    typedef bbCHAR8  bbCHAR;
    typedef bbCHAR8  bbCHARCP;
    #define bbSIZEOF_CHAR 1
    #define bbSIZEOF_CHARCP 1
    #define bbT(t) (bbCHAR8*)t

#elif bbENC==bbENC_DBC

    typedef bbCHAR16 bbCHAR;
    typedef bbCHAR16 bbCHARCP;
    #define bbSIZEOF_CHAR 2
    #define bbSIZEOF_CHARCP 2
    #define bbT(t) L##t

#elif bbENC==bbENC_UTF8

    typedef char     bbCHAR;
    typedef bbCHAR32 bbCHARCP;
    #define bbSIZEOF_CHAR 1
    #define bbSIZEOF_CHARCP 4
    #define bbT(t) t

#elif bbENC==bbENC_UTF16

    typedef bbCHAR16 bbCHAR;
    typedef bbCHAR32 bbCHARCP;
    #define bbSIZEOF_CHAR 2
    #define bbSIZEOF_CHARCP 4
    #define bbT(t) L##t

#elif bbENC==bbENC_UTF32

    typedef bbCHAR32 bbCHAR;
    typedef bbCHAR32 bbCHARCP;
    #define bbSIZEOF_CHAR 4
    #define bbSIZEOF_CHARCP 4
    #define bbT(t) L##t

#endif /* bbENC */

#define bbS64MIN ((bbS64)((bbU64)1<<63))
#define bbS64MAX ((bbS64)(((bbU64)1<<63)-1))

#define bbARRSIZE(arr) (sizeof(arr)/sizeof(arr[0]))

/** Unsigned integer with the size of a pointer. */
typedef unsigned long bbUPTR;

typedef unsigned int bbERR; /**< Error code or function return code. */

/** Handle to a heap block.
    @see groupHeap.
*/
typedef bbUINT bbHEAPH;

typedef struct bbENCSTATE bbENCSTATE;

/** Type for variable argument list.
    See ANSI va_list.
*/
#if bbOS == bbOS_PALMOS
#define bbVALIST char*
#else
#define bbVALIST va_list
#endif

/** Get pointer to first parameter of variable argument list.
    See ANSI va_start.
*/
#if bbOS == bbOS_PALMOS
#define bbVASTART(ap,v) ( ap = (_Palm_va_list)&v + sizeof(v) )
#else
#define bbVASTART(ap,v) va_start(ap,v)
#endif

/** Reset pointer of variable argument list.
    See ANSI va_end.
*/
#if bbOS == bbOS_PALMOS
#define bbVAEND(ap)
#else
#define bbVAEND(ap) va_end(ap)
#endif

/** Marshal bbU32 (32 bit) to little endian bytes */
#define bbMA32LE(val) (bbU8)val,(bbU8)(val>>8),(bbU8)(val>>16),(bbU8)(val>>24)
/** Marshal bbU32 (32 bit) to big endian bytes */
#define bbMA32BE(val) (bbU8)(val>>24),(bbU8)(val>>16),(bbU8)(val>>8),(bbU8)val
/** Marshal bbU32 (24 bit) to little endian bytes */
#define bbMA24LE(val) (bbU8)val,(bbU8)(val>>8),(bbU8)(val>>16)
/** Marshal bbU32 (24 bit) to big endian bytes */
#define bbMA24BE(val) (bbU8)(val>>16),(bbU8)(val>>8),(bbU8)val
/** Marshal bbU16 (16 bit) to little endian bytes */
#define bbMA16LE(val) (bbU8)val,(bbU8)(val>>8)
/** Marshal bbU16 (16 bit) to big endian bytes */
#define bbMA16BE(val) (bbU8)(val>>8),(bbU8)val

#if bbCPU != bbCPU_X86
/** Load little endian bbU16 (16 bit) from unaligned address */
#define bbLD16LE(adr) ((bbU32)*((bbU8*)(adr))|((bbU32)*(((bbU8*)(adr))+1)<<8))
/** Load little endian 24 bit word (low 24 bit of a bbU32) from unaligned address */
#define bbLD24LE(adr) ((bbU32)*((bbU8*)(adr))|((bbU32)*(((bbU8*)(adr))+1)<<8)|((bbU32)*(((bbU8*)(adr))+2)<<16))
/** Load little endian bbU32 (32 bit) from unaligned address */
#define bbLD32LE(adr) ((bbU32)*((bbU8*)(adr))|((bbU32)*(((bbU8*)(adr))+1)<<8)|((bbU32)*(((bbU8*)(adr))+2)<<16)|((bbU32)*(((bbU8*)(adr))+3)<<24))
/** Load little endian bbU64 (64 bit) from unaligned address */
#define bbLD64LE(adr) ((bbU64)bbLD32LE(adr)|((bbU64)bbLD32LE((adr)+4)<<32))
/** Store bbU16 (16 bit) to unaligned address, stores in little endian order */
#define bbST16LE(adr,w) *(bbU8*)(adr)=(bbU8)(w),*((bbU8*)(adr)+1)=(bbU8)((w)>>8)
/** Store 24 bit word (low 24 bit of a bbU32) to unaligned address, stores in little endian order */
#define bbST24LE(adr,w) *(bbU8*)(adr)=(bbU8)(w),*((bbU8*)(adr)+1)=(bbU8)((w)>>8),*((bbU8*)(adr)+2)=(bbU8)((w)>>16)
/** Store bbU32 (32 bit) to unaligned address, stores in little endian order */
#define bbST32LE(adr,w) *(bbU8*)(adr)=(bbU8)(w),*((bbU8*)(adr)+1)=(bbU8)((w)>>8),*((bbU8*)(adr)+2)=(bbU8)((w)>>16),*((bbU8*)(adr)+3)=(bbU8)((w)>>24)
/** Store bbU64 (64 bit) to unaligned address, stores in little endian order */
#define bbST64LE(adr,w) bbST32LE(adr,(bbU32)w)),bbST32LE(adr,(bbU32)(w>>32)))
/** Load little endian bbS16 (16 bit) from unaligned address and sign-extend */
#define bbLDS16LE(adr) (bbS32)((bbU32)*((bbU8*)(adr))|((bbS32)*(((bbS8*)(adr))+1)<<8))
/** Load little endian 24 bit word (low 24 bit of a bbU32) from unaligned address and sign-extend */
#define bbLDS24LE(adr) (bbS32)((bbU32)*((bbU8*)(adr))|((bbU32)*(((bbU8*)(adr))+1)<<8)|((bbS32)*(((bbS8*)(adr))+2)<<16))
#else // x86 support non-aligned memory access
#define bbLD16LE(adr) ((bbU32)*(const bbU16*)(adr))
#define bbLD24LE(adr) ((bbU32)*((bbU16*)(adr))|((bbU32)*(((bbU8*)(adr))+2)<<16))
#define bbLD32LE(adr) (*(const bbU32*)(adr))
#define bbLD64LE(adr) (*(const bbU64*)(adr))
#define bbST16LE(adr,w) *(bbU16*)(adr)=(bbU16)(w)
#define bbST24LE(adr,w) *(bbU16*)(adr)=(bbU16)(w),*((bbU8*)(adr)+2)=(bbU8)((w)>>16)
#define bbST32LE(adr,w) *(bbU32*)(adr)=(bbU32)(w)
#define bbST64LE(adr,w) *(bbU64*)(adr)=(bbU64)(w)
#define bbLDS16LE(adr) ((bbS32)*((bbS16*)(adr)))
#define bbLDS24LE(adr) (bbS32)((bbU32)*(const bbU16*)(adr)|((bbS32)*(((bbS8*)(adr))+2)<<16))
#endif


/** Load big endian bbU32 (32 bit) from unaligned address */
#define bbLD32BE(adr) ((((bbU32)*(bbU8*)(adr))<<24)|(((bbU32)*((bbU8*)(adr)+1))<<16)|(((bbU32)*((bbU8*)(adr)+2))<<8)|((bbU32)*((bbU8*)(adr)+3)))
/** Load big endian bbU32 (64 bit) from unaligned address */
#define bbLD64BE(adr) (((bbU64)bbLD32BE(adr)<<32)|(bbU64)bbLD32BE((adr)+4))
/** Load big endian 24 bit word (low 24 bit of a bbU32) from unaligned address */
#define bbLD24BE(adr) ((((bbU32)*(bbU8*)(adr))<<16)|(((bbU32)*((bbU8*)(adr)+1))<<8)|((bbU32)*((bbU8*)(adr)+2)))
/** Load big endian bbU16 (16 bit) from unaligned address */
#define bbLD16BE(adr) ((((bbU32)*(bbU8*)(adr))<<8)|(bbU32)*((bbU8*)(adr)+1))
/** Store bbU32 (32 bit) to unaligned address, stores in big endian order */
#define bbST32BE(adr,w) *(bbU8*)(adr)=(bbU8)((w)>>24),*((bbU8*)(adr)+1)=(bbU8)((w)>>16),*((bbU8*)(adr)+2)=(bbU8)((w)>>8), *((bbU8*)(adr)+3)=(bbU8)(w)
/** Store 24 bit word (low 24 bit of a bbU32) to unaligned address, stores in big endian order */
#define bbST24BE(adr,w) *(bbU8*)(adr)=(bbU8)((w)>>16),*((bbU8*)(adr)+1)=(bbU8)((w)>>8),*((bbU8*)(adr)+2)=(bbU8)(w)
/** Store bbU16 (16 bit) to unaligned address, stores in big endian order */
#define bbST16BE(adr,w) *(bbU8*)(adr)=(bbU8)((w)>>8),*((bbU8*)(adr)+1)=(bbU8)(w)
/** Load big endian 24 bit word (low 24 bit of a bbU32) from unaligned address and sign-extend */
#define bbLDS24BE(adr) (bbS32)((((bbS32)*(bbS8*)(adr))<<16)|(((bbU32)*((bbU8*)(adr)+1))<<8)|((bbU32)*((bbU8*)(adr)+2)))
/** Load big endian bbS16 (16 bit) from unaligned address and sign-extend */
#define bbLDS16BE(adr) (bbS32)((((bbS32)*(bbS8*)(adr))<<8)|(bbU32)*((bbU8*)(adr)+1))

/** Load native endian bbU32 (32 bit) from unaligned address */
#if bbCPUE==bbCPUE_LE
#define bbLD32(adr) bbLD32LE(adr)
#else
#define bbLD32(adr) bbLD32BE(adr)
#endif

/** Load native endian bbU16 (16 bit) from unaligned address */
#if bbCPUE==bbCPUE_LE
#define bbLD16(adr) bbLD16LE(adr)
#else
#define bbLD16(adr) bbLD16BE(adr)
#endif

/** Store native endian bbU32 (32 bit) to unaligned address */
#if bbCPUE==bbCPUE_LE
#define bbST32(adr,w) bbST32LE(adr,w)
#else
#define bbST32(adr,w) bbST32BE(adr,w)
#endif

/** Store native endian bbU16 (16 bit) to unaligned address */
#if bbCPUE==bbCPUE_LE
#define bbST16(adr,w) bbST16LE(adr,w)
#else
#define bbST16(adr,w) bbST16BE(adr,w)
#endif


/** Load little endian bbU32 (32 bit) from aligned address */
#if bbCPUE==bbCPUE_LE
#define bbLDA32LE(adr) (*(const bbU32*)(adr))
#else
#define bbLDA32LE(adr) bbLD32LE(adr)
#endif

/** Store bbU32 (32 bit) to aligned address, stores in little endian order */
#if bbCPUE==bbCPUE_LE
#define bbSTA32LE(adr,w) *(bbU32*)(adr)=(bbU32)(w)
#else
#define bbSTA32LE(adr,w) *(bbU8*)(adr)=(bbU8)(w),*((bbU8*)(adr)+1)=(bbU8)((w)>>8),*((bbU8*)(adr)+2)=(bbU8)((w)>>16),*((bbU8*)(adr)+3)=(bbU8)((w)>>24)
#endif

/** Load little endian bbU16 (16 bit) from aligned address */
#if bbCPUE==bbCPUE_LE
#define bbLDA16LE(adr) (*(const bbU16*)(adr))
#else
#define bbLDA16LE(adr) bbLD16LE(adr)
#endif

/** Returns the minimum of a set of two values. */
#define bbMIN(a,b) ((a)<=(b)?(a):(b))
/** Returns the maximum of a set of two values. */
#define bbMAX(a,b) ((a)>=(b)?(a):(b))

/** @addtogroup groupErr Error handling
@{*/

/** Global variable holding the last occured error code.
    Do not use directly, use #bbErrGet and #bbErrSet instead.
*/
extern bbERR bbgErr;

/** Set last error code.
    @param err (bbERR) Error code to set
    @return (bbERR) Value of \a err
*/
#define bbErrSet(err) bbgErr=err

/** Get last error code.
    @return (bbERR) Error which occured most recently
*/
#define bbErrGet() bbgErr

/** Get string for error code.
    @return Pointer to string. This pointer will point to an inernal buffer
            managed by babellib and is valid only as long as no other babellib
            functions are called.
*/
const bbCHAR* bbGetErrStr(const bbERR err);

/** Get string for last error code.
    @return Pointer to string. This pointer will point to an inernal buffer
            managed by babellib and is valid only as long as no other babellib
            functions are called.
*/
#define bbGetLastErrStr() bbGetErrStr(bbgErr)

#define bbERRSTRMAXLEN 256
extern bbCHAR bbgErrStr[bbERRSTRMAXLEN];

/*@}*/

#if ((defined(DEBUG) || defined(_DEBUG) || defined(bbDOXYGEN)) && !defined(bbDEBUG))
    #define bbDEBUG
#elif defined(__MWERKS__)
    #if ((__ide_target("Debug")==1) && !defined(bbDEBUG))
        #define bbDEBUG
    #endif
#endif

#ifdef bbDEBUG
    #ifndef ASSERT
        #if bbOS == bbOS_QT && defined(__cplusplus)
            #define bbASSERT(a) Q_ASSERT(a)
        #elif defined(WIN32)
            #define bbASSERT(a) if(!(a)) _CrtDbgBreak();
        #elif bbOS == bbOS_PALMOS
            #define bbASSERT(a) ErrFatalDisplayIf((!(a)), "bbASSERT");
        #else
            /** Traps debugger if expression \a evaluates to false.
                Will have no effect if #bbDEBUG is undefined.
            */
            #define bbASSERT(a) if(!(a)) exit(1);
        #endif
    #else
        #define bbASSERT ASSERT
    #endif
#else
    #define bbASSERT(a)
#endif

#ifdef bbDEBUG
/** Lock mutex with user defined ID, included in debug builds only.
    If the same mutex ID is locked twice, bbASSERT is called.
    Since the mutex ID is user defined, care must be taken not to
    use the same ID for different mutexes. Often it is convenient
    to use object pointers as mutex ID.
    @param id User-defined ID
*/
void bbDEBUGMUTEX_LOCK(bbU32 id);
/** Unlock mutex with user defined ID, included in debug builds only.
    @param id User-defined ID
    @see bbDEBUGMUTEX_LOCK()
*/
void bbDEBUGMUTEX_UNLOCK(bbU32 id);
#else
#define bbDEBUGMUTEX_LOCK(id)
#define bbDEBUGMUTEX_UNLOCK(id)
#endif

#ifdef bbDEBUG
#define bbTOUCH(n) n=0
#else
#define bbTOUCH(n)
#endif

#if defined(_MSC_VER)
#define bbDLL __declspec(dllexport)
#else
#define bbDLL
#endif

#if defined(_MSC_VER)
#define bbINLINE __forceinline
#elif defined(__cplusplus)
#define bbINLINE inline
#else
#define bbINLINE
#endif

#if (bbOS == bbOS_WINCE) || (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64)
#define bbDIRDELIMSTR bbT("\\")
#define bbDIRDELIM '\\'
#define bbFILEEXTDELIMSTR bbT(".")
#define bbFILEEXTDELIM '.'
#define bbCSD bbT(".")
#elif (bbOS == bbOS_RISCOS)
#define bbDIRDELIMSTR bbT(".")
#define bbDIRDELIM '.'
#define bbFILEEXTDELIMSTR "/"
#define bbFILEEXTDELIM '/'
#define bbCSD bbT("@")
#else
#define bbDIRDELIMSTR bbT("/")
#define bbDIRDELIM '/'
#define bbFILEEXTDELIMSTR bbT(".")
#define bbFILEEXTDELIM '.'
#define bbCSD bbT(".")
#endif

/** @name Error codes and return codes
@{*/

enum
{
    bbEOK = 0,      /**< Error code, indicates success of operation. */
    bbESYS = 1,     /**< Error code, indicates system error, for Windows call GetLastError(). */
    bbEEND,         /**< Error code, indicates end of operation. */
    bbEBUSY,        /**< Error code, indicates operation is busy. */
    bbEUK,          /**< Error code, indicates unknown error. */
    bbENOMEM,       /**< Error code, indicates out of memory. */
    bbEFULL,        /**< Error code, indicates buffer full. */
    bbENOTFOUND,    /**< Error code, indicates object not found. */
    bbENOTSUP,      /**< Error code, indicates not supported feature or function. */
    bbEBADPARAM,    /**< Error code, indicates bad parameters passed to function. */
    bbEBADSTATE,    /**< Error code, indicates bad program state. */
    bbESYNTAX,      /**< Error code, indicates syntax error. */
    bbEOVERFLOW,    /**< Error code, indicates overflow. */
    bbENOHEAP,      /**< Error code, indicates out of heap (@see bbHeapAlloc) */
    bbEEOF,         /**< Error code, indicates end of file */
    bbEFILEOPEN,    /**< Error code, indicates bbFileOpen failed. */
    bbEFILEACCESS,  /**< Error code, indicates a file operation that was forbidden due to access restrictions. */
    bbEFILENOTFOUND,/**< Error code, indicates referenced file or directory not found. */
    bbEFILEEXISTS,  /**< Error code, indicates referenced file alread exists. */
    bbEFILESEEK,    /**< Error code, indicates file seek error. */
    bbEFILEREAD,    /**< Error code, indicates file read error. */
    bbEFILEWRITE,   /**< Error code, indicates file write error. */
    bbEFILEFORMAT,  /**< Error code, indicates file format error. */

    bbELAST = ~0    /**< Error code, indicates error, error number can be found in #bbgErr. */
};

/** Error code base, gr library. @internal */
#define bbEBASE_GR 0x100
/** Error code base, picotype library. @internal */
#define bbEBASE_PT 0x200
/** Error code base, 7e library. @internal */
#define bbEBASE_E7 0x300
/** Error code base, ui library. @internal */
#define bbEBASE_UI 0x400
/** Error code base, dt library. @internal */
#define bbEBASE_DT 0x500
/** Error code base, pp library. @internal */
#define bbEBASE_PP 0x600
/** Error code base, mh library. @internal */
#define bbEBASE_MH 0x700

#ifdef  __cplusplus
}
#endif

/*@}*/

#endif /* bbBABEL_DEFS_H_ */

