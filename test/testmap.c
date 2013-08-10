#include "babel/map.h"
#include <stdio.h>

int cb(const bbCHAR* key, bbU64PTR val)
{
    printf("%s -> 0x%"bbI64"X\n", key, val);
    return 0;
}

int main(int argc, char** argv)
{
    int i;
    bbCHAR str[64];
    bbU64PTR val;

    bbMap map;
    bbMapInit(&map);
    bbMapDump(&map);

    bbMapAdd(&map, bbT("apple"), 1);
    bbMapAddC(&map, bbT("peach"), 23);
    bbMapDump(&map);

    bbMapAdd(&map, bbT("birne"), 234);
    bbMapDump(&map);

    for(i=0; i<111; i+=1)
    {
        sprintf(str, "%d", i, i*i & 255);
        bbMapAdd(&map, str, i);
    }
    bbMapDump(&map);

    i = bbMapGet(&map, "peach"); printf("peach -> %p\n", i);
    i = bbMapGet(&map, "birne"); printf("birne -> %p\n", i);
    i = bbMapGet(&map, 0); printf("(nil) -> %p\n", i);

    bbMapEnumerate(&map, cb);

    printf("map size: %u\n", bbMapGetSize(&map));
    val = bbMapDel(&map, "noexist"); printf("del noexist: 0x%"bbI64"X (%d)\n", val, bbErrGet());
    val = bbMapDel(&map, "peach"); printf("del peach: 0x%"bbI64"X (%d)\n", val, bbErrGet());
    printf("map size: %u\n", bbMapGetSize(&map));

    printf("testmap: add/del stress ---\n");

    for(i=0; i<111111; i+=1)
    {
        sprintf(str, "%d", i*i & 511);

        if (i & 5)
            bbMapAdd(&map, str, str);
        else
            bbMapDel(&map, str);
    }
    bbMapDump(&map);

    bbMapDestroy(&map);
    return 0;
}

