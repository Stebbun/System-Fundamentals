#include "hw1.h"

// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	/* code here */
	//check if first argument is "bin/hw1"
	//first argument is (*argv) which points to an array of chars
	char *s = "-s";
	char *t = "-t";
	char *h = "-h";
	char *d = "-d";
	char *e = "-e";

	//check if argv[1] exists. if not, return 0.
	if(argc < 2){
		ret = 0;
		return ret;
	}

	//check if argv[1] is -h, -s, or -t
	if( streq( (*(argv + 1)), h ) == 1){
		//ignore all other arguments and return 0x80
		ret = 0x80;
		return ret;
	}
	else if( streq( (*(argv + 1)), s ) == 1){
		//argc can either be 5 or 6
		if( !(argc == 5 || argc == 6) ){
			ret = 0;
			return ret;
		}



		//check if argv[5] is n
		if( argc == 6 ){
			//get n % alphabet length to get value for lsb
			//turn n from string into number
			int num = atoui((*(argv + 5)));
			if(num == -1){
				//not a valid number so it fails
				ret = 0;
				return ret;
			}
			else{
				char val = (char)(num % strlength(Alphabet));
				ret = ret | val;
			}
		}

		if( argc == 5 ){
			int num = 320;
			char val = (char)(num % strlength(Alphabet));
			ret = ret | val;
		}

		//set the second msb to 1 since it is s.
		ret = ret | 0x40;

		//handle  argv[2]
		if( streq( (*(argv + 2)), d ) == 1 ){
			//set bit 5
			ret = ret | 0x20;


		}
		else if( streq( (*(argv + 2)), e ) == 1 ){
			//leave bit 5 as 0

		}
		else{
			ret = 0;
			return ret;
		}

	}
	else if( streq( (*(argv + 1)), t ) == 1){
		//argc must be 5
		if(argc != 5){
			ret = 0;
			return ret;
		}

		//handle  argv[2]
		if( streq( (*(argv + 2)), d ) == 1 ){
			//set bit 5
			ret = ret | 0x20;
		}
		else if( streq( (*(argv + 2)), e ) == 1 ){
			//leave bit 5 as 0

		}
		else{
			ret = 0;
			return ret;
		}

		ret = ret | (320 % strlength(Alphabet));
	}
	else{
		ret = 0;
		return ret;
	}

	//after these if statements, handle file stuff
	//handle infile
	if( streq( (*(argv + 3)) , "-" ) ){
		*in = stdin;
	}
	else{
		*in = fopen( (*(argv + 3)), "r");
		if(*in == NULL){
			return 0;
		}
	}

	//handle outfile
	if( streq( (*(argv + 4)) , "-" ) ){
		*out = stdout;
	}
	else{
		*out = fopen( (*(argv + 4)), "w");
		if(*out == NULL){
			return 0;
		}
	}

	return ret;
}

int streq(char *str1, char *str2){
	int i = 0;
	while( (*(str1 + i)) != 0 || (*(str2 + i)) != 0){
		if( (*(str1 + i)) != (*(str2 + i)) ){
			return 0;
		}
		i++;
	}
	return 1;
}

int strlength(char *str){
	int i = 0;
	while( *(str + i) != 0 ){
		i++;
	}
	return i;
}

int atoui(char *str){
	int i = 0;
	int output = 0;

	while( *(str + i) != 0 ){
		if( (*(str + i) >= 0x30) &&  (*(str + i) <= 0x39) ){
			output = (output * 10) + (*(str + i) - 0x30);
			i++;
		}
		else{
			return -1;
		}
	}
	return output;
}

int iswhite(char c){
	if( c == 0x20 || c == 0x9 || c == 0xD || c == 0xA || c == 0xB || c == 0xC ){
		return 1;
	}

	return 0;
}

int isalphab(char c){
	//if uppercase letters
	if( c >= 0x41 && c <= 0x5A ){
		return 1;
	}
	//if lowercase letters
	if( c >= 0x61 && c <= 0x7A ){
		return 1;
	}
	//if ! @ # $
	if( c == 0x21 || c == 0x40 || c == 0x23 || c == 0x24){
		return 1;
	}

	return 0;
}

char startlet(char c, char *str){
	char i = 0;

	while( *(str + i) != 0 ){
		if( ( *(str + i)) == c || ( *(str + i)) == (c - 0x20)){
			return i;
		}
		i++;
	}

	return i;
}

char islowerc(char c){
	if(c >= 0x61 && c <= 0x7A){
		return 1;
	}
	return 0;
}

char isvowel(char c){
	//a e i u o A E I U O
	if( c == 0x41 || c == 0x45 || c == 0x49 || c == 0x55 || c == 0x4F || c == 0x61 || c == 0x65 || c == 0x69 || c == 0x75 || c == 0x6F){
		return 1;
	}
	return 0;
}

char islet(char c){
	if( ( (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A) ) ){
		return 1;
	}

	return 0;
}

char isdouble(char c1, char c2){
	//EE, Ee, eE, ee
	if(c1 == c2){
		return 1;
	}
	else if( (c1 - 0x20) == c2){
		return 1;
	}
	else if((c2 - 0x20) == c1){
		return 1;
	}

	return 0;
}

char* intut(char c, char** tut){
	while( *tut != NULL ){
		if(*tut == NULL){
			return 0;
		}

		if( (**tut) == c || (**tut) == (c + 0x20)){
			return *tut;
		}
		tut++;
	}

	return 0;
}

char* shiftstr(char shiftamt, char* str){
	//char length = strlength(str);

	char* str2 = "ABCDEFGHIJ!K#LMN$OP@QRSTUVWXYZ";

	//i need to set the value at str2 to

	return str2;
}

char samelet(char c1, char c2){
	if( c1 == c2 ){
		return 1;
	}
	else if((c1 - 0x20) == c2){
		return 1;
	}
	else if( (c2 - 0x20) == c1 ){
		return 1;
	}

	return 0;
}

char toupperc(char c){
	//check if its upper already
	if( c >= 0x41 && c <= 0x5A ){
		return c;
	}
	else{
		return (c - 0x20);
	}
}

char tolowerc(char c){
	//check if its lower already
	if( c >= 0x61 && c <= 0x7A ){
		return c;
	}
	else{
		return (c + 0x20);
	}
}
