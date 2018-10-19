// Helper functions for music

#include <cs50.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "helpers.h"

// Converts a fraction formatted as X/Y to eighths
int duration(string fraction)
{
    int frac = fraction[2] - '0';
    int steps;

    if (fraction[2] == '8')
    {
        steps = fraction[0] - '0';
    }
    else
    {
        int tmp = fraction[2] - '0';
        steps = fraction[0] - '0';
        while (tmp != 8)
        {
            steps *= 2;
            tmp *= 2;
        }
    }
    return steps;
}

// Calculates frequency (in Hz) of a note
int frequency(string note)
{
    int sharpFlat = 0;
    int octave = 0;
    if (strlen(note) == 3)
    {
        //A#4
        if (note[1] == '#')
        {
            //Sharp
            sharpFlat = 1;
        }
        else
        {
            //Flat
            sharpFlat = -1;
        }
        octave = note[2] - '0';
    }
    else
    {
        octave = note[1] - '0';
    }
    float startFloat = 12 * (octave - 4) + sharpFlat;
    int start = round(startFloat);

    int letter = note[0] - 65;

    //At this point start is set to the A of whatever octave we are in +/- sharp or flat


    int actual;
    switch (letter)
    {
        case 1: //B
            actual = start + 2;
            break;
        case 2: //C
            actual = start - 9;
            break;
        case 3: //D
            actual = start - 7;
            break;
        case 4: //E
            actual = start - 5;
            break;
        case 5: //F
            actual = start - 4;
            break;
        case 6: //G
            actual = start - 2;
            break;
        default:
            actual = start;
            break;
    }
    float power = (float)actual / 12.0f;
    float hz = (float)pow(2.0f, power) * 440.0f;
    int trueHz = round(hz);
    return trueHz;
}

// Determines whether a string represents a rest
bool is_rest(string s)
{
    if (s[0] == '\0')
    {
        return true;
    }
    else
    {
        return false;
    }
}
