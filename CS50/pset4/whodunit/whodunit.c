#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    //Check for correct number of command line arg
    if (argc != 3)
    {
        fprintf(stderr, "Usage: whodunit infile outfile\n");
        return 1;
    }

    //Get name for infile and outfile, save
    char *infile = argv[1];
    char *outfile = argv[2];

    //Open infile, save ref
    FILE *inptr = fopen(infile, "r");
    if (!inptr)
    {
        fprintf(stderr, "Could not open %s\n", infile);
        return 2;
    }

    //Open outfile, save ref
    FILE *outptr = fopen(outfile, "w");
    if (!outptr)
    {
        fprintf(stderr, "Could not create %s\n", outfile);
        return 3;
    }

    //Read infile header info
    BITMAPFILEHEADER fhead;
    fread(&fhead, sizeof(BITMAPFILEHEADER), 1, inptr);
    BITMAPINFOHEADER ihead;
    fread(&ihead, sizeof(BITMAPINFOHEADER), 1, inptr);

    //Check that infile is of type Bitmap
    if (fhead.bfType != 0x4d42 || fhead.bfOffBits != 54 || ihead.biSize != 40 ||
        ihead.biBitCount != 24 || ihead.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //Write header info to outfile
    fwrite(&fhead, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&ihead, sizeof(BITMAPINFOHEADER), 1, outptr);

    //Get image padding
    int pad = (4 - (ihead.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //Iterate through every scanline
    for (int i = 0, biHeight = abs(ihead.biHeight); i < biHeight; i++)
    {
        // Iterate through every pixel of current scanline
        for (int j = 0; j < ihead.biWidth; j++)
        {
            // Storage for current triple
            RGBTRIPLE tri;

            // read RGB triple from infile
            fread(&tri, sizeof(RGBTRIPLE), 1, inptr);

            //I chose to set any fully red pixel to white and every other pixel to black thus uncovering the message
            if (tri.rgbtRed == 255)
            {
                tri.rgbtBlue = 255;
                tri.rgbtGreen = 255;
                tri.rgbtRed = 255;
            }
            else
            {
                tri.rgbtBlue = 0;
                tri.rgbtGreen = 0;
                tri.rgbtRed = 0;
            }

            // write triple to outfile
            fwrite(&tri, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding
        fseek(inptr, pad, SEEK_CUR);

        // then add it back
        for (int k = 0; k < pad; k++)
        {
            fputc(0x00, outptr);
        }
    }

    fclose(inptr);
    fclose(outptr);
    return 0;
}