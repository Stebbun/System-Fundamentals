#include "hw1.h"
#include "info.h"

int main(int argc, char **argv) {

    FILE* in;
    FILE* out;
    //char* shiftedalpha = Alphabet;
    char shiftamt;
    /* Note: create a variable to assign the result of validargs */
    char mode = validargs(argc, argv, &in, &out);

    //handle failure
    if( mode == 0 ){
    	USAGE(mode);
    	return EXIT_FAILURE;
    }

    //handle h flag
    if( mode == (char)0x80 ){
    	USAGE(mode);
    	return EXIT_SUCCESS;
    }

    //handle substitution cipher
    char temp = 0x40;		//7th bit represents s
    if( (temp & mode) == 0x40 ){
    	//substitution cipher was selected
    	//now we check if encryption or decryption was selected
    	temp = 0x20;		//6th bit represents d
    	if( (temp & mode) == 0x20 ){
    		//decryption was selected
    		char c;
    		while( !feof(in) ){
    			//get the char value
    			c = getc(in);
    			if( isalphab(c) == 1 ){
    				//then we have to shift it
    				//get the shift amount from lower 5 bits of mode
    				temp = 0x1F;
    				shiftamt = temp & mode;

    				//set cursor to starting letter
    				char *cursor = Alphabet + startlet(c, Alphabet);


    				//decrement to cursor until shiftamt is reached. wrap around if front of string
    				for(char i = 0; i < shiftamt; i++){
    					if( cursor == Alphabet ){
    						cursor = Alphabet + strlength(Alphabet);
    					}
    					cursor--;
    				}

    				c = *cursor;
    				putc( c, out );
    			}
    			else{
    				if(c != EOF){
    					putc( c, out );
    				}
    			}
    		}
    	}
    	else{
    		//encryption was selected
    		char c;
    		while( !feof(in) ){
    			//get the char value
    			c = getc(in);

    			//if its whitespace, we dont shift
    			//if its a letter(upper or lower) or !, @, #, $, we shift
    			if( iswhite(c) == 1 ){
    				//then output it
    				putc( c, out );
    			}
    			else if( isalphab(c) == 1 ){
    				//then we have to shift it
    				//get the shift amount from lower 5 bits of mode
    				temp = 0x1F;
    				shiftamt = temp & mode;

    				//set cursor to starting letter
    				char *cursor = Alphabet + startlet(c, Alphabet);

    				//add to cursor until shiftamt is reached. wrap around if end of string
    				for(char i = 0; i < shiftamt; i++){

    					cursor++;
    					if( *cursor == 0 ){
    						cursor = Alphabet;

    					}
    				}

    				c = *cursor;
    				putc( c, out );
    			}
    			else{
    				//apparently i didnt read the const.c close enough. oh wells.
    				if(c != EOF){
    					putc( c, out );
    				}
    			}

    		}
    	}

    	shiftamt = 0x1F & mode;


    	printinfostr("shifted alphabet", shiftstr(shiftamt, Alphabet));
    	printinfonum("shift amount", shiftamt);

    	if( streq((*(argv + 3)), "-") == 1){
    		printinfostr("input file", "STDIN");
    	}
    	else{
    		printinfostr("input file", *(argv + 3));
    	}

    	if( streq((*(argv + 4)), "-") == 1){
    		printinfostr("output file", "STDOUT");
    	}
    	else{
    		printinfostr("output file", (*(argv + 4)));
    	}

    	if( (mode & 0x20) == 0x20 ){
    		printinfostr("operation", "decryption" );
    	}
    	else{
    		printinfostr("operation", "encryption" );
    	}
    }
    else{
    	//Tutnese. KIll. ME. NOW.
    	//now we check if encryption or decryption was selected
    	temp = 0x20;		//6th bit represents d
    	if( (temp & mode) == 0x20 ){
    		//decryption
    		char* ptr1 = buffer;
    		//char* ptr2 = buffer + 1;
    		char prevChar = 0;	//store the previous char that was printed. this variable is meaningless without sumc.
    		char sumc = 0;		//single ummatched char was the last operatoion if this is 1

    		while( !feof(in) ){
    			*ptr1 = getc(in);

    			//check for failure
    			if( sumc == 1 && samelet(*ptr1, prevChar) ){
    				//fail
    				return EXIT_FAILURE;
    			}

    			//so we get a character
    			//check if letter or non-letter
    			char isLet = islet(*ptr1);
    			if( isLet == 1 ){
    				//it is a letter
    				//now check if vowel or consonant
    				char isVowel = isvowel(*ptr1);
    				if( isVowel == 1 ){
    					//it is a vowel
    					//print it as is
    					putc(*ptr1, out);

    				}
    				else{
    					//if the first char is a consonant, this gets complicated
    					//Sus tut o nun yuck Bub rug squato kuck
    					//if the letter is an s, then we have to see if it spells out squa or squat
    					//a consonant must follow squa, a vowel must follow squat
    					//we also have to check if it maps to something in tut array and spells it out
    					//if its any other consonant we have to see if it maps to something in the tut array
    					//once its confirmed the consonant maps to something, we have to spell it out and see if it
    					//matches the syllable entirely
    					//not matching the syllable results in failure
    					//what is the best way to sequentially check all these conditions? im dying.
    					//we will need to use the buffer.

    					//check if its an s
    					char isS = samelet( 's', *ptr1);
    					if( isS == 1 ){
    						//the letter is an S
    						//shit its an S
    						//what should i compare first?
    						//to Squa and Squat or the syllable in the tut array?
    						//maybe we should do this in stages
    						//shortest ones first
    						//Squa  == 4
    						//Squat == 5
    						//mapping == ?

    						char* tutString = intut(*ptr1, Tutnese);

    						if( tutString != 0 ){
    							//there is a tutString that exists for the letter s
    							//lets say swertop or saw

    							//there exists a mapping that we have to match
    							//requires rest of buffer array sigh
    							//Kuck
    							//worry about lowercase and uppercase when printing
    							//check length of tutstring?
    							char length = strlength(tutString);
    							char wasSuccess = 0;

    							if( length < 4){
    								char i;
    								for(i = 1; i < length; i++){
    									*(ptr1 + i) = getc(in);
    								}
    								//null terminator at the end to denote end of string
    								*(ptr1 + i) = 0;

    								//now we have to compare ptr1 and tutString
    								//hmm wait what about lower case and upper case when comparing
    								//what if we just compare everything past the first letter
    								//since we already know that the first letters are equal
    								if( streq( (ptr1 + 1), (tutString + 1) ) == 1 ){
    									//if the strings are equal, then we print first letter of ptr1
    									putc( *ptr1 , out);
    									wasSuccess = 1;
    								}

    							}
    							else if(length == 4){
    								char i;
    								for(i = 1; i < length; i++){
    									*(ptr1 + i) = getc(in);
    								}
    								//null terminator at the end to denote end of string
    								*(ptr1 + i) = 0;

    								//now we have to compare ptr1 and tutString
    								//hmm wait what about lower case and upper case when comparing
    								//what if we just compare everything past the first letter
    								//since we already know that the first letters are equal
    								if( streq( (ptr1 + 1), (tutString + 1) ) == 1 ){
    									//if the strings are equal, then we print first letter of ptr1
    									putc( *ptr1 , out);
    									wasSuccess = 1;
    								}
    								else if( streq( "squa", ptr1 ) == 1 || streq( "Squa", ptr1 ) == 1){

    								}

    							}
    							else if(length == 5){

    							}
    							else{
    								//tutstring is longest
    							}

    							if(wasSuccess == 0){
    								return EXIT_FAILURE;
    							}
    						}
    						else{
    							//there is no tutString, so we only have to check squa and squat
    							//check if it is squa first
    							//so need to get the next 3 chars into the buffer and then null terminate it
    							char i;
    							for(i = 1; i < 4; i++){
    								*(ptr1 + i) = getc(in);
    							}
    							//null terminator at the end to denote end of string
    							*(ptr1 + i) = 0;

    							//now we compare ptr1 to "squa"
    							if( streq( "squa", ptr1 ) == 1 || streq( "Squa", ptr1 ) == 1 ){
    								//hmm wait.
    								//we also have to consider if next letter is t
    								//squat can be squa t or squat vowel
    								//squaT is squa T
    								//
    								char temp = getc(in);
    								if( temp == 'T'  ){
    									//definitely two t's where last t is caps
    									//check if the s is lower or upper in ptr1
    									char isLow = islowerc(*ptr1);
    									if( isLow == 1 ){
    										//s
    										putc('t', out);
    									}
    									else{
    										//S
    										putc('T', out);
    									}
    									putc('T', out);

    								}
    								else if( temp == 't' ){
    									//ambiguity here, just gonna make it so next one must be vowel
    									*(ptr1 + i) = 't';
    									*(ptr1 + i + 1) = getc(in);
    									*(ptr1 + i + 2) = 0;
    									//ptr1 will be "squat" + a vowel;
    									//if not a vowel, fail
    									char isVowel = isvowel(*(ptr1 + i + 1));
    									if( isVowel == 1){
    										//then we gotta put double vowels.
    										//first vowel we have to check *ptr1
    										char isLow = islowerc(*ptr1);
    										if( isLow == 1 ){
    											//put lowercase version of the vowel
    											putc( tolowerc(*(ptr1 + i + 1)), out );
    										}
    										else{
    											//put uppercase version of the vowel
    											putc( toupperc(*(ptr1 + i + 1)), out );
    										}

    										//second vowel just put it as is
    										putc(*(ptr1 + i + 1), out);
    									}
    									else{
    										//not vowel, fail
    										return EXIT_FAILURE;
    									}

    								}
    								else{
    									//if its not a t then its a double consonant
    									//check if the s is upper or lower
    									//Squa bub
    									//check mapping of that consonant
    									//save information about the case of s in a variable
    									char isLow = islowerc(*ptr1);
    									//we will now clear buffer array and begin checking for mapping
    									//put temp at 0 index
    									*ptr1 = temp;

    									//first we check if the consonant maps to something in the tut array
    									char* tutString = intut(*ptr1, Tutnese);

    									if( tutString != 0 ){
    										//there exists a mapping that we have to match
    										//requires rest of buffer array sigh
    										//Kuck
    										//worry about lowercase and uppercase when printing
    										//check length of tutstring?
    										char length = strlength(tutString);

    										//now we know how long the tutString is
    										//we will add chars to the buffer in a loop until the length is reached
    										//buffer index 0 already has the first letter of the sequence
    										//we will put a null terminator at the end
    										char i;
    										for(i = 1; i < length; i++){
    											*(ptr1 + i) = getc(in);
    										}
    										//null terminator at the end to denote end of string
    										*(ptr1 + i) = 0;

    										//now we have to compare ptr1 and tutString
    										//hmm wait what about lower case and upper case when comparing
    										//what if we just compare everything past the first letter
    										//since we already know that the first letters are equal
    										if( streq( (ptr1 + 1), (tutString + 1) ) == 1 ){
    											//if the strings are equal, then we will print 2 things
    											//the lower or uppercase version of the consonant
    											//the consonant as is
    											if( isLow == 1 ){
    												//put lowercase version of the con
    												putc( tolowerc(*ptr1), out );
    											}
    											else{
    												//put uppercase version of the con
    												putc( toupperc(*ptr1), out );
    											}
    											//temp will be printed as is
    											putc(*ptr1, out);
    										}
    										else{
    											//if it doesnt match it must fail
    											return EXIT_FAILURE;
    										}

    									}

    								}
    							}

    						}

    					}
    					else{
    						//the letter is not an S
    						//i should do this part first

    						//first we check if the consonant maps to something in the tut array
    						char* tutString = intut(*ptr1, Tutnese);

    						//tutString will be 0 if it didnt exist
    						//else tutString will be the string we have to match

    						if( tutString != 0 ){
    							//there exists a mapping that we have to match
    							//requires rest of buffer array sigh
    							//Kuck
    							//worry about lowercase and uppercase when printing
    							//check length of tutstring?
    							char length = strlength(tutString);

    							//now we know how long the tutString is
    							//we will add chars to the buffer in a loop until the length is reached
    							//buffer index 0 already has the first letter of the sequence
    							//we will put a null terminator at the end
    							char i;
    							for(i = 1; i < length; i++){
    								*(ptr1 + i) = getc(in);
    							}
    							//null terminator at the end to denote end of string
    							*(ptr1 + i) = 0;

    							//now we have to compare ptr1 and tutString
    							//hmm wait what about lower case and upper case when comparing
    							//what if we just compare everything past the first letter
    							//since we already know that the first letters are equal
    							if( streq( (ptr1 + 1), (tutString + 1) ) == 1 ){
    								//if the strings are equal, then we print first letter of ptr1
    								putc( *ptr1 , out);
    							}
    							else{
    								//if it doesnt match it must fail
    								return EXIT_FAILURE;
    							}

    						}
    						else{
    							//then we just print this consonant as is
    							//but what if next char is the same letter. Thats not allowed and it should fail. how to handle this?
    							//make more variables before the while
    							prevChar = *ptr1;
    							sumc = 1;
    							putc(*ptr1, out);
    						}

    					}
    				}
    			}
    			else{
    				//it is a special character
    				//print it as is
    				putc(*ptr1, out);
    			}
    		}
    		char newlin = '\n';
    		putc(newlin, out);
    	}
    	else{
    		//encryption
    		//put 2 chars in the buffer
    		char* ptr1 = buffer;
    		char* ptr2 = buffer + 1;

    		*ptr1 = getc(in);
    		*ptr2 = getc(in);

    		while( !feof(in) ){


    			//use pointers to compare consecutive chars to check for doubles
    			ptr1 = buffer;
    			ptr2 = buffer + 1;

    			//should i check if theyre letters first or doubles first?
    			//if theyre doubles, then they can either be double letters or double  nonletters
    			//if theyre double letters they can either be double vowels or double consonants

    			//check if ptrs are doubles
    			char isDouble = isdouble(*ptr1, *ptr2);
    			if(isDouble == 1){
    				//now check if double letters or double nonletters
    				char isLet = islet(*ptr1);
    				if( isLet == 1 ){
    					//now check if double vowels or double consonants
    					char isVowel = isvowel(*ptr1);
    					if( isVowel == 1 ){
    						//handle double vowels
    						//check if ptr1 and ptr2 are caps
    						char islow1 = islowerc(*ptr1);

    						//if it is low, first letter should be low
    						//put squat and the second letter  in buffer 2, 3, 4, 5, 6, 7 and a null terminator at 8

    						if( islow1 == 1 ){
    							//then lower case s for first part
    							*(buffer + 2) = 0x73;	//s
    						}
    						else{
    							//then upper case s
    							*(buffer + 2) = 0x53;	//S
    						}

    						*(buffer + 3) = 0x71;	//q
    						*(buffer + 4) = 0x75;	//u
    						*(buffer + 5) = 0x61;	//a
    						*(buffer + 6) = 0x74;	//t
    						*(buffer + 7) = *ptr2;	//vowel

    						putc( *(buffer + 2), out );
    						putc( *(buffer + 3), out );
    						putc( *(buffer + 4), out );
    						putc( *(buffer + 5), out );
    						putc( *(buffer + 6), out );
    						putc( *(buffer + 7), out );

    						*buffer = getc(in);
    						*(buffer + 1) = getc(in);
    					}
    					else{
    						//handle double consonants
    						//check if ptr1 and ptr2 are caps
    						char islow1 = islowerc(*ptr1);
    						char islow2 = islowerc(*ptr2);

    						if( islow1 == 1 ){
    							//then lower case s for first part
    							*(buffer + 2) = 0x73;	//s
    						}
    						else{
    							//then upper case s
    							*(buffer + 2) = 0x53;	//S
    						}
    						putc( *(buffer + 2), out );

    						*(buffer + 2) = 0x71;		//q
    						putc( *(buffer + 2), out );

    						*(buffer + 2) = 0x75;		//u
    						putc( *(buffer + 2), out );

    						*(buffer + 2) = 0x61;		//a
    						putc( *(buffer + 2), out );

    						//check if in tutnese
    						char* tutString = intut(*ptr2, Tutnese);
    						if( islow2 == 0 ){
    							char temp = *tutString - 0x20;
    							putc(temp, out);
    							tutString++;
    						}

    						//output the tut string
    						while( *tutString != 0 ){
    							putc( *tutString, out );
    							tutString++;
    						}
    						*buffer = getc(in);
    						*(buffer + 1) = getc(in);
    					}

    				}
    				else{
    					//handle double nonletters
    					putc(*ptr1, out);
    					putc(*ptr2, out);

    					*buffer = getc(in);
    					*(buffer + 1) = getc(in);
    				}
    			}
    			else{
    				//not doubles, means singles, focus on first char
    				//check letters or non letters
    				char isLetter = islet(*ptr1);
    				if( isLetter == 1){
    					//check if its vowel or consonant now
    					char isVowel = isvowel(*ptr1);
    					if( isVowel == 1 ){
    						//handle vowel
    						putc(*ptr1, out);
    					}
    					else{
    						//handle consonant
    						char* tutString = intut(*ptr1, Tutnese);
    						char islow1 = islowerc(*ptr1);
    						if( islow1 == 0 ){
    							char temp = *tutString - 0x20;
    							putc(temp, out);
    							tutString++;
    						}
    						//output the tut string
    						while( *tutString != 0 ){
    							putc( *tutString, out );
    							tutString++;
    						}
    					}

    				}
    				else{
    					//just print it
    					putc(*ptr1, out);

    				}
				*ptr1 = *ptr2;
    			*ptr2 = getc(in);
    			}
    		}
    			if( *ptr2 == EOF && *ptr1 != EOF){
    				putc( *ptr1, out);
    			}
    		char newlin = '\n';
    		putc(newlin, out);
    	}
    }

    fclose(in);
    fclose(out);

    return EXIT_SUCCESS;
}
