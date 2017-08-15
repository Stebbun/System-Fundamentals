#include "mispelling.h"

char** gentypos(int n, char* word) {
	srand(time(NULL));
	char** typos = (char**) malloc(sizeof(char*) * n);
	int len = strlen(word);
	while(n--){
		int i = rand() % len;
		char nc = 97 + (rand() % 26);
		typos[n] = strdup(word);
		typos[n][i] = nc;
		//if they are not the same word, free the typo
		if(!strcmp(word, typos[n]))
			free(typos[n++]);
	}
	return typos;
}
