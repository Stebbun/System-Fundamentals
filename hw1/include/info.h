#ifndef INFO_H
#define INFO_H
#include <stdlib.h>
#include <stdio.h>

#ifdef INFO
	#define printinfostr(label, output ...) do{fprintf(stderr, "INFO: %s: %s\n", label, output );}while(0)
	#define printinfonum(label, output ...) do{fprintf(stderr, "INFO: %s: %d\n", label, output );}while(0)
#else
	#define printinfostr(label, output ...)
	#define printinfonum(label, output ...)
#endif

#endif /* INFO_H */
