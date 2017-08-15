#ifndef HW_H
#define HW_H
#include <stdlib.h>
#include <stdio.h>

#include "const.h"

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the program
 * and will return 1 byte (char) that will contain the information necessary for
 * the proper execution of the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @param in The OUTPUT_FILE of the program.
 * @param out The INPUT_FILE of the program.
 * @return Refer to homework document for the return value of this function.
 */
char validargs(int argc, char **argv, FILE **in, FILE **out);

int streq(char *str1, char *str2);

int strlength(char *str);

int atoui(char *str);

int iswhite(char c);

int isalphab(char c);

char startlet(char c, char *str);

char islowerc(char c);

char isvowel(char c);

char islet(char c);

char isdouble(char c1, char c2);

char* intut(char c, char** tut);

char* shiftstr(char shiftamt, char* str);

char samelet(char c1, char c2);

char toupperc(char c);

char tolowerc(char c);

#define USAGE(retcode) do{                                                        \
fprintf(stderr, "%s\n",                                                           \
"usage: ./hw1 [-s | -t | -h]\n"                                                   \
"    -s       Substitution cipher\n"                                              \
"             Additional parameters: [-e | -d] n INPUT_FILE OUTPUT_FILE\n"        \
"                 -e           Encrypt using the substitution cipher.\n"          \
"                 -d           Decrypt using the substitution cipher.\n"          \
"                  n           The amount of position to shift by.\n"             \
"                  INPUT_FILE  This can be any file on the file system or '-'\n"  \
"                              which specifies stdin.\n"                          \
"                  OUTPUT_FILE This can be any file on the system or '-'\n"       \
"                              which specifies stdout.\n"                         \
"\n"                                                                              \
"    -t       Tutnese Translation\n"                                              \
"             Additional parameters: [-e | -d]   INPUT_FILE OUTPUT_FILE\n"        \
"                 -e          Encode into tutnese\n"                              \
"                 -d          Decode from tutnese\n"                              \
"                 INPUT_FILE  This can be any file on the file system or '-'\n"   \
"                             which specifies stdin\n"                            \
"                 OUTPUT_FILE This can be any file on the system or '-'\n"        \
"                             which specifies stdout.\n"                          \
"\n"                                                                              \
"    -h       Display this help menu.");                                          \
exit(retcode);                                                                    \
} while(0)

#endif
