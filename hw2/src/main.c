#include "hw2.h"

int main(int argc, char *argv[]){
    FILE* DEFAULT_INPUT = stdin;
    FILE* DEFAULT_OUTPUT = stdout;
    char DEFAULT_DICT_FILE[]= "rsrc/dictionary.txt";
    //create dictionary
    if((dict = (struct dictionary*) malloc(sizeof(struct dictionary))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }

    /*if((m_list = (struct misspelled_word*) malloc(sizeof(struct misspelled_word*))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }
    m_list = NULL;*/

    //dict is not yet modified
    modifiedDict = 0;
    dict->num_words = 0;
    dict->word_list = NULL;

    struct Args args;
    // Set struct default values
    args.d = false;
    args.i = false;
    args.o = false;
    args.a = false;
    args.n = 0;
    m_list_size = 0;
    m_words_found = 0;
    unique_m_words = 0;
    strcpy(args.dictFile, DEFAULT_DICT_FILE);
    // Make a loop index
    //int i;
    char line[MAX_SIZE];
    //Declare Files
    FILE* dFile;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;

    //using getopt for command line args
    char opt = 0;
    while( (opt = getopt(argc, argv, "ho:i:d:A:")) != -1 ){
        switch(opt){
            case 'h' :
                USAGE(EXIT_SUCCESS);
                free(dict);
                return EXIT_SUCCESS;
                break;
            case 'o' :
                oFile = fopen( optarg, "w");
                args.o = true;
                break;
            case 'i' :
                iFile = fopen( optarg, "r");
                args.i = true;
                break;
            case 'd' :
                strcpy(args.dictFile, optarg);
                args.d = true;
                break;
            case 'A' :
                args.n = atoi(optarg);
                args.a = true;
                break;
            default:
                USAGE(EXIT_FAILURE);
                free(dict);
                return EXIT_FAILURE;
                break;
        }
    }

    dFile = fopen(args.dictFile, "r");


    if(iFile == NULL || dFile == NULL || !(args.n >= 0 && args.n <= 5) )
    {
        USAGE(EXIT_FAILURE);
        free(dict);
        return EXIT_FAILURE;
    }
    else
    {
        processDictionary(dFile);

    }

    while(fgets(line, MAX_SIZE+1, iFile) != NULL)
    {
        char word[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];
        char* character = line;
        bool inword = 0;

        //fgets(line, MAX_SIZE+1, iFile);

        //if there isn't a space or newline at the end of the line, put one there
        if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n')){
            strcat(line, " ");
        }
        //replaces spaces within a line with new lines
        while(*character != 0x0)
        {
            if( (*character == ' ' || *character == '\n' || *character == '\t' || *character == 0xB || *character == 0xC || *character == 0xD)  && inword == 1)
            {
                /*char* punct = wdPtr-1;
                    printf("char:%c",punct);
                while(!((*punct>='a' && *punct<='z') || (*punct>='A' && *punct<='Z')))
                {
                    punct--;
                }
                punct++;
                printf("%d", strlen(wdPtr)-strlen(punct));
                */
                inword = 0;

                    *wdPtr = 0x0;
                    wdPtr = word;

                    processWord(wdPtr, args.a, args.n);

                    char temp[] = " ";
                    *temp = *character;

                    strcat(wdPtr, temp);                         //why concat a space at the end? what about preserving whitespace characters?

                    //to lower
                    char lowerWord[] = "";
                    strcat(lowerWord, wdPtr);

                    for(int i = 0; i < (int)strlen(lowerWord); i++){
                        lowerWord[i] = tolower(lowerWord[i]);
                    }

                    fwrite(lowerWord, 1, strlen(lowerWord), oFile);   //write the word to output
            }
            else if(*character == '\n'){
                fwrite("\n", 1, strlen("\n"), oFile);
            }
            else
            {
                if(!(*character == ' ' || *character == '\n' || *character == '\t' || *character == 0xB || *character == 0xC || *character == 0xD)){
                    inword = 1;
                    (*wdPtr) = *character;
                    wdPtr++;
                }
                else{
                    char temp[] = " ";
                    *temp = *character;
                    fwrite(temp, 1, strlen(temp), oFile);
                }
            }
            character++;
        }

        if(iFile == stdin)
            break;
    }

    if(modifiedDict == 1){
        //create new dictionary in the same directory as old dictionary file
        //search through args.dictFile
        //iterate backwards until you encounter first / and save this pointer
        //make a substring starting at the pointer + 1
        char* ptr0 = args.dictFile;
        char* ptr1 = args.dictFile;
        char* ptr2;

        while(*ptr1 != '\0'){
            ptr1++;
        }
        //ptr1 is now at end of string
        while(*ptr1 != '/' && ptr1 != ptr0){
            ptr1--;
        }
        char newFileName[MAX_SIZE];
        if(ptr1 == ptr0){
            //just use ptr0 as file name
            strcpy(newFileName, "new_");
            strcat(newFileName, ptr0);
        }
        else{

            //ptr1 is now at last /
            ptr2 = ptr1++;
            //ptr2 points at beginning of file name

            //number of chars to take from first part of string
            int numChars = (ptr1 - ptr0);

            strncpy(newFileName, ptr0, numChars);
            newFileName[numChars] = '\0';

            //append "new_"
            strcat(newFileName, "new_");

            //append ptr2
            strcat(newFileName, ++ptr2);
        }

        FILE* newDFile;
        newDFile = fopen(newFileName, "w");

        newDict(newDFile);
        fclose(newDFile);
    }

    //printWords(dict->word_list , oFile);
    printStats();

    //printf("\n--------FREED WORDS--------\n");
    freeWords(dict->word_list);
    //free m_list
    freeMWords(m_list);
    //free dictionary
    free(dict);

    fclose(dFile);
    fclose(iFile);
    fclose(oFile);
    return EXIT_SUCCESS;
}
