#include "sfish.h"
#include "debug.h"

/*
 * As in previous hws the main function must be in its own file!
 */

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/
    char *cmd;
    int argC;
    char** argV;

    char* home = getenv("HOME");
    char* path = getenv("PATH");

    char lastDir[512];

    char currDir[512];

    char buffer[512];

    char prompt[256];

    int isBuilt = 0;
    int isRedir = 0;

    strcpy(prompt, "steli> : ");
	setcwd(buffer);
	strcat(prompt, buffer);
	strcat(prompt, " ");
	currentPrompt = prompt;
	signal(SIGUSR2, sigusr_handler);
	//signal(SIGTSTP, stp_handler);

	sigset_t mask, pmask;
	sigemptyset(&mask);
	sigemptyset(&pmask);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_BLOCK, &mask, &pmask);

	struct sigaction sigact;
	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_sigaction = &sigchild_handler;
	sigact.sa_flags = SA_SIGINFO;
	sigaction(SIGCHLD, &sigact, NULL);

    while((cmd = readline(prompt)) != NULL) {
        /* All your debug print statements should use the macros found in debu.h */
        /* Use the `make debug` target in the makefile to run with these enabled. */
        //info("Length of command entered: %ld\n", strlen(cmd));
        /* You WILL lose points if your shell prints out garbage values. */

        argC = checkNumArgs(cmd);

        argV = cmdParser(argC, cmd);

        //handle builtins
        //exit
        if(argC > 0){
	        if(strcmp(argV[0], "exit") == 0){
	        	isBuilt = 1;
	        	if(argC == 1){
	        		sigprocmask(SIG_SETMASK, &pmask, NULL);
	        		break;
	        	}
	        	else{
	        		printf("%s\n", "ERROR: INVALID COMMAND");
	        	}
	        }

	        //help
	        if (strcmp(argV[0], "help") == 0){
	        	isBuilt = 1;
	        	if(argC == 1){
	        		built_help();
	        	}
	        	else{
	        		printf("%s\n", "ERROR: INVALID COMMAND");
	        	}
	        }

	        //pwd
	        if(strcmp(argV[0], "pwd") == 0){
	        	isBuilt = 1;
	        	if(argC == 1){
	        		pid_t pid = fork();
	        		if(pid == 0){
	        			built_pwd();
	        		}
	        		else if(pid < 0){
	        			//fork error
	        			printf("%s\n", "FORK ERROR");
	        		}
	        		else{
	        			int status;
						waitpid(pid, &status, 0);
	        		}
	        	}
	        	else{
	        		printf("%s\n", "ERROR: INVALID COMMAND");
	        	}
	        }
	        //alarm
	        if(strcmp(argV[0], "alarm") == 0){
	        	isBuilt = 1;
	        	if(argC == 2){
	        		int num = atoi(argV[1]);

	        		if(num < 0){
	        			printf("%s\n", "ERROR: INVALID INARGUMENTS");
	        		}
	        		else{
	        			numSecs = num;
	        			signal(SIGALRM, handle_alarm);
	        			alarm(numSecs);
	        		}
	        	}
	        	else{
	        		printf("%s\n", "ERROR: INVALID INARGUMENTS");
	        	}
	        }

	        setcwd(currDir);

	        //cd
	        if(strcmp(argV[0], "cd") == 0){
	        	isBuilt = 1;
	        	if(argC == 1){
	        		//set last directory
	        		setcwd(lastDir);
	        		//go to home directory
	        		built_cd_none(home);
	        		setcwd(currDir);
	        	}
	        	else if(argC == 2 && strcmp(argV[1], "-") == 0){

				    //save currDir and save it to lastDIr after cd
				    setcwd(buffer);	//save hw4/src

				   	built_cd_dash(lastDir);	//go back to hw4
				   	setcwd(currDir);		//set currDir as hw4

				   	strcpy(lastDir, buffer);	//set lastDIr as buffer
	        	}
	        	else if(argC == 2 && strcmp(argV[1], ".") == 0){
	        		//do nothing?
	        	}
	        	else if(argC == 2 && strcmp(argV[1], "..") == 0){
	        		setcwd(lastDir);
	        		built_cd_dot_2(currDir);
	        		setcwd(currDir);
	        	}
	        	else if(argC == 2){

				    //save currDir and save it to lastDIr after cd
				    setcwd(buffer);

	        		if(built_cd(currDir, argV[1]) == 0){
	        			strcpy(lastDir, buffer);
	        			setcwd(currDir);
	          		}
	        	}
	        	else if(argC > 2){
	        		printf("%s\n", "ERROR: INVALID COMMAND");
	        	}
	        }

	        //part 3
	        //if it contains < and >
	        if( check_redir_right(argV, argC) == 1 && check_redir_left(argV, argC) == 1 ){
	        	isRedir = 1;
	        	handle_redir_left_right(argV, argC, path);
	        }
	        else if( check_redir_right(argV, argC) == 1 && check_redir_left(argV, argC) == 0 ){
	        	//contains > only
	        	isRedir = 1;
	        	handle_redir_right(argV, argC, path);
	        }
	        else if( check_redir_right(argV, argC) == 0 && check_redir_left(argV, argC) == 1 ){
	        	//contains < only
	        	isRedir = 1;
	        	handle_redir_left(argV, argC, path);
	        }
	        else if( check_num_pipes(argV, argC) == 1 ){
	        	//contains one pipe
	        	isRedir = 1;
	        	handle_redir_pipes(argV, argC, path, 1);
	        }
	        else if( check_num_pipes(argV, argC) == 2 ){
	        	//contains two pipes
	        	isRedir = 1;
	        	handle_redir_pipes(argV, argC, path, 2);
	        }


	        //part 2
	        if(isBuilt == 0 && isRedir == 0){
	        	handle_exec(argV, path);
	        }


	        isRedir = 0;
	        isBuilt = 0;
	        strcpy(prompt, "steli> : ");
			setcwd(buffer);
			strcat(prompt, buffer);
			strcat(prompt, " ");
			currentPrompt = prompt;

			for(int i = 0; i < argC; i++){
	    		free(argV[i]);
	    	}
	    	free(argV);
	    }
    }

    /* Don't forget to free allocated memory, and close file descriptors. */
    free(cmd);

    return EXIT_SUCCESS;
}
