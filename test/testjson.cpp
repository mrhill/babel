#include "babel/json.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    bbERR err;
    bbStrBuf str;

    bbJsonVal val(bbJSONTYPE_OBJECT);

    //val["key"] = "value";

    bbJsonVal val2(bbJSONTYPE_INTEGER);
    val2.u.integer = 23;
    bbMapAdd(&val.u.object, bbT("Hallo"), (bbUPTR)&val2);

    err = val.Dump(str);
    bbPrintf(bbT("%d, %s\n"), err, str.GetPtr());

    return 0;
}

