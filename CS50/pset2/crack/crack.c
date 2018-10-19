#define _XOPEN_SOURCE

#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <unistd.h>

int main(int agrc, char *agrv[])
{
    //HAS WORKED ON FOLLOWING HASH SETS:

    //NOTE: All 5 letter long passwords take 5-50 minutes to complete.
    //    HASH              PASSWORD
    //50fkUxYHbnXGw             rofl
    //50xcIMJ0y.RXo             YES
    //50mjprEcqC/ts             CA
    //50GApilQSG3E2             UPenn
    //50n0AAUD.pL8g             lloyd       //Took 48m24s
    //50CcfIk1QrPr6             maybe       //Took 6m12s
    //509nVI8B9VfuA             TF
    //50JIIyhDORqMU             nope
    //50JGnXUgaafgc             ROFL
    //51u8F0dkeDSbY             NO
    //50cI2vYkF0YU2             LOL


    //Ensure that user gave correct number of command line arguments
    if (agrc != 2)
    {
        printf("Usage: ./crack hash\n");
        return 1;
    }
    //All possible chars in password, in order of average letter frequency in english language (per Wikipedia)
    string possibleLetters = "\0etaoinshrdlucmfwypvbgkjqxzETAOINSHRDLUCMFWYPVBGKJQXZ";
    int letterCount = 57;

    //Get hash from argv and create salt
    string hash = agrv[1];
    char salt[3];
    memcpy(salt, hash, 2);
    salt[2] = '\0';

    //Create password attempt. Fill with all \0 for now so all char places not needed are ignored
    char attempt[6] = "\0\0\0\0\0\0";

    //Go through each char in attempt one at a time, brute force method to guess password
    for (int five = 0; five < letterCount; five++)
    {
        for (int four = 0; four < letterCount; four++)
        {
            for (int three = 0; three < letterCount; three++)
            {
                for (int two = 0; two < letterCount; two++)
                {
                    for (int one = 0; one < letterCount; one++)
                    {
                        //Create current attempt
                        attempt[0] = possibleLetters[one];
                        attempt[1] = possibleLetters[two];
                        attempt[2] = possibleLetters[three];
                        attempt[3] = possibleLetters[four];
                        attempt[4] = possibleLetters[five];

                        //Check current attempt against hash
                        if (strcmp(crypt(attempt, salt), hash) == 0)
                        {
                            printf("%s\n", attempt);
                            return 0;
                        }
                    }
                }
            }
        }
    }
    //If this point in code is reached, hash can not be cracked
    printf("Could not solve for hash %s\n", hash);
    return 1;
}