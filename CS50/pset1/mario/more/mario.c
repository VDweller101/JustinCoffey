#include <stdio.h>
#include <cs50.h>

int main(void)
{
    int n;
    do
    {
        //prompt user for number between 0 and 23, keep asking for number if they give one outside that range
        n = get_int("Height: ");
    }
    while (n < 0 || n > 23);

    for (int i = 1; i <= n; i++)
    {
        //print number of spaces n - i
        for (int j = 0; j < (n - i); j++)
        {
            printf(" ");
        }
        //print i #
        for (int k = 0; k < i; k++)
        {
            printf("#");
        }
        //print gap
        printf("  ");
        //print i #
        for (int l = 0; l < i; l++)
        {
            printf("#");
        }
        //go to next line
        printf("\n");
    }
}