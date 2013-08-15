#include "babel/json.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    bbERR err;
    bbStrBuf str;
    bbJsonVal val;
    bbJsonVal val2;
    bbJsonVal val3;

    bbJsonValInit(&val);
    bbJsonObjAddInt(&val, "fourtytwo", 42);
    bbJsonObjAddStr(&val, "key", "value");

    bbJsonValInitType(&val2, bbJSONTYPE_INTEGER);
    val2.u.integer = 23;
    bbJsonObjAddObj(&val, bbT("Hallo"), &val2);

    bbJsonValInit(&val3);
    bbJsonArrInsObj(&val3, -1, &val2);
    bbJsonArrInsObj(&val3, -1, &val2);
    bbJsonArrInsObj(&val3, -1, &val2);
    bbJsonArrInsDbl(&val3, 1, 23.42);
    bbJsonArrInsStr(&val3, 0, bbT("My string"));
    bbJsonArrInsBool(&val3, 123, 1);
    bbJsonObjAddObj(&val, bbT("arr"), &val3);

    bbStrBufInit(&str);
    err = bbJsonValDump(&val, &str);
    bbPrintf(bbT("%d, %s\n"), err, bbStrBufGetPtr(&str));

    bbStrBufDestroy(&str);
    bbJsonValDestroy(&val3);
    bbJsonValDestroy(&val2);
    bbJsonValDestroy(&val);

    return 0;
}

