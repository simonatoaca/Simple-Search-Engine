#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "search_utils.h"

#define QUERY_LEN 300
#define WORD_LEN 21
#define MAX_WORDS 21

typedef struct {
	int *index;
} inverted_index;