#include "babel/json.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    bbERR err;
    bbStrBuf str;
    bbJsonVal val;
    bbJsonVal val2;
    bbJsonVal val3;
    bbJsonVal valp;
    bbJsonVal* pVal;

    bbStrBufInit(&str);

    bbJsonValInit(&val);
    bbJsonValInitType(&val2, bbJSONTYPE_INTEGER);
    bbJsonValInit(&val3);
    bbJsonValInit(&valp);

    bbJsonObjAddInt(&val, "fourtytwo", 42);
    bbJsonObjAddStr(&val, "key", "value");

    val2.u.integer = 23;
    bbJsonObjAdd(&val, bbT("Hallo"), &val2);

    bbJsonArrIns(&val3, -1, &val2, 1);
    bbJsonArrIns(&val3, -1, &val2, 1);
    bbJsonArrIns(&val3, -1, &val2, 1);
    bbJsonArrInsDbl(&val3, 1, 23.42);
    bbJsonArrInsStr(&val3, 0, bbT("My string"));
    bbJsonArrInsBool(&val3, 123, 1);
    bbJsonArrIns(&val3, -1, &val2, 1);
    bbJsonObjAdd(&val, bbT("arr"), &val3);

    bbJsonValInitType(&val2, bbJSONTYPE_OBJECT);
    bbJsonObjAddInt(&val2, "fourtythree", 43);
    bbJsonObjAddStr(&val2, "key2", "value2");
    bbJsonObjAdd(&val, bbT("xyz"), &val2);
    bbJsonObjAdd(&val, bbT("qwe"), &val2);

    err = bbJsonValDump(&val, &str, 1);
    bbPrintf(bbT("%s\n"), bbStrBufGetPtr(&str));
    bbStrBufClear(&str);

    bbPrintf(bbT("Deleting \"key\"\n")); bbJsonObjDel(&val, "key");
    bbPrintf(bbT("Deleting \"Hallo\"\n")); bbJsonObjDel(&val, "Hallo");

    bbPrintf(bbT("Deleting elements in \"arr\"\n"));
    pVal = bbJsonObjGet(&val, "arr");
    bbJsonArrDel(pVal, bbJsonArrGetSize(pVal)-2, 2);
    bbJsonArrDel(pVal, 0, 1);

    err = bbJsonValDump(&val, &str, 1);
    bbPrintf(bbT("%s\n"), bbStrBufGetPtr(&str));

    bbJsonValInitParse(&valp, bbStrBufGetPtr(&str), bbStrBufGetLen(&str));
    //bbJsonValInitParse(&valp, bbT("{{{{23\":\"3\"}"), 1000);
    bbStrBufClear(&str);
    err = bbJsonValDump(&valp, &str, 1);
    bbPrintf(bbT("%d: %s\n"), err, bbStrBufGetPtr(&str));

    bbJsonValDestroy(&valp);
    bbJsonValDestroy(&val3);
    bbJsonValDestroy(&val2);
    bbJsonValDestroy(&val);
    bbStrBufDestroy(&str);
    return 0;
}

