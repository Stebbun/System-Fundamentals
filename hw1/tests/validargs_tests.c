#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <string.h>
#include "hw1.h"

#define HELP 0x80
#define SUBS 0x40
#define TUTE 0x40
#define ENCR 0x20
#define DECR 0x20
#define NVAL 0x1F
#define N320 0x08


/**
 * only use this function for printing out values immediately.
 * static variable risks being overwritten with successive calls.
 */
const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

Test(ValidArgs_Suite, help_menu){

    char *fake_args[] = {"program_name", "-h", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(2, fake_args, &in, &out);

    cr_assert_eq((mode & HELP), HELP, "Help menu bit wasn't set, got %s ", byte_to_binary(mode));
}

Test(ValidArgs_Suite, subs_decr){
    char *fake_args[] = {"program_name", "-s", "-d", "-", "-", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(5, fake_args, &in, &out);

    cr_assert_eq((mode & SUBS), SUBS, "Substitution bit wasn't set, got %s ", byte_to_binary(mode));
    cr_assert_eq((mode & DECR), DECR, "Decoding bit wasn't set, got %s ", byte_to_binary(mode));
}

Test(ValidArgs_Suite, subs_decr_n){
    char *fake_args[] = {"program_name", "-s", "-d", "-", "-", "3452", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(6, fake_args, &in, &out);

    cr_assert_eq((mode & SUBS), SUBS, "Substitution bit wasn't set, got %s ", byte_to_binary(mode));
    cr_assert_eq((mode & DECR), DECR, "Decoding bit wasn't set, got %s ", byte_to_binary(mode));

    int n = 3452;

    cr_assert_eq((mode & NVAL), n % (strlen(Alphabet)), "n value expected to be %d was %d", (int)(n % (strlen(Alphabet))), (mode & NVAL));
}

Test(ValidArgs_Suite, subs_encr){
    char *fake_args[] = {"program_name", "-s", "-e", "-", "-", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(5, fake_args, &in, &out);

    cr_assert_eq((mode & SUBS), SUBS, "Substitution bit wasn't set, got %s ", byte_to_binary(mode));
    cr_assert_eq((mode & ENCR), 0, "Encoding bit wasn't zero, got %s ", byte_to_binary(mode));
}

Test(ValidArgs_Suite, subs_encr_n){
    char *fake_args[] = {"program_name", "-s", "-e", "-", "-", "3425", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(6, fake_args, &in, &out);

    cr_assert_eq((mode & SUBS), SUBS, "Substitution bit wasn't set, got %s ", byte_to_binary(mode));
    cr_assert_eq((mode & ENCR), 0, "Encoding bit wasn't zero, got %s ", byte_to_binary(mode));

    int n = 3425;

    cr_assert_eq((mode & NVAL), n % (strlen(Alphabet)), "n value expected to be %d was %d",
                (int)(n % (strlen(Alphabet))), (mode & NVAL));
}

Test(ValidArgs_Suite, tuts_encr){
    char *fake_args[] = {"program_name", "-t", "-e", "-", "-", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(5, fake_args, &in, &out);

    cr_assert_eq((mode & TUTE), 0, "Tutense bit wasn't zero, got %s ", byte_to_binary(mode));
    cr_assert_eq((mode & ENCR), 0, "Encoding bit wasn't zero, got %s ", byte_to_binary(mode));

    cr_assert_eq((mode & NVAL), (320 % (strlen(Alphabet))), "n value expected to be %d was %d", (int)(320 % (strlen(Alphabet))), (mode & NVAL));

}

Test(ValidArgs_Suite, tuts_decr){
    char *fake_args[] = {"program_name", "-t", "-d", "-", "-", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(5, fake_args, &in, &out);

    cr_assert_eq((mode & TUTE), 0, "Tutense bit wasn't zero, got %s ", byte_to_binary(mode));
    cr_assert_eq((mode & DECR), DECR, "Decoding bit wasn't set, got %s ", byte_to_binary(mode));

    cr_assert_eq((mode & NVAL), (320 % (strlen(Alphabet))), "n value expected to be %d was %d", (int)(320 % (strlen(Alphabet))), (mode & NVAL));
}

Test(ValidArgs_Suite, bad_args){
    char *fake_args[] = {"program_name", "-c", "-f", "foobar", "fizzbuzz", NULL};
    FILE* in;
    FILE* out;

    char mode = validargs(5, fake_args, &in, &out);

    cr_assert_eq(mode, 0, "Bad args got a non-zero mode, got %s ", byte_to_binary(mode));
}

Test(ValidArgs_Suite, create_files){
    char *fake_args[] = {"program_name", "-s", "-e", "testfile", "foobar", NULL};
    FILE* in;
    FILE* out;

    FILE* testfile = fopen("testfile", "w");
    fclose(testfile);

    validargs(5, fake_args, &in, &out);

    cr_assert_not_null(in, "in file was NULL");

    fclose(in);

    cr_assert_not_null(out, "out file was NULL");

    fclose(out);
    cr_log_warn("Deleting created file.");
    remove("foobar");
    remove("testfile");
}
