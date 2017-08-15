#include <criterion/criterion.h>
#include "sfish.h"

Test(parsesuite, Test_num_args){
	char* str = "hi there";	//2 args
	char* str2 = " hi";		//1 arg
	char* str3 = " hi ";	//1 arg
	char* str4 = " notice me jwong ";	//3 arg
	char* str5 = "\"hi there buddy i like butts\""; //1 arg


	cr_assert(checkNumArgs(str) == 2, "str 1 failed");
	cr_assert(checkNumArgs(str2) == 1, "str 2 failed");
	cr_assert(checkNumArgs(str3) == 1, "str 3 failed");
	cr_assert(checkNumArgs(str4) == 3, "str 4 failed");
	cr_assert(checkNumArgs(str5) == 1, "str 5 failed");
}

Test(parsesuite, Test_max_arg_len){
	char* str = "hi there";	//5
	char* str2 = " hie";		//3
	char* str3 = " hi ";	//2
	char* str4 = " notice me jwong ";	//6

	cr_assert(getMaxTokLen(str) == 5, "str 1 failed");
	cr_assert(getMaxTokLen(str2) == 3, "str 2 failed");
	cr_assert(getMaxTokLen(str3) == 2, "str 3 failed");
	cr_assert(getMaxTokLen(str4) == 6, "str 4 failed");
}
