#include "hw2.h"
#include "mispelling.h"

/* Great filename. */

void processDictionary(FILE* f){
	//initialize dictionary
    dict->num_words = 0;

    char line[MAX_SIZE];
    //this loop adds a word and its mispellings from the file
    while(fgets(line, MAX_SIZE+1, f) != NULL)
    {
        //initialize the current word.
        struct dict_word* currWord;
        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            printf("OUT OF MEMORY.\n");
            return;
        }
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        //variables
        char word[MAX_SIZE];		//the word
        char* wdPtr = word;			//pointer to word
        char* character = line;		//char pointer starting at line
        //char word_list[MAX_SIZE][MAX_MISSPELLED_WORDS+1]; // first index is the word, rest of the indices are mispelled words in the row
        int counter = 0;		//max should be 6. counts the word and mispellings
        int firstWord = 1;		//boolean whether its the first word or not

        //fgets(line, MAX_SIZE+1, f);
        //if there isn't a space at the end of the line, put one there
        if((line[strlen(line)-2] != ' ' && line[strlen(line)-1] == '\n') || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        // if(*character == ' ' && *(character+1) == EOF)
        // 	break;

        //bob huinj ugf ghjg
        while(*character != '\n' || *character != '\0' )
        {
            if(counter >= MAX_MISSPELLED_WORDS+1)
                break;
            if(*character == EOF || character == NULL || *character == '\0'){
            	break;
            }
            //if the character is a space, add the word in word_list and make word NULL.
            if(*character == ' ' )
            {
                *wdPtr = 0x0;	//put null terminator at end of word
                wdPtr = word;	//reset the word pointer to word starting address
                if(firstWord)
                {
                	//makes the current word the head of the dictionary word list
                    addWord(currWord, wdPtr);

                    firstWord = 0;
                }
                else
                {
                    struct misspelled_word* currMisspelling;
                    if((currMisspelling = malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }

                    addMisspelledWord(currMisspelling, currWord, wdPtr);
                }
                counter++;
            }
            //if the character isn't a space or a new line, add the character to word. edit: any whitespace
            else if(*character != '\n' && *character != ' ' && *character != EOF){
                (*wdPtr) = *character;
                wdPtr++;
            }
            character++;
        }
    }
    //delete last node
    // struct dict_word* temp = dict->word_list;
    // dict->word_list = dict->word_list->next;
    // free(temp);
    // dict->num_words--;
}

void addWord(struct dict_word* dWord, char* word){
    //setting up dWord fields
    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;
    dWord->next = dict->word_list;
    strcpy(dWord->word, word);
    dict->word_list = dWord;
    dict->num_words++;
}

void addMisspelledWord(struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){
    //setting up misspelledWord fields
    strcpy(misspelledWord->word, word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;
    misspelledWord->next = m_list;
    //(correctWord->misspelled)[++correctWord->num_misspellings] = misspelledWord;
    correctWord->misspelled[correctWord->num_misspellings] = misspelledWord;
    correctWord->num_misspellings++;
    m_list = misspelledWord;
    m_list_size++;
}

void freeWords(struct dict_word* currWord){
    while(currWord != NULL)
    {
        //freeWords(currWord);

        //int i;

        //free word

    	struct dict_word* temp = currWord;
    	temp = temp->next;

        free(currWord);
        currWord = temp;
    }
}

void freeMWords(struct misspelled_word* currWord){
    while(currWord != NULL)
    {
        //freeWords(currWord);

        //int i;

        //free word

    	struct misspelled_word* temp = currWord;
    	temp = temp->next;

        free(currWord);
        currWord = temp;
    }
}

void printWords(struct dict_word* currWord, FILE* f){
    while(currWord != NULL)
    {
        //printWords(currWord->next, f);

        char line[MAX_SIZE];
        int i;

        sprintf(line, "%s\n", currWord->word);
        fwrite(line, 1, strlen(line), f);

        sprintf(line, "\tNUMBER OF TIMES WORD IS MISSPELLED: %d\n", currWord->misspelled_count); // puts string into buffer
        fwrite(line, 1, strlen(line), f);

        sprintf(line, "\tNUMBER OF MISSPELLINGS: %d\n", currWord->num_misspellings);
        fwrite(line, 1, strlen(line)+1, f);

        for(i = 0; i<currWord->num_misspellings; i++)
        {
            sprintf(line, "\tMISPELLED WORD #%d: %s\n", i,((currWord->misspelled)[i])->word);
            fwrite(line, 1, strlen(line)+1, f);

            sprintf(line,"\t\tMISPELLED?: %d\n", ((currWord->misspelled)[i])->misspelled);
            fwrite(line, 1, strlen(line)+1, f);

            sprintf(line, "\t\tACTUAL WORD: %s\n", (((currWord->misspelled)[i])->correct_word)->word);
            fwrite(line, 1, strlen(line)+1, f);


            if( ((currWord->misspelled)[i])->next != NULL)
            {
                sprintf(line, "\t\tNEXT MISPELLED WORD: %s\n", ((currWord->misspelled)[i])->next->word);
                fwrite(line, 1, strlen(line)+1, f);
            }
        }

        if( (currWord->next) != NULL)
        {
            sprintf(line,"\tNEXT WORD: %s\n", (currWord->next)->word);
            fwrite(line, 1, strlen(line)+1, f);
        }
        currWord = currWord->next;
    }
}

void processWord(char* inputWord, bool a, char n){

    if(foundMisspelledMatch(inputWord))
        return;
    if(foundDictMatch(inputWord))
        return;
    if(a == false)
    	return;
    else
    {
    	//add to the dictionary
    	struct dict_word* newWord;
    	if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL){
    		printf("ERROR: OUT OF MEMORY.\n");
    		return;
    	}
    	//add word to dictionary
    	addWord(newWord, inputWord);

    	if(modifiedDict == 0)
    		modifiedDict = 1;

    	//generate n typo strings
    	char** typos = gentypos(n, inputWord);

    	//printf("word: %s length: %lo  misspellings: %s %s %s \n", inputWord, strlen(inputWord), typos[0], typos[1], typos[2]);

    	for(int i = 0; i < n; i++){

    		struct misspelled_word* newMWord;
    		if((newMWord = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL){
            	printf("ERROR: OUT OF MEMORY.");
                return;
            }
            //printf("word: %s length: %lo\n", (*typos)+i, strlen((*typos)+i));
            addMisspelledWord(newMWord, newWord, typos[i] );

            //need to free typos after using it
            free(typos[i]);
    	}
    	free(typos);
    	//create a

    	//remove all interactivity from here
        // char ch;
        // char conf;

        // do
        // {
        //     printf("\"%s\" was not found in the dictionary. Do you want to add it (Y/N)? ", inputWord);
        //     scanf("%c", &conf);
        //     while ((ch = getchar()) != '\n' && ch != EOF);
        // }while(conf!='Y' && conf!='N');

        // if(conf == 'Y')
        // {
        //     struct dict_word* newWord;
        //     //int counter = 0;

        //     if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        //     {
        //         printf("ERROR: OUT OF MEMORY.\n");
        //         return;
        //     }

        //     addWord(newWord, inputWord);
        //     dict->word_list = newWord;		//redundant
        //     printf("Added \"%s\" to Dictionary. Add misspellings (Y/N)? ", inputWord);

        //     do
        //     {
        //         scanf("%c", &conf);
        //         while ((ch = getchar()) != '\n' && ch != EOF);
        //     }while(conf!='Y' && conf!='N');

        //     if(conf=='Y')
        //     {
        //         int numMisspellings=0;
        //         do
        //         {
        //             printf("How many misspellings (1-5)?");
        //             scanf("%d", &numMisspellings);
        //             while ((ch = getchar()) != '\n' && ch != EOF);
        //         }while(numMisspellings<1 || numMisspellings>5);

        //         while(numMisspellings > 0)
        //         {
        //             char word[WORDLENGTH];
        //             char* wdPtr = word;
        //             struct misspelled_word* newMWord;

        //             if((newMWord = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
        //             {
        //                 printf("ERROR: OUT OF MEMORY.");
        //                 return;
        //             }
        //             printf("Enter misspelling: ");
        //             scanf("%s", word);
        //             addMisspelledWord(newMWord, newWord, wdPtr);
        //             printf("Misspelling added\n");
        //             while ((ch = getchar()) != '\n' && ch != EOF);
        //             numMisspellings--;
        //         }
        //     }
        // }
    }
}

bool foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
        	if(listPtr->correct_word->misspelled_count == 0){
        		unique_m_words++;
        	}
            strcpy(inputWord, listPtr->correct_word->word);		//this corrects the input word
            listPtr->misspelled = 1;							//boolean value to show it was misspelled
            listPtr->correct_word->misspelled_count++;			//increment misspell count
            m_words_found++;
            return true;
        }
        listPtr = listPtr->next;
    }
    return false;
}

bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return true;
        listPtr = listPtr->next;
    }
    return false;
}

void printStats(){
	fprintf(stderr, "Total number of words in dictionary: ");
	fprintf(stderr, "%i\n", dict->num_words);
	fprintf(stderr, "Size of dictionary (in bytes): ");
	fprintf(stderr, "%i\n", (int)(sizeof(struct dictionary) + sizeof(struct dict_word) * dict->num_words));
	fprintf(stderr, "Size of misspelled word list (in bytes): ");
	fprintf(stderr, "%i\n", (int)( sizeof(struct misspelled_word) * m_list_size));



	fprintf(stderr, "Total number of misspelled words: ");
	fprintf(stderr, "%i\n", m_words_found);

	int numtops = 0;
	if(unique_m_words > 3)
		numtops = 3;
	else
		numtops = unique_m_words;

	//what if there were no mispelled words?

	fprintf(stderr, "Top %i misspelled words:\n", numtops);

	//find top 3 words using an array of flags
	int flags[dict->num_words];
	//initialize all flags to 0
	memset(flags, 0, sizeof(int)*(dict->num_words));

	//flag index 0 corresponds to head of linkedlist of dict words
	//find max
	//i forget to reset the pointer back to the maxIndex

	for(int i = 0; i < numtops; i++){
		int counter = 0;
		int maxIndex = 0;
		struct dict_word* ptr = dict->word_list;
		int max = ptr->misspelled_count;
		while(ptr != NULL){
			if(ptr->misspelled_count >= max && flags[counter] == 0){
				max = ptr->misspelled_count;
				maxIndex = counter;
			}
			ptr = ptr->next;
			counter++;
		}
		flags[maxIndex] = 1;

		//reset the pointer to maxIndex
		ptr = dict->word_list;
		for(int j = 0; j < maxIndex; j++)
			ptr = ptr->next;

		fprintf(stderr, "%s (%i times): ", ptr->word, ptr->misspelled_count);

		//print all misspellings of the word

		//figure out number of commas i need
		int numCommas = 0;
		for(int j = 0; j < ptr->num_misspellings; j++){
			if(ptr->misspelled[j]->misspelled == 1){
				numCommas++;
			}
		}
		//1 less comma than there are words
		if(numCommas >= 1)
			numCommas--;

		//check if misspelled is 1
		int iscomma = 0;

		for(int j = 0; j < ptr->num_misspellings; j++){
			if(ptr->misspelled[j]->misspelled == 1){
				if(iscomma == 1 && numCommas > 0){
					fprintf(stderr, ", ");
					numCommas--;
				}
				fprintf(stderr, "%s", ptr->misspelled[j]->word);
			}
			iscomma = 1;
		}
		fprintf(stderr, "\n");
	}
}

void newDict(FILE* newDFile){
	struct dict_word* dictPtr = dict->word_list;
	while(dictPtr != NULL){
		//write the word to file, then all the misspellings, then a new line

		char word1[MAX_SIZE] = "";
		char* temp = dictPtr->word;
		strcat(word1, temp);
		strcat(word1, " ");

		fwrite(word1,  1, strlen(word1), newDFile);

		//printf("%s %i\n", word1, dictPtr->num_misspellings);
		//printf("word: %s length: %lo  misspellings:  \n", word1, strlen(word1));

		for(int i = 0; i < dictPtr->num_misspellings; i++){
			//printf("%s ", dictPtr->misspelled[i]->word);
			char word2[MAX_SIZE] = "";
			char* m_word = dictPtr->misspelled[i]->word;
			strcat(word2, m_word);

			if(i != (dictPtr->num_misspellings - 1))
				strcat(word2, " ");
			else
				strcat(word2, "\n");

			fwrite(word2, 1, strlen(word2), newDFile);
		}
		dictPtr = dictPtr->next;
	}
}
