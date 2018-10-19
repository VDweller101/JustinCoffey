#include <stdio.h>
#include <cs50.h>

int main(void)
{
    long long cardNumber;
    do
    {
        cardNumber = get_long_long("Number: ");
    }
    while (cardNumber < 0);

    //Check length of card number input
    int digitCount = 0;
    long long digits = cardNumber;

    while (digits > 0)
    {
        digits = digits / 10;
        digitCount++;
    }

    //if card number is not 13, 15, or 16 digits long print INVALID
    if (digitCount != 13 && digitCount != 15 && digitCount != 16)
    {
        printf("INVALID\n");
    }
    else
    {
        //Create and populate array of int with card number
        int revNumber[digitCount];
        digits = cardNumber;
        //first array populated backwards
        for(int i = 0; i < digitCount; i++)
        {
            revNumber[i] = digits % 10;
            digits = digits / 10;
        }
        //second array populated in correct order
        int number[digitCount];
        for(int i = 0; i < digitCount; i++)
        {
            number[(digitCount - i) - 1] = revNumber[i];
        }

        //Check card number based on number of digits.
        int val = 0;
        int temp = 0;

        //Start with second to last digit and grab every other digit from right to left
        for (int i = digitCount - 2; i >= 0; i = i - 2)
        {
            //Double each digit
            temp = number[i] * 2;
            //if doubled digit over 10, add digits of number, otherwise just add number
            if (temp >= 10)
            {
                val += temp % 10;
                val++;
            }
            else
            {
                val += temp;
            }
        }
        //Add in other numbers
        for (int i = digitCount - 1; i >= 0; i = i - 2)
        {
            val += number[i];
        }

        if (val % 10 == 0)
        {
            switch (digitCount)
            {
                case 13:
                //Check if starts with 4 for Visa
                if (number[0] == 4)
                {
                    printf("VISA\n");
                }
                else
                {
                    printf("INVALID\n");
                }
                break;

                case 15:
                //Check if number starts with 34 or 37 for American Express
                if (number[0] == 3)
                {
                    if(number[1] == 4 || number[1] == 7)
                    {
                        printf("AMEX\n");
                    }
                    else
                    {
                        printf("INVALID\n");
                    }
                }
                else
                {
                    printf("INVALID\n");
                }
                break;
                
                case 16:
                //Check if number starts with 4 for Visa
                if (number[0] == 4)
                {
                    printf("VISA\n");
                }
                //Or if t starts with 51-55 for MasterCard
                else if (number[0] == 5)
                {
                    if (number[1] >= 1 && number[1] <= 5)
                    {
                        printf("MASTERCARD\n");
                    }
                    else
                    {
                        printf("INVALID\n");
                    }
                }
                else
                {
                    printf("INVALID\n");
                }
                break;
            }
        }
        else
        {
            printf("INVALID\n");
        }
    }
}