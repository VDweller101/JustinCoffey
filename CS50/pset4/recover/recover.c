#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: recover image\n");
        return 1;
    }

    char *infile = argv[1];

    //Create pointers to infile and outfile, initialize outfile to null
    FILE *inptr = fopen(infile, "r");
    FILE *outptr = NULL;

    if (!inptr)
    {
        fprintf(stderr, "Could not open file\n");
        return 2;
    }
    //Use uint8 for single bytes
    uint8_t currentBlock[512];
    //Starting at -1 so first image is labled 0 for file naming reasons
    int imageCount = -1;
    //For checking if currently writing a file when new jpg header is found
    bool inImage = false;

    //fread will return number of elements read unless there not enough to read. If fread does not return 512, end of file reached
    while (fread(&currentBlock, 1, 512, inptr) == 512)
    {
        //Put in its own bool to make code therafter simpler
        bool jpgHead = (currentBlock[0] == 0xff && currentBlock[1] == 0xd8 && currentBlock[2] == 0xff && (currentBlock[3] & 0xf0) == 0xe0);

        if (jpgHead)
        {
            if (inImage)
            {
                //Close out last image because the beginning of a new jpg has been found
                inImage = false;
                fclose(outptr);
                fseek(inptr, -512, SEEK_CUR);
            }
            else
            {
                inImage = true;

                imageCount++;

                //create outfile

                //Creare filename
                char fileName[7];
                //If image count < 10, need 00 leading file number, else need 0
                if (imageCount < 10)
                {
                    sprintf(fileName, "00%i.jpg", imageCount);
                }
                else
                {
                    sprintf(fileName, "0%i.jpg", imageCount);
                }
                //Create outfile pointer
                outptr = fopen(fileName, "w");
                //Write block
                for (int i = 0; i < 512; i++)
                {
                    fwrite(&currentBlock[i], 1, 1, outptr);
                }
            }
        }
        else
        {
             if (inImage)
             {
                //Still in file, keep writing
                for (int i = 0; i < 512; i++)
                {
                    fwrite(&currentBlock[i], 1, 1, outptr);
                }
             }
        }
    }
}