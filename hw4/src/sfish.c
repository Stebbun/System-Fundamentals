#include "sfish.h"

char** cmdParser(int argC, char* cmd){
	char** argV;

	if( (argV = malloc((argC+1) * sizeof(char*))) == NULL ){
		//mem error
		printf("%s\n", "NOT ENOUGH MEMORY.");
	}
	argV[argC] = NULL;

	int maxTokLen = getMaxTokLen(cmd) + 1;

	for(int i = 0; i < argC; i++){
		if( (argV[i] = malloc( sizeof(char)*maxTokLen )) == NULL ){
			//mem error
			printf("%s\n", "NOT ENOUGH MEMORY.");
		}
	}

	parseArgs(argC, argV, cmd);

	return argV;
}

void parseArgs(int argC, char** argV, char* cmd){
	int inToken = 0;
	int index = 0;
	char* ptr = cmd;
	int counter = 0;

	while( isWhitespace(*ptr)  == 1){
		ptr++;
	}

	char* tokPtr = ptr;

	while( *ptr != 0 ){
		if( isWhitespace(*ptr) == 0){
			if(inToken == 0){
				tokPtr = ptr;
			}

			inToken = 1;
			counter++;
			ptr++;
		}
		if(inToken == 1 && isWhitespace(*ptr) == 1){
			inToken = 0;

			strncpy(argV[index], tokPtr, counter+1);
			argV[index][counter] = '\0';

			index++;
			counter = 0;
		}
		if(inToken == 0 && isWhitespace(*ptr) == 1){
			ptr++;
		}
		if(inToken == 1 && *ptr == 0 ){
			strcpy(argV[index], tokPtr);
		}
	}
}

int checkNumArgs(char* cmd){
	int result = 0;
	int inToken = 0;

	//cmd is a null terminated string
	char* ptr = cmd;

	//ignore any initial whitespaces
	while( isWhitespace(*ptr)  == 1){
		ptr++;
	}

	//beginning of first token
	while( *ptr != 0 ){
		if( isWhitespace(*ptr) == 0){
			inToken = 1;
			ptr++;
		}
		if(inToken == 1 && isWhitespace(*ptr) == 1){
			result++;
			inToken = 0;
		}
		if(inToken == 0 && isWhitespace(*ptr) == 1){
			ptr++;
		}
		if(inToken == 1 && *ptr == 0 ){
			result++;
		}
	}

	//return num of tokens
	return result;
}

int getMaxTokLen(char* cmd){
	int counter = 0;
	int result = 0;
	int inToken = 0;

	//cmd is a null terminated string
	char* ptr = cmd;

	//ignore any initial whitespaces
	while( isWhitespace(*ptr)  == 1){
		ptr++;
	}

	//beginning of first token
	while( *ptr != 0 ){
		if( isWhitespace(*ptr) == 0){
			inToken = 1;
			ptr++;
			counter++;
		}
		if(inToken == 1 && isWhitespace(*ptr) == 1){
			inToken = 0;
			if(counter > result){
				result = counter;
			}
			counter = 0;
		}
		if(inToken == 0 && isWhitespace(*ptr) == 1){
			ptr++;
		}
		if(inToken == 1 && *ptr == 0 ){
			if(counter > result){
				result = counter;
			}
		}
	}

	return result;
}

int isWhitespace(char c){
	if(c == 32 || c == 9 || c == 10 || c== 11 || c == 12 || c ==13){
		return 1;
	}
	return 0;
}

void built_help(){

}

void built_pwd(){
	char dirStr[512];

	if( (getcwd(dirStr, sizeof(dirStr))) != NULL ){
		printf("%s\n", dirStr);
	}
	else{
		printf("%s\n", "ERROR OCCURRED WITH PWD");
	}
}

void setcwd(char* block){
	char cwdBLock[512];

	//sets the current working directory.
    getcwd(cwdBLock, sizeof(cwdBLock));

    strcpy(block, cwdBLock);
}

void built_cd_none(char* home){
	//get home directory path
	chdir(home);
}

void built_cd_dash(char* lastDir){
	chdir(lastDir);
}

int built_cd(char* currDir, char* dir){
	char* fullDir;
	char* slash = "/";

	size_t len = strlen(currDir) + strlen(dir) + 1;

	if( (fullDir = malloc(len * sizeof(char))) == NULL){
    	printf("%s\n", "NOT ENOUGH MEMORY.");
    }

    strcpy(fullDir, currDir);
    strcat(fullDir, slash);
    strcat(fullDir, dir);

    int ret = chdir(fullDir);
    if(ret == -1){
    	printf("%s\n", "ERROR: DIRECTORY NOT FOUND");
    }

    free(fullDir);

    return ret;
}

void built_cd_dot_2(char* dir){
	//dir is the current directory we are in
	char result[512];
	int lastIndex = 0;

	//find the index of last /
	for(int i = 0; i < strlen(dir); i++){
		if( dir[i] == '/' ){
			lastIndex = i;
		}
	}

	strcpy(result, dir);
	result[lastIndex] = '\0';

	chdir(result);
}

void handle_exec(char** argV, char* path){
	pid_t pid;

	pid = fork();

	if( pid == 0 ){
		//this is child

		//if the executable contains a / then stat(2) the file

		handle_exec_helper(argV, path);
	}
	else if(pid < 0){
		//error
		printf("%s\n", "FORKING ERROR");
	}
	else{
		//this is parent
		int status;
		waitpid(pid, &status, 0);
	}
}

void handle_exec_helper(char** argV, char* path){
	if( containsSlash(argV[0]) ){
		//stat the file, if it exists, exec(3)
		struct stat buffer;
		if( stat(argV[0], &buffer) == 0 ){
			execv(argV[0], argV);

			//if it gets to here it failed
			exit(EXIT_FAILURE);
		}
		printf("%s\n", "COMMAND NOT FOUND");
	}
	else{
		//does not contain a /, search through PATH
		struct stat buffer;

		int isFound = 0;
		char* tok;
		char charBuff[256];
		tok = strtok(path, ":");

		strcpy(charBuff, tok);
		strcat(charBuff, "/");
		strcat(charBuff, argV[0]);

		//check if charBuff exists
		if( stat(charBuff, &buffer) == 0 ){
			execv(charBuff, argV);
			//if it gets to here it failed
			exit(EXIT_FAILURE);
		}
		else{
			//else loop through rest of tokens
			while( tok != NULL && isFound == 0 ){
				strcpy(charBuff, tok);
				strcat(charBuff, "/");
				strcat(charBuff, argV[0]);

				//check if charBuff exists
				if( stat(charBuff, &buffer) == 0 ){
					isFound = 1;
					execv(charBuff, argV);
					//if it gets to here it failed
					exit(EXIT_FAILURE);
				}

				tok = strtok(NULL, ":");
			}
		}
		printf("%s\n", "COMMAND NOT FOUND");
	}
}

int containsSlash(char* str){
	char* ptr = str;

	while( *ptr != '\0' ){
		if(*ptr == '/'){
			return 1;
		}
		ptr++;
	}


	return 0;
}

int check_redir_left(char** argV, int argC){
	for(int i = 0; i < argC; i++){
		if( strcmp(argV[i], "<") == 0 ){
			return 1;
		}
	}

	return 0;
}

int check_redir_right(char** argV, int argC){
	for(int i = 0; i < argC; i++){
		if( strcmp(argV[i], ">") == 0 ){
			return 1;
		}
	}

	return 0;
}

int check_num_pipes(char** argV, int argC){
	int counter = 0;

	for(int i = 0; i < argC; i++){
		if( strcmp(argV[i], "|") == 0 ){
			counter++;
		}
	}

	return counter;
}

void handle_redir_right(char** argV, int argC, char* path){
	//find location of the <, add a pointer that pointers to the index after <, null the <
	char* inputFileName;

	for(int i = 0; i < argC; i++){
		if( strcmp(argV[i], ">") == 0 ){
			if( (i+1) < argC ){
				inputFileName = argV[i+1];
				argV[i] = NULL;
			}
			else{
				printf("%s\n", "ERROR: INVALID COMMAND");
				return;
			}
		}
	}

	pid_t pid;

	pid = fork();

	if(pid == 0){
		//child
		FILE* file = fopen(inputFileName, "w");

		if( file == NULL){
			printf("%s\n", "FILE NOT FOUND");
			exit(EXIT_FAILURE);
		}
		else{
			//continue
			int newFD = fileno(file);

			//1 is stdin
			dup2(newFD, 1);
			handle_exec_helper(argV, path);
		}

	}
	else if(pid < 0){
		printf("%s\n", "FORK ERROR");
	}
	else{
		//parent
		int status;
		waitpid(pid, &status, 0);
	}
}

void handle_redir_left(char** argV, int argC, char* path){
	//find location of the <, add a pointer that pointers to the index after <, null the <
	char* inputFileName;

	for(int i = 0; i < argC; i++){
		if( strcmp(argV[i], "<") == 0 ){
			if( (i+1) < argC ){
				inputFileName = argV[i+1];
				argV[i] = NULL;
			}
			else{
				printf("%s\n", "ERROR: INVALID COMMAND");
				return;
			}
		}
	}

	pid_t pid;

	pid = fork();

	if(pid == 0){
		//child
		FILE* file = fopen(inputFileName, "r");

		if( file == NULL){
			printf("%s\n", "FILE NOT FOUND");
			exit(EXIT_FAILURE);
		}
		else{
			//continue
			int newFD = fileno(file);

			//0 is stdin
			dup2(newFD, 0);
			handle_exec_helper(argV, path);
		}

	}
	else if(pid < 0){
		printf("%s\n", "FORK ERROR");
	}
	else{
		//parent
		int status;
		waitpid(pid, &status, 0);
	}

}

void handle_redir_left_right(char** argV, int argC, char* path){
	//find location of the <, add a pointer that pointers to the index after <, null the <
	char* inputFileName;
	char* outputFileName;

	for(int i = 0; i < argC; i++){
		if( strcmp(argV[i], "<") == 0 ){
			if( (i+1) < argC ){
				inputFileName = argV[i+1];
				argV[i] = NULL;
				i++;
			}
			else{
				printf("%s\n", "ERROR: INVALID COMMAND");
				return;
			}
		}
		if( strcmp(argV[i], ">") == 0 ){
			if( (i+1) < argC ){
				outputFileName = argV[i+1];
				argV[i] = NULL;
			}
			else{
				printf("%s\n", "ERROR: INVALID COMMAND");
				return;
			}
		}
	}

	pid_t pid;

	pid = fork();

	if(pid == 0){
		//child
		FILE* infile = fopen(inputFileName, "r");
		FILE* outfile = fopen(outputFileName, "w");

		if( infile == NULL ){
			printf("%s\n", "FILE NOT FOUND");
			exit(EXIT_FAILURE);
		}
		else{
			//continue
			int newInFD = fileno(infile);
			int newOutFD = fileno(outfile);

			//0 is stdin
			dup2(newInFD, 0);
			dup2(newOutFD, 1);
			handle_exec_helper(argV, path);
		}

	}
	else if(pid < 0){
		printf("%s\n", "FORK ERROR");
	}
	else{
		//parent
		int status;
		waitpid(pid, &status, 0);
	}
}

void handle_redir_pipes(char** argV, int argC, char* path, int numPipes){

	pid_t opid;
	opid = fork();
	if(opid == 0){
		pid_t pid;
		int inputPipe = 0;
		int pipefd[2];
		int pipeIndex[numPipes];
		int pipeCounter = 0;

		char** argvPtr = argV;

		//update programIndex to pipeIndex[i] + 1 after each iteration

		for(int i = 0; i < argC; i++){
			if( strcmp(argV[i], "|") == 0 ){
				if(i+1 < argC){
					argV[i] = NULL;
					pipeIndex[pipeCounter] = i;
				}
				else{
					//error
				}
			}
		}

		//printf("%s\n", argV[0]);
		//printf("%s\n", argV[3]);
		for(int i = 0; i < numPipes; i++){
			pipe(pipefd);

			pid = fork();

			if(pid == 0){
				//if input pipe is not stdin, make it stdin
				if( inputPipe != 0){
					dup2(inputPipe, 0);
					close(inputPipe);
				}
				if( pipefd[1] != 1 ){
					dup2(pipefd[1], 1);
					close(pipefd[1]);
				}
				handle_exec_helper(argvPtr, path);
			}
			else if(pid < 0){
				//fork error
				printf("%s\n", "fork error");
			}
			else{
				int status;
				waitpid(pid, &status, 0);
			}
			if(pipeCounter < numPipes){
				argvPtr = argV + (pipeIndex[pipeCounter]+1);
				pipeCounter++;
			}

			close(pipefd[1]);
			inputPipe = pipefd[0];
		}
		if(inputPipe != 0){
			dup2(inputPipe, 0);
		}
		argvPtr = argV + pipeIndex[numPipes-1]+1;
		handle_exec_helper(argvPtr, path);
	}
	else if(opid < 0){
		//error
		printf("%s\n", "fork error");
	}
	else{
		int status;
		waitpid(opid, &status, 0);
	}
}

void handle_alarm(int sig){
	signal(SIGALRM, SIG_IGN);
	printf("\nYour %i second timer has finished!\n", numSecs);
	printf("%s", currentPrompt);
	signal(SIGALRM, handle_alarm);
}

void sigusr_handler(int sig){
	signal(SIGUSR2, SIG_IGN);
	printf("\nWell that was easy.\n");
	printf("%s", currentPrompt);
	signal(SIGUSR2, sigusr_handler);
}

void stp_handler(int sig){
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTSTP, stp_handler);
}

void sigchild_handler(int sig, siginfo_t* siginfo, void* context){
	int pid = siginfo->si_pid;
	double time = siginfo->si_stime + siginfo->si_utime;
	double clk_tck = sysconf(_SC_CLK_TCK);
	int cpuTime = (time/clk_tck) * 1000;
	printf("\nChild with PID %i has died. It spent %i milliseconds utilizing the CPU.\n", pid, cpuTime);
}
