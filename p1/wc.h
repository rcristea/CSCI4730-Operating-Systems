#ifndef WC_H
#define WC_H

#include <stdio.h>

typedef struct count_t {
		int linecount;
		int wordcount;
		int charcount;
} count_t;

count_t word_count(FILE* fp, long offset, long size);
extern int crashRate;

#endif
