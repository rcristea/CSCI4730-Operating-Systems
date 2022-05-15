/*************************************************
	* C program to count no of lines, words and 	 *
	* characters in a file.			 *
	*************************************************/

#include "wc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int crashRate = 0;
count_t word_count(FILE* fp, long offset, long size)
{
		char ch;
		long rbytes = 0;

		count_t count;
		// Initialize counter variables
		count.linecount = 0;
		count.wordcount = 0;
		count.charcount = 0;
		
		printf("[pid %d] reading %ld bytes from offset %ld\n", getpid(), size, offset);

		if(fseek(fp, offset, SEEK_SET) < 0) {
				printf("[pid %d] fseek error!\n", getpid());
		}

		while ((ch=getc(fp)) != EOF && rbytes < size) {
				// Increment character count if NOT new line or space
				if (ch != ' ' && ch != '\n') { ++count.charcount; }

				// Increment word count if new line or space character
				if (ch == ' ' || ch == '\n') { ++count.wordcount; }

				// Increment line count if new line character
				if (ch == '\n') { ++count.linecount; }
				rbytes++;
		}

		srand(getpid());
		if(crashRate > 0 && (rand()%100 < crashRate)) 
		{
				printf("[pid %d] crashed.\n", getpid());
				abort();
		}
    // printf("count: %d\n", count.linecount);
		return count;
}

