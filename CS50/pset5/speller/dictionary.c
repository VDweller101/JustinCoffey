// Implements a dictionary's functionality

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "dictionary.h"

// Returns true if word is in dictionary else false

//abcdefghijklmnopqrstuvwxyz'
#define CHARACTER_COUNT (27)
//Will ignore all words greater than 45 characters
#define MAX_LENGTH (45)
//root of trie structure will be globally available
struct Node *root;

unsigned int wordsLoaded = 0;

//Node for the trie structure
struct Node
{
    struct Node *children[CHARACTER_COUNT];
    bool endOfWord;
};

//Creation of new Node
struct Node *newNode(void)
{
    //Constructs new Node, setting all children to NULL
    struct Node *nNode = NULL;

    nNode = (struct Node *)malloc(sizeof(struct Node));

    if (nNode)
    {
        nNode->endOfWord = false;

        for (int i = 0; i < CHARACTER_COUNT; i++)
        {
            nNode->children[i] = NULL;
        }
    }
    else
    {
        printf("Could not malloc when creating new node.\n");
    }
    return nNode;
}

//Convert char to an index to be used in Nodes
int charToIndex(char letter)
{
    if (letter == '\'')
    {
        // ' is set to last place in node children. Return 26
        return 26;
    }
    else if ((int)letter >= 65 && (int)letter <= 90)
    {
        //Is uppercase letter
        return (int)letter - 65;
    }
    else if ((int)letter >= 97 && (int)letter <= 122)
    {
        //Is lowercase letter
        return (int)letter - 97;
    }
    else
    {
        //Is other character, return -1
        return -1;
    }
}

//Insert new word in to dictionary
void Insert(const char *word)
{
    if (word == NULL)
    {
        printf("Did not receive string on Insert().\n");
    }

    //Start at root
    struct Node *curPos = root;
    //Crawl through data structure
    for (int i = 0; i < strlen(word); i++)
    {
        //Convert char to index value
        int index = charToIndex(word[i]);
        //If no node at next position create one
        if (!curPos->children[index])
        {
            curPos->children[index] = newNode();
        }
        //Set curPos to next position
        curPos = curPos->children[index];
    }
    //Set last node visited to valid end of word
    curPos->endOfWord = true;
    //Add new word to total count
    wordsLoaded++;
}

//Check if word is in dictionary
bool check(const char *word)
{

    //Early exit if passed a blank string
    if (word[0] == '\0')
    {
        return false;
    }

    //Initialize first position of crawler
    struct Node *curPos = root;

    //Crawl through trie structure
    for (int i = 0; i < strlen(word) + 1; i++)
    {
        if (word[i] == '\0')
        {
            //At end of word, check if current node is possible end of a word
            if (curPos->endOfWord)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            //Not at end of word so convert char to index value
            int index = charToIndex(word[i]);

            //If no node to move to, word is misspelled
            if (!curPos->children[index])
            {
                return false;
            }
            //If there is a node to move to, move to is now
            else
            {
                curPos = curPos->children[index];
            }
        }
    }
    //If this point is reached there is an error
    printf("Reached end of Check() without finding solution.\n");
    return false;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Initialize trie structure
    root = newNode();

    //Go through dictionary, pick out each word
    char *curWord = malloc(MAX_LENGTH + 1);
    int curWordLength = 0;

    //Open file for reading
    FILE *file = fopen(dictionary, "r");

    //Get first char
    char curChar = fgetc(file);
    //Current place in file stream
    int curPlace = 0;
    //Whether or not we are currently building a word
    bool inWord = false;

    //While still in the file
    while (curChar != EOF)
    {
        //Currently in a word
        if (inWord)
        {
            //Check if current character is an alpha or '
            if (isalpha(curChar) || curChar == '\'')
            {
                //Still adding to word
                curWord[curWordLength] = curChar;
                curWordLength++;
            }
            else
            {
                //Word has ended
                curWord[curWordLength] = '\0';
                curWordLength = 0;
                Insert(curWord);
                for (int i = 0; i < 45; i++)
                {
                    //Reset placeholder to NULL values
                    curWord[i] = '\0';
                }
            }
        }
        else
        {
            //Not currently in a word, check if next char is a letter or apostraphe
            if (isalpha(curChar) || curChar == '\'')
            {
                inWord = true;
                curWord[curWordLength] = curChar;
                curWordLength++;
            }
        }

        curPlace++;
        curChar = fgetc(file);
    }
    //Dictionary read through successfully
    free(curWord);
    fclose(file);
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return wordsLoaded;
}

//Recursivly frees nodes 1 by 1 starting from end child of each node passed
bool freeNode(struct Node *node)
{
    for (int i = 26; i >= 0; i--)
    {
        if (node->children[i])
        {
            freeNode(node->children[i]);
        }
    }
    free(node);
    if (node)
    {
        //Successfully freed node
        return true;
    }
    else
    {
        return false;
    }
}
// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    if (freeNode(root))
    {
        //freeNode returns bool if it succeeded.
        return true;
    }

    return false;
}