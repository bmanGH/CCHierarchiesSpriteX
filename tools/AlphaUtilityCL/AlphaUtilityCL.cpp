/*
 * alpha dilate command line tool
 * author: xu xiao cheng <xiaocheng.xu@snsplus.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "FreeImage.h"

struct pixel {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
};

// http://publications.gbdirect.co.uk/c_book/chapter10/interpreting_program_arguments.html
static const char SwitchChar = '-';
static const char Unknown = '?';
int OptIndex = 1;       /* first option should be argv[1] */
char *OptArg = NULL;    /* global option argument pointer */
int options(int argc, char *argv[], const char *legal)
{
        static char *posn = "";  /* position in argv[OptIndex] */
        char *legal_index = NULL;
        int letter = 0;

        if(!*posn){
                /* no more args, no SwitchChar or no option letter ? */
                if((OptIndex >= argc) ||
                        (*(posn = argv[OptIndex]) != SwitchChar) ||
                        !*++posn)
                                return -1;
                /* find double SwitchChar ? */
                if(*posn == SwitchChar){
                        OptIndex++;
                        return -1;
                }
        }
        letter = *posn++;
        if(!(legal_index = strchr(legal, letter))){
                if(!*posn)
                        OptIndex++;
                return Unknown;
        }
        if(*++legal_index != ':'){
                /* no option argument */
                OptArg = NULL;
                if(!*posn)
                        OptIndex++;
        } else {
                if(*posn)
                        /* no space between opt and opt arg */
                        OptArg = posn;
                else
                        if(argc <= ++OptIndex){
                                posn = "";
                                return Unknown;
                        } else
                                OptArg = argv[OptIndex];
                posn = "";
                OptIndex++;
        }
        return letter;
}

void alphaDilate (pixel** inbuf,  pixel** outbuf, int w, int h)
{
    for(int row = 0; row < h; row++)
    {
        for(int col = 0; col < w; col++)
        {   
            int r = 0, g = 0, b = 0, a = 0;
            int x = 0, y = 0;
            int n = 0;

            if (inbuf[row][col].r == 0 &&
                inbuf[row][col].g == 0 &&
                inbuf[row][col].b == 0 &&
                inbuf[row][col].a == 0) {
                // left
                x = col - 1;
                y = row;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }
                
                // up
                x = col;
                y = row - 1;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }

                // right
                x = col + 1;
                y = row;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }

                // down
                x = col;
                y = row + 1;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }

                // left-top
                x = col - 1;
                y = row - 1;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }
                
                // right-top
                x = col + 1;
                y = row - 1;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }

                // left-down
                x = col - 1;
                y = row + 1;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }

                // right-down
                x = col + 1;
                y = row + 1;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    if (inbuf[y][x].r != 0 ||
                        inbuf[y][x].g != 0 ||
                        inbuf[y][x].b != 0 ||
                        inbuf[y][x].a != 0) {
                        n++;
                        r += inbuf[y][x].r;
                        g += inbuf[y][x].g;
                        b += inbuf[y][x].b;
                        a += inbuf[y][x].a;
                    }
                }

                if (n > 0) {
                    r /= n;
                    g /= n;
                    b /= n;
                    a = 0;
                }
            }
            else {
                r = inbuf[row][col].r;
                g = inbuf[row][col].g;
                b = inbuf[row][col].b;
                a = inbuf[row][col].a;
            }

            outbuf[row][col].r = r;
            outbuf[row][col].g = g;
            outbuf[row][col].b = b;
            outbuf[row][col].a = a;
        }
    }
}

int main(int argc, char** argv)
{
    FIBITMAP* bitmap = NULL;
    FIBITMAP* result = NULL;
    int width = 0;
    int height = 0;
    int bytespp = 0;
    pixel** inbuf = NULL;
    pixel** outbuf = NULL;
    int start;
    pixel** buf1 = NULL;
    pixel** buf2 = NULL;
    int dilateAmount = 5;
    char imageFile[255];
    memset(imageFile, 0, sizeof(imageFile));
    char resultFile[255];
    memset(resultFile, 0, sizeof(resultFile));

    // get image file name from arg
    bool isInit = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            strcpy(imageFile, argv[i + 1]);
            isInit = true;
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strcpy(resultFile, argv[i + 1]);
        }
        else if (strcmp(argv[i], "-dilate") == 0 && i + 1 < argc) {
            dilateAmount = atoi(argv[i + 1]);
        }
    }

    if (!isInit) {
        printf("example: AlphaUtilityCL -dilate 5 -i input.png -o output.png\n");
        return 1; // error args
    }
    
    // init freeimage
    FreeImage_Initialise(TRUE);

    // read image to bitmap
    bitmap = FreeImage_Load(FIF_PNG, imageFile, PNG_DEFAULT);
    if(!bitmap) {
        printf("image load failed\n");
        return 2; // image load failed
    }

    // get image width and height
    width  = FreeImage_GetWidth(bitmap);
    height = FreeImage_GetHeight(bitmap);
    // get byte count per pixel
    bytespp = FreeImage_GetLine(bitmap)/ width;
    printf("load image %s with width:%i height:%i bytes per pixel:%i\n", 
        imageFile, width, height, bytespp);

    // input buffer
    inbuf = (pixel**)calloc(height, sizeof(pixel*));
    for(int i = 0; i < height; i++)
    {
        inbuf[i] = (pixel*)calloc(width, sizeof(pixel));
    }
    // output buffer
    outbuf = (pixel**)calloc(height, sizeof(pixel*));
    for(int i = 0; i < height; i++)
    {
        outbuf[i] = (pixel*)calloc(width, sizeof(pixel));
    }
    if(!inbuf || !outbuf) {
        printf("memory alloc error\n");
        goto quit;
    }

    // read bitmap bytes to input buffer
    for(int y = 0; y < height; y++)
    {
        BYTE* bitsLine = FreeImage_GetScanLine(bitmap, y);
        for(int x = 0; x < width; x++)
        {
            outbuf[y][x].r = inbuf[y][x].r = bitsLine[FI_RGBA_RED];
            outbuf[y][x].g = inbuf[y][x].g = bitsLine[FI_RGBA_GREEN];
            outbuf[y][x].b = inbuf[y][x].b = bitsLine[FI_RGBA_BLUE];
            outbuf[y][x].a = inbuf[y][x].a = bitsLine[FI_RGBA_ALPHA];
            bitsLine += bytespp;
        }
    }

    // do alpha dilate
    // start = clock();
    for (int i = 0; i < dilateAmount; i++) {
        pixel** tmpbuf = inbuf;
        inbuf = outbuf;
        outbuf = tmpbuf;

        for(int j = 0; j < height; j++)
        {
            memset(outbuf[j], 0, width * sizeof(pixel));
        }

        alphaDilate(inbuf, outbuf, width, height);
    }
    // printf("time elapsed: %i ms\n", (clock() - start));

    // write output buffer to bitmap
    result =  bitmap;
    for(int y = 0; y < height; y++)
    {
        BYTE* bitsLine = FreeImage_GetScanLine(result, y);
        for(int x = 0; x < width; x++)
        {
            bitsLine[FI_RGBA_RED]   = outbuf[y][x].r ;
            bitsLine[FI_RGBA_GREEN] = outbuf[y][x].g ;
            bitsLine[FI_RGBA_BLUE]  = outbuf[y][x].b ;
            bitsLine[FI_RGBA_ALPHA] = outbuf[y][x].a ;
            bitsLine += bytespp;
        }
    }

    // save result bitmap to image
    FreeImage_Save(FIF_PNG, result, resultFile, PNG_DEFAULT);
    printf("save result to file %s\n", resultFile);

quit:
    // release resource
    for(int i = 0; i < height; i++)
    {
        free(inbuf[i]);
    }
    free(inbuf);
    for(int i = 0; i < height; i++)
    {
        free(outbuf[i]);
    }
    free(outbuf);
    FreeImage_Unload(bitmap);
    FreeImage_DeInitialise();
    return 0;
}