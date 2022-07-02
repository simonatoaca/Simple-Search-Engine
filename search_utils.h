#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Stack.h"
#include "Hashtable.h"
#define BUF_SIZE 5000

unsigned int get_doc_count(char *path);
int search_word(char *string, char *word);
int *search_in_docs(char *word, char *path, int doc_count);
char **make_postfix(char *query, int *wd_count);
void perform_search(char *word, char *path, int doc_count, hashtable_t *ht);
int *evaluate_expression(char **query, hashtable_t *ht, int wd_count, int doc_count);