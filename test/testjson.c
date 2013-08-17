#include "babel/json.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    bbERR err;
    bbStrBuf str;
    bbJsonVal val;
    bbJsonVal val2;
    bbJsonVal val3;
    bbJsonVal* pVal;

    bbStrBufInit(&str);

    bbJsonValInit(&val);
    bbJsonValInitType(&val2, bbJSONTYPE_INTEGER);
    bbJsonValInit(&val3);

    bbJsonObjAddInt(&val, "fourtytwo", 42);
    bbJsonObjAddStr(&val, "key", "value");

    val2.u.integer = 23;
    bbJsonObjAddObj(&val, bbT("Hallo"), &val2);

    bbJsonArrInsObj(&val3, -1, &val2);
    bbJsonArrInsObj(&val3, -1, &val2);
    bbJsonArrInsObj(&val3, -1, &val2);
    bbJsonArrInsDbl(&val3, 1, 23.42);
    bbJsonArrInsStr(&val3, 0, bbT("My string"));
    bbJsonArrInsBool(&val3, 123, 1);
    bbJsonArrInsObj(&val3, -1, &val2);
    bbJsonObjAddObj(&val, bbT("arr"), &val3);

    /*
    bbJsonValInitType(&val2, bbJSONTYPE_OBJECT);
    bbJsonObjAddInt(&val2, "fourtythree", 43);
    bbJsonObjAddStr(&val2, "key2", "value2");
    bbJsonObjAddObj(&val, bbT("xyz"), &val2);
    bbJsonObjAddObj(&val, bbT("qwe"), &val2);
    */

    err = bbJsonValDump(&val, &str, 0);
    bbPrintf(bbT("%s\n"), bbStrBufGetPtr(&str));
    bbStrBufClear(&str);
/*
    bbJsonObjDel(&val, "key");
    bbJsonObjDel(&val, "Hallo");

    pVal = bbJsonObjGet(&val, "arr");
    bbJsonArrDel(pVal, bbJsonArrGetSize(pVal)-1);
    bbJsonArrDel(pVal, 0);
    err = bbJsonValDump(&val, &str, 1);
    bbPrintf(bbT("%s\n"), bbStrBufGetPtr(&str));
*/
    bbJsonValDestroy(&val3);
    bbJsonValDestroy(&val2);
    bbJsonValDestroy(&val);
    bbStrBufDestroy(&str);
    return 0;
}

