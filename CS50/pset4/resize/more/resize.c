#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: resize resize_value infile outfile\n");
        return 1;
    }

    float size = atof(argv[1]);
    if (size < 0.0f || size > 100.0f)
    {
        fprintf(stderr, "Resize value must be between 0 and 100\n");
        return 1;
    }
    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    if (size == 1)
    {
        //If copying image

        // write outfile's BITMAPFILEHEADER
        fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

        // write outfile's BITMAPINFOHEADER
        fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

        // determine padding for scanlines
        int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

        // iterate over infile's scanlines
        for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }

            // skip over padding, if any
            fseek(inptr, padding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int k = 0; k < padding; k++)
            {
                fputc(0x00, outptr);
            }
        }

        // close infile
        fclose(inptr);

        // close outfile
        fclose(outptr);

        // success
        return 0;
    }
    else if (size > 1)
    {
        //If enlarging image

        BITMAPFILEHEADER bf2 = bf;
        BITMAPINFOHEADER bi2 = bi;

        //Get new dimensions
        bi2.biWidth *= size;
        bi2.biHeight *= size;

        //Store new and old padding values
        int paddingOld = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
        int paddingNew = (4 - (bi2.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

        //Set outfile headers
        bi2.biSizeImage = ((sizeof(RGBTRIPLE) * bi2.biWidth) + paddingNew) * abs(bi2.biHeight);
        bf2.bfSize = bi2.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


        // write outfile's BITMAPFILEHEADER
        fwrite(&bf2, sizeof(BITMAPFILEHEADER), 1, outptr);

        // write outfile's BITMAPINFOHEADER
        fwrite(&bi2, sizeof(BITMAPINFOHEADER), 1, outptr);

        // iterate over infile's scanlines
        for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
        {
            //Create scanlines size times
            for (int l = 0; l < size; l++)
            {
                // iterate over infile pixels in scanline
                for (int j = 0; j < bi.biWidth; j++)
                {
                    // temporary storage
                    RGBTRIPLE triple;

                    // read RGB triple from infile
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    for (int k = 0; k < size; k++)
                    {
                        // write RGB triple to outfile
                        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    }
                }

                // then add it back
                for (int k = 0; k < paddingNew; k++)
                {
                    fputc(0x00, outptr);
                }
                //Each time we need to copy a scanline we need to set inptr back to beginning of line.
                if (l < size - 1)
                {
                    fseek(inptr, -bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
                }
            }
            // skip over padding, if any
            fseek(inptr, paddingOld, SEEK_CUR);
        }

        // close infile
        fclose(inptr);

        // close outfile
        fclose(outptr);

        // success
        return 0;
    }
    else
    {
        //If shrinking image
        BITMAPFILEHEADER bf2 = bf;
        BITMAPINFOHEADER bi2 = bi;

        //Set new dimensions
        bi2.biWidth *= size;
        bi2.biHeight *= size;

        //Store in and out padding
        int paddingOld = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
        int paddingNew = (4 - (bi2.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

        //New headers
        bi2.biSizeImage = ((sizeof(RGBTRIPLE) * bi2.biWidth) + paddingNew) * abs(bi2.biHeight);
        bf2.bfSize = bi2.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        // write outfile's BITMAPFILEHEADER
        fwrite(&bf2, sizeof(BITMAPFILEHEADER), 1, outptr);

        // write outfile's BITMAPINFOHEADER
        fwrite(&bi2, sizeof(BITMAPINFOHEADER), 1, outptr);

        // iterate over infile's scanlines
        for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i ++)
        {
            //With a scale factor smaller than 1 we can use the following formula to check if any given scanline should
            //be included in the outfile
            if (ceil(i * size) != ceil((i + 1) * size))
            {
                printf("i: %i\n", i);
                // iterate over infile pixels in scanline
                for (int j = 0; j < bi.biWidth; j ++)
                {
                    //Check if pixel should be in outfile
                    if (ceil(j * size) != ceil((j + 1) * size))
                    {
                        printf("j: %i\n", j);
                        // temporary storage
                        RGBTRIPLE triple;
                        // read RGB triple from infile
                        fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                        // write RGB triple to outfile
                        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    }
                    //Otherwise, seek forward 1 pixel
                    else
                    {
                        fseek(inptr, sizeof(RGBTRIPLE), SEEK_CUR);
                    }
                }

                // then add it back
                for (int k = 0; k < paddingNew; k++)
                {
                    fputc(0x00, outptr);
                }
                // skip over padding, if any
                fseek(inptr, paddingOld, SEEK_CUR);
            }
            //Otherwise seek forward 1 line
            else
            {
                fseek(inptr, ((sizeof(RGBTRIPLE) * abs(bi.biWidth)) + paddingOld), SEEK_CUR);
            }
        }

        // close infile
        fclose(inptr);

        // close outfile
        fclose(outptr);

        // success
        return 0;

    }
}