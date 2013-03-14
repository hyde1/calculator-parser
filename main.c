#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"

int main(int argc, char **argv){
	char *mathstring;
	int length = 100;
	if( argc <= 1 ){
		printf("Usage: %s <math string>\n", argv[0]);
		return 0;
	}
	for(int i=1; i<argc; ++i)
		length += strlen(argv[i]);
	++length;

	mathstring = calloc(1, length*sizeof(char));
	for(int i=1; i<argc; ++i){
		strcat(mathstring, argv[i]);
	}
	str_replace(mathstring, ',', '.');


	mathsobject *root = parse_string(mathstring);
	if( !root )
		printf("Unknown error\n");
	else {
		long double res = calculate_tree(root);
		if( res == (unsigned long long) res )
			printf("%llu\n", (unsigned long long)calculate_tree(root));
		else
			printf("%Lf\n", calculate_tree(root));
		release_tree(root);
	}

	free(mathstring);
	return EXIT_SUCCESS;
}
