#ifndef SFISH_H
#define SFISH_H
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#endif

char** cmdParser(int argC, char* cmd);
int checkNumArgs(char* cmd);
int isWhitespace(char c);
int getMaxTokLen(char* cmd);
void parseArgs(int argC, char** argV, char* cmd);

void built_help();
void built_pwd();
void built_cd_none(char* home);
void built_cd_dash(char* lastDir);
void built_cd_dot_2(char* dir);
int built_cd(char* currDir, char* dir);

void setcwd(char* cwdBLock);

void handle_exec(char** argV, char* path);
int containsSlash(char* str);

int check_redir_left(char** argV, int argC);
int check_redir_right(char** argV, int argC);
int check_num_pipes(char** argV, int argC);

void handle_redir_right(char** argV, int argC, char* path);
void handle_redir_left(char** argV, int argC, char* path);
void handle_redir_left_right(char** argV, int argC, char* path);
void handle_redir_pipes(char** argV, int argC, char* path, int numPipes);

void handle_exec_helper(char** argV, char* path);

void handle_alarm(int sig);
void sigusr_handler(int sig);
void stp_handler(int sig);
void sigchild_handler(int sig, siginfo_t* siginfo, void* context);

int numSecs;
char* currentPrompt;
