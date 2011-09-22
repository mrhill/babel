#ifndef bbBABEL_CONFIG_H_
#define bbBABEL_CONFIG_H_

/** @file
    This file configurates compilations options of babellib.
*/

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroup groupConfig Configuration options
@{*/

/** Compile debug code */
#if ((defined(DEBUG) || defined(_DEBUG) || defined(bbDOXYGEN)) && !defined(bbDEBUG))
    #define bbDEBUG
#elif defined(__MWERKS__)
    #if ((__ide_target("Debug")==1) && !defined(bbDEBUG))
        #define bbDEBUG
    #endif
#endif

#define bbCFGENC_SBC   0
#define bbCFGENC_DBC   1
#define bbCFGENC_UTF8  2
#define bbCFGENC_UTF16 3

/** Internal character encoding.
    Choose one from bbCFGENC_*
*/
#ifndef bbCFGENC
#define bbCFGENC 3
#endif

/* Put windows libs into UNICODE mode */
#ifndef UNICODE
#define UNICODE
#endif

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
#define bbCPG bbCPG_UNICODE

#define bbSTRCONV_INTERNAL 0
#define bbSTRCONV_ICONV    1
#define bbSTRCONV_WIN      2

/** Select implementation of bbCCONV functions.
    Undefine to for auto-detection.
*/
#define bbSTRCONV bbSTRCONV_INTERNAL

/** Use of native C++ 64 bit integer calculations. */
#define bbUSE_LONGLONG 1

#define bbOS_NONE    0
#define bbOS_POSIX   1
#define bbOS_WIN32   2
#define bbOS_WINCE   3
#define bbOS_SYMBIAN 4
#define bbOS_PALMOS  5
#define bbOS_ITRON   6
#define bbOS_RISCOS  7
#define bbOS_ANDROID 8

/** Select target operating system.
    Undefine to for auto-detection.
*/
/*#define bbOS bbOS_NONE*/

#define bbCPU_UNDEFINED 0
#define bbCPU_X86  1
#define bbCPU_ARM  2
#define bbCPU_MIPS 3
#define bbCPU_SHX  4
#define bbCPU_68K  5

/** Target CPU.
    Undefine for auto-detection.
*/
/*#define bbCPU bbCPU_UNDEFINED*/

#define bbCPUE_LE 0
#define bbCPUE_BE 1

/** CPU endianess.
    Undefine for auto-detection based on #bbCPU.
*/
/*#define bbCPUE bbCPUE_LE*/

/** Size of type int in bytes. Undefine to try auto-detection. */
/*#define bbSIZEOF_INT 4*/
/** Size of pointer types in bytes. Undefine to try auto-detection. */
/*#define bbSIZEOF_UPTR 4*/

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_CONFIG_H_ */
