// DO NOT MODIFY THE CONTENTS OF THIS FILE IT WILL BE REPLACED DURING GRADING
#include "const.h"

/* This variable will be in this format/name, but the string it is initalized to
 * will be at most 31 characters. Any characters which do not appear in the alphabet
 * string should be preserved.
**/
char Alphabet[] = "ABCDEFGHIJ!K#LMN$OP@QRSTUVWXYZ";

/* buffer which can be used for translations */
char buffer[10] = {0};

/**
 * The array for tutnese translation will always be in this format/name,
 * but the actual contents of each string and order of the strings will change.
 * Not all consonants may have a corresponding syllable. If they do not,
 * the character is preserved in the translation.
**/
char* Tutnese[22] = {
  "bub", "cash", "dud",
  "fud", "gug", "hash",
  "jay", "kuck", "lul", "mum", "nun",
  "pub", "quack", "rug", "sus", "tut",
  "vuv", "wack", "xux", "yuck", "zug", NULL
};

/* For example, an alternative declaration could be

char* Tutnese[16] = {
    "kik", "loo", "pup", "rap", "sas",
    "faf", "huh", "jack", "cat", "duck",
    "tut", "wack", "xux", "yum", "zoom", NULL
};

char* Tutnese[22] = {
  "jay", "kuck", "lul", "mum", "nun",
  "fud", "gug", "hash",
  "pub", "quack", "rug", "sus", "tut",
  "vuv", "wack", "xux", "yuck", "zug",
  "bub", "cash", "dud", NULL
};

*/
