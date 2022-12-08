#include "uartfunction.h"
#include <stdio.h>
#include <string.h>


int isPresent( uint8_t *line,  uint8_t *word)//Define a String searching function
{
	int i, j, status, n, m;
	n = strlen(line);
	m = strlen(word);
	for (i = 0; i <= n - m; i++)
        {       
		if (strncmp(line+i, word, m)==0) return 1;
                }
	return 0;

}



