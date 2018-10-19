#include <stdio.h>
#include <cs50.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        //If they did not have only 1 command line argument
        printf("Usage: ./caesar k\n");
        return 1;
    }
    //Convert key argument to an int
    int key = atoi(argv[1]);
    string plainText;
    //Get plaintext from user, keep asking if string is blank
    do
    {
        plainText = get_string("plaintext: ");
    }
    while (plainText[0] == '\0');

    printf("ciphertext: ");

    //Go through entire string and check if each char is upper, lower or special char
    //Slide char up accordingly
    int textLength = 0;
    while (plainText[textLength] != '\0')
    {
        if (isupper(plainText[textLength]))
        {
            printf("%c", (((plainText[textLength] + key - 'A') % 26) + 'A'));
        }
        else if (islower(plainText[textLength]))
        {
            printf("%c", (((plainText[textLength] + key - 'a') % 26) + 'a'));
        }
        else
        {
            printf("%c", plainText[textLength]);
        }
        textLength++;
    }
    printf("\n");
}