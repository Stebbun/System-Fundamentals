#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_SIZE 256
#define WORDLENGTH 50
#define MAX_MISSPELLED_WORDS 5

#define USAGE(return_code) do {\
fprintf(stderr, "%s\n",\
        "Usage: spell [-h] [-o OUT_FILE] [-i IN_FILE] [-d DICT_FILE] [-An]\n"\
        "Spell Checker using a custom dictionary. Auto corrects any known misspellings in the text.\n"\
        "Additional function to add new words and randomized misspellings to the dictionary.\n\n"\
        "\t-h\tDisplays this usage.\n"\
        "\t-o\tOUT_FILE filename, if omitted output to stdout\n"\
        "\t-i\tIN_FILE filename, if omitted input comes from stdin (CTRL+D to end input)\n"\
        "\t-d\tfor the dictionary filename, if omitted use default 'rsrc/dictionary.txt'\n"\
        "\t-An\tAutomatically add n (in range 0-5) random misspellings for any word not in the dictionary.\n");\
} while (0)

int modifiedDict;
struct dictionary* dict;
struct misspelled_word* m_list;
int m_list_size;
int m_words_found;
int unique_m_words;

struct Args{
    bool d;
    bool i;
    bool o;
    bool a;

    char n;

    char dictFile[MAX_SIZE];
    char input[MAX_SIZE];
    char output[MAX_SIZE];
};

struct dictionary{
    int num_words;
    struct dict_word* word_list;
};

struct dict_word{
    char word[WORDLENGTH];
    int misspelled_count; // initialize to 0
    int num_misspellings; // initialize to 0
    struct misspelled_word* misspelled[MAX_MISSPELLED_WORDS];
    struct dict_word* next;
};

struct misspelled_word{
    char word[WORDLENGTH];
    int misspelled; // initialize to 0
    struct dict_word* correct_word;
    struct misspelled_word* next;
};



/**
 * @brief      read in dictionary
 *
 * @param      f     opened dictionary file
 */
void processDictionary(FILE* f);


/**
 * @brief      Adds a word.
 *
 * @param      dWord  The d word
 * @param      word   The word
 */
void addWord(struct dict_word* dWord, char* word);



/**
 * @brief      Adds a misspelled word.
 *
 * @param      misspelledWord  The misspelled word
 * @param      correctWord     The correct word
 * @param      word            The word
 */
void addMisspelledWord(struct misspelled_word * misspelledWord, struct dict_word* correctWord, char* word);


/**
 * @brief      free the words
 *
 * @param      word  The word
 */
void freeWords(struct dict_word* word);

void freeMWords(struct misspelled_word* currWord);



/**
 * @brief      Print the words
 *
 * @param      word  The word
 * @param      f     { parameter_description }
 */
void printWords(struct dict_word* word, FILE* f);

 

/**
 * @brief      Process a word
 *
 * @param      inputWord  The input word
 */
void processWord(char* inputWord, bool a, char n);



/**
 * @brief      find the misspelling
 *
 * @param      inputWord  The input word
 *
 * @return     boolean
 */
bool foundMisspelledMatch(char* inputWord);



/**
 * @brief      find match in dictionary
 *
 * @param      inputWord  The input word
 *
 * @return     boolean
 */
bool foundDictMatch(char* inputWord);

void printStats();

void newDict(FILE* newDFile);
