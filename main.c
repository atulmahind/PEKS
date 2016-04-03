/*
 * main.c
 *
 *  Created on: Jul 7, 2015
 *      Author: mahind
 */

#include <stdio.h>

#include "peks.h"

//#define DEBUG 1

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: %s <word1> <word2>\n",argv[0]);
        return 1;
    }

	char *W1, *W2;
	W1 = *(++argv);
	W2 = *(++argv);

	int match;
	match = peks_scheme(W1, W2);

	if(match)
		printf("Equal\n");
	else 
		printf("Not equal\n");

	return 0;
}
