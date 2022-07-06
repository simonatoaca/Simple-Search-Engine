#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "hashtable.h"
#define BUF_SIZE 2000

char **get_doc_count_and_names(char *path, int *doc_count);
int search_word(char *string, char *word);
int *search_in_docs(char *word, char *path, int doc_count, char **doc_names);
char **make_postfix(char *query, int *wd_count);
void perform_search(char *word, char *path, int doc_count,
					char **doc_names, hashtable_t *ht);
int *evaluate_expression(char **query, hashtable_t *ht, int wd_count, int doc_count);