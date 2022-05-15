#include "wc.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv)
{
		long fsize;
		FILE *fp;
		count_t count;
		struct timespec begin, end;
		int nChildProc = 0;
		
		
		/* 1st arg: filename */
		if(argc < 2) {
				printf("usage: wc <filname> [# processes] [crash rate]\n");
				return 0;
		}
		
		/* 2nd (optional) arg: number of child processes */
		if (argc > 2) {
				nChildProc = atoi(argv[2]);
				if(nChildProc < 1) nChildProc = 1;
				if(nChildProc > 10) nChildProc = 10;
		}

		/* 3rd (optional) arg: crash rate between 0% and 100%. Each child process has that much chance to crash*/
		if(argc > 3) {
				crashRate = atoi(argv[3]);
				if(crashRate < 0) crashRate = 0;
				if(crashRate > 50) crashRate = 50;
				printf("crashRate RATE: %d\n", crashRate);
		}
		
		printf("# of Child Processes: %d\n", nChildProc);
		printf("crashRate RATE: %d\n", crashRate);

		count.linecount = 0;
		count.wordcount = 0;
		count.charcount = 0;

  // start to measure time
		clock_gettime(CLOCK_REALTIME, &begin);

		// Open file in read-only mode
		fp = fopen(argv[1], "r");

		if(fp == NULL) {
				printf("File open error: %s\n", argv[1]);
				printf("usage: wc <filname>\n");
				return 0;
		}
		
		// get a file size
		fseek(fp, 0L, SEEK_END);
		fsize = ftell(fp);
		
		/* word_count() has 3 arguments.
			* 1st: file descriptor
			* 2nd: starting offset
			* 3rd: number of bytes to count from the offset
			*/
		count = word_count(fp, 0, fsize);

		fclose(fp);


		clock_gettime(CLOCK_REALTIME, &end);
		long seconds = end.tv_sec - begin.tv_sec;
		long nanoseconds = end.tv_nsec - begin.tv_nsec;
		double elapsed = seconds + nanoseconds*1e-9;

		printf("\n========= %s =========\n", argv[1]);
		printf("Total Lines : %d \n", count.linecount);
		printf("Total Words : %d \n", count.wordcount);
		printf("Total Characters : %d \n", count.charcount);
		printf("======== Took %.3f seconds ========\n", elapsed);

		return(0);
}

