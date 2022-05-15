#include "wc.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

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
		} else {
      nChildProc = 1;
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
		
    int pipes[nChildProc][2];
    pid_t pids[nChildProc], wpid;
    int status = 0;

    int size = fsize / (nChildProc == 0 ? 1 : nChildProc);

    for (int i = 0; i < nChildProc; i++) {
      pipe(pipes[i]);
      pids[i] = fork();
      if (pids[i] == 0) {
        int offset = i * size;
        if (i == nChildProc - 1) {
          size = fsize - offset;
        }

        fp = fopen(argv[1], "r");

        count_t wc;
        wc = word_count(fp, offset, size);

        write(pipes[i][1], &wc, sizeof(wc));
        exit(0);
      }
    }

    for (int i = 0; i < nChildProc; i++) {
      waitpid(pids[i], &status, 0);
      if (WIFSIGNALED(status)) {
        printf("%s", "\tError: child stopped because of an uncaught signal. Retrying...\n");
        bool isNotCrashing = true;
        while (isNotCrashing) {
          pipe(pipes[i]);
          pids[i] = fork();
          if (pids[i] == 0) {
            int offset = i * size;
            if (i == nChildProc - 1) {
              size = fsize - offset;
            }

            fp = fopen(argv[1], "r");

            count_t wc;
            wc = word_count(fp, offset, size);

            write(pipes[i][1], &wc, sizeof(wc));
            isNotCrashing = false;
            exit(0);
          } else {
            waitpid(pids[i], &status, 0);
            if (WIFSIGNALED(status)) {
              printf("%s", "\tError: child stopped because of an uncaught signal. Retrying...\n");
            } else {
              isNotCrashing = false;
            }
          }
        }
      }
    }

    for (int i = 0; i < nChildProc; i++) {
      count_t wc;
      read(pipes[i][0], &wc, sizeof(wc));
      count.linecount += wc.linecount;
      count.wordcount += wc.wordcount;
      count.charcount += wc.charcount;
    }

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

