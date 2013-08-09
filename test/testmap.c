#include "babel/map.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    int i;
    char str[64];

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
        bbMapDump(&map);
    }
    bbMapDump(&map);

    i = bbMapGet(&map, "peach"); printf("peach -> %p\n", i);
    i = bbMapGet(&map, "birne"); printf("birne -> %p\n", i);
    i = bbMapGet(&map, 0); printf("(nil) -> %p\n", i);

    bbMapDestroy(&map);
    return 0;
}

