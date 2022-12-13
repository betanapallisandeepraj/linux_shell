#include <stdio.h>
#include <string.h>

int main()
{
	char *string,*found;

	string = strdup("Hello there, peasants!");
	printf("Original string: '%s'\n",string);

	while( (found = strsep(&string," ")) != NULL ) {
		printf("found=%s\n",found);
		printf("string=%s\n",string);
	}
	return(0);
}
