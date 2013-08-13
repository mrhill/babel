#include <babel/strbuf.h>

extern "C" int testStrBuf()
{
    bbStrBuf str;

    str.Printf(bbT("ABC%d"), 23);
    bbPrintf(bbT("%s\n"), str.GetPtr());

    str.Printf(bbT("%8X%8X%8X%8X%8X%8X%8X%8X"), 1,2,3,4,5,6,7,8);
    bbPrintf(bbT("%s\n"), str.GetPtr());

    str.Printf(bbT("%08X%08X%08X%08X%08X%08X%08X%08Xaaa"), 1,2,3,4,5,6,7,8);
    bbPrintf(bbT("%s\n"), str.GetPtr());

    return 0;
}

