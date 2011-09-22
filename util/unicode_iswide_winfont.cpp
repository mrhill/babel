#define _unicode
#define WINVER        0x0400
#define _WIN32_WINNT  0x0500
#define _WIN32_IE     0x0400
#define _RICHEDIT_VER 0x0100

#include <windows.h>
#include <CRTDBG.H>
#include <stdlib.h> /* for malloc */
#include <stdio.h>  /* for printf, etc. */
#include <wchar.h>  /* for printf, etc. */

static LOGFONT gLF[] = {
    0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("‚l‚r –¾’©"),
    0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("GulimChe"),
    0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("NSimSun")
};

static char* gFName[] = {
    "unicode_iswide_msmincho.dat",
    "unicode_iswide_gulimche.dat",
    "unicode_iswide_nsimsun.dat"
};

static TCHAR gFace[256];

static DWORD gBits[65536/32];

int main(int argc, char** argv)
{
    int f, i;
    TEXTMETRIC tm;
    ABC   realwidth;

    HDC hDC = CreateCompatibleDC(NULL);

    for (f=0; f<sizeof(gLF)/sizeof(LOGFONT); f++)
    {
        HFONT hFont = CreateFontIndirect(&gLF[f]);
        if (hFont)
        {
            memset(gBits, 0, sizeof(gBits));

            SelectObject(hDC, hFont);
            GetTextMetrics(hDC, &tm);
            GetTextFace(hDC, 256, gFace);
            printf("%s %dx%d (%S)\n", gFName[f], tm.tmAveCharWidth, tm.tmHeight, gFace);

            DWORD tmp = GetFontUnicodeRanges(hDC, NULL);
            GLYPHSET* pGS = (GLYPHSET*)malloc(tmp);
            GetFontUnicodeRanges(hDC, pGS);

            for (i=0; i<(int)pGS->cRanges; i++)
            {
                DWORD cp     = (DWORD) pGS->ranges[i].wcLow;
                DWORD cp_end = cp + (DWORD) pGS->ranges[i].cGlyphs;

                if (cp_end > 0x10000UL)
                {
                    _CrtDbgBreak();
                }

                while (cp < cp_end)
                {
                    GetCharABCWidths(hDC, cp, cp, &realwidth);
                    realwidth.abcB += realwidth.abcA + realwidth.abcC;

                    if (realwidth.abcB == (UINT)tm.tmAveCharWidth)
                    {
                    }
                    else if (realwidth.abcB == (UINT)tm.tmAveCharWidth*2)
                    {
                        gBits[cp>>5] |= (1UL<<(cp&31));
                    }
                    else
                    {
                        _CrtDbgBreak();
                        printf("X");
                    }

                    cp++;
                }
            }            
            
            free(pGS);
            DeleteObject(hFont);

            FILE* fh = fopen(gFName[f], "wb");
            fwrite(gBits, 1, sizeof(gBits), fh);
            fclose(fh);
        }
    }

    DeleteObject(hDC);
    return 0;
}
