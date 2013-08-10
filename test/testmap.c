#include "babel/map.h"
#include <stdio.h>

int cb(const bbCHAR* key, bbU64PTR val)
{
    printf("enum \"%s\" -> 0x%"bbI64"X\n", key, val);
    return 0;
}

int main(int argc, char** argv)
{
    int i, j;
    bbCHAR str[1024];
    bbU64PTR val;

    bbMap map;

    printf("testmap: init ----------------\n");

    bbMapInit(&map);
    bbMapDump(&map);

    printf("testmap: Add/AddC ----------------\n");

    bbMapAdd(&map, bbT("apple"), 1);
    bbMapAddC(&map, bbT("peach"), 23);
    bbMapAdd(&map, bbT("birne"), 234);
    bbMapDump(&map);

    for(i=0; i<111; i+=1)
    {
        sprintf(str, "%d", i, i*i & 255);
        bbMapAdd(&map, str, i);
    }
    bbMapDump(&map);

    printf("testmap: Get/Enumerate ----------------\n");

    i = bbMapGet(&map, "peach"); printf("peach -> %p\n", i);
    i = bbMapGet(&map, "birne"); printf("birne -> %p\n", i);
    i = bbMapGet(&map, 0); printf("(nil) -> %p\n", i);

    bbMapEnumerate(&map, cb);

    printf("testmap: Del ----------------\n");

    printf("map size: %u\n", bbMapGetSize(&map));
    val = bbMapDel(&map, "noexist"); printf("del noexist: 0x%"bbI64"X (%d)\n", val, bbErrGet());
    val = bbMapDel(&map, "peach"); printf("del peach: 0x%"bbI64"X (%d)\n", val, bbErrGet());
    printf("map size: %u\n", bbMapGetSize(&map));

    printf("testmap: Add/Del stress ----------------\n");

    for(i=0; i<111111; i+=1)
    {
        sprintf(str, "%d", i*i & 511);

        if (i & 5)
            bbMapAdd(&map, str, str);
        else
            bbMapDel(&map, str);
    }
    bbMapDump(&map);

    printf("testmap: long key Add/Del stress ----------------\n");

    for(i=0; i<111111; i+=1)
    {
        int l = 511 + i*i & 511;
        for(j=0; j<l; j++)
            str[j] = 65 + ((i*i+j*j)&15);
        str[j]=0;

        if (i & 10)
            bbMapAdd(&map, str, str);
        else
            bbMapDel(&map, str);
    }
    bbMapDump(&map);


    bbMapDestroy(&map);
    return 0;
}

