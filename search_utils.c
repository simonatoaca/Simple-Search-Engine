#include "search_utils.h"
#include "search_engine.h"

char **get_doc_count_and_names(char *path, int *doc_count)
{
	char **doc_names = NULL;
	DIR *directory = NULL;
	struct dirent *entry;

	directory = opendir(path);

	if (directory)
		while ((entry = readdir(directory)) != NULL)
			if (entry->d_type == DT_REG) {
				doc_names = realloc(doc_names, (++(*doc_count)) * sizeof(char *));
				doc_names[*doc_count - 1] = strdup(entry->d_name);
			}

	closedir(directory);
	return doc_names;
}

int search_word(char *string, char *word)
{
	if (strstr(string, word))
		return 1;
	return 0;
}

int *search_in_docs(char *word, char *path, int doc_count, char **doc_names)
{
	int *index = calloc(doc_count, sizeof(int));
	char file_path[BUF_SIZE];
	char *buffer = malloc(BUF_SIZE * sizeof(char)); 

	// Searches for <word> in every file
	for (int i = 0; i < doc_count; i++) {
		sprintf(file_path, "%s/%s", path, doc_names[i]);

		FILE *in = fopen(file_path, "r");

		if (!in) {
			printf("[ERROR] No such file: %s\n", file_path);
			return NULL;
		}

		while (fgets(buffer, BUF_SIZE, in)) {
			// If the word is found break
			if ((index[i] = search_word(buffer, word)))
				break;
		}

		fclose(in);
	}

	free(buffer);
	return index;
}

char **make_postfix(char *query, int *wd_count)
{
	char **postfix_query = malloc(MAX_WORDS * sizeof(char *));

	stack_tt *st = st_create(WORD_LEN * sizeof(char));

	char *aux_query = strdup(query);
	char *word = strtok(aux_query, " ");
	int count = 0;

	// Transforms the query into a postfix expression
	while (word) {
		if (word[0] == '!') {
			st_push(st, "!");
			word++;
			continue;
		} else if (word[0] == '(') {
			if (word[strlen(word) - 1] ==  ')') {
				char *aux_wd = strdup(word + 1);
				aux_wd[strlen(aux_wd) - 1] = '\0';
				postfix_query[count++] = strdup(aux_wd);
				free(aux_wd);
			} else {
				st_push(st, "(");
				postfix_query[count++] = strdup(word + 1);
			}
		} else if (word[strlen(word) - 1] == ')') {
			char *aux_wd = strdup(word);
			aux_wd[strlen(word) - 1] = '\0';
			postfix_query[count++] = strdup(aux_wd);
			free(aux_wd);
	
			while (!st_is_empty(st) && strcmp((char *)st_peek(st), "(")) {
				postfix_query[count++] = strdup((char *)st_peek(st));
				st_pop(st);
			}
			// Pop '('
			st_pop(st);
		} else if (!strcmp(word, "!") || !strcmp(word, "||") || !strcmp(word, "&&")) {
			while (!st_is_empty(st) && strcmp((char *)st_peek(st), "(")) {
				postfix_query[count++] = strdup((char *)st_peek(st));
				st_pop(st);
			}

			st_push(st, word);
		} else {
			postfix_query[count++] = strdup(word);
		}

		word = strtok(NULL, " ");
	}

	// Extract remaining elements
	while (!st_is_empty(st)) {
		postfix_query[count++] = strdup((char *)st_peek(st));
		st_pop(st);
	}

	st_free(st);
	free(aux_query);
	*wd_count = count;
	return postfix_query;
}

void perform_search(char *word, char *path, int doc_count,
					char **doc_names, hashtable_t *ht)
{
	if (strcmp(word, "||") || strcmp(word, "&&") || strcmp(word, "!")) {
		// Add/update the word's inverted index in the hashtable
		inverted_index inv_idx = {search_in_docs(word, path, doc_count, doc_names)};

		ht_put(ht, word, strlen(word) + 1, &inv_idx, sizeof(inv_idx));
	}
}

int *operation_on_vector(int *v1, int *v2, int size, char *operation)
{
	int *result = calloc(size, sizeof(int));
	for (int i = 0; i < size; i++) {
		if (!strcmp(operation, "&&")) {
			result[i] = v1[i] && v2[i];
		} else if (!strcmp(operation, "||")) {
			result[i] = v1[i] || v2[i];
		} else if (!strcmp(operation, "!")) {
			result[i] = !v1[i];
		}
	}

	return result;
}

int *evaluate_expression(char **query, hashtable_t *ht, int wd_count, int doc_count)
{
	stack_tt *st = st_create(sizeof(int *));

	for (int i = 0; i < wd_count; i++) {
		if (strcmp(query[i], "&&") && strcmp(query[i], "||") && strcmp(query[i], "!")) {
			inverted_index *inv_idx = (inverted_index *)ht_get(ht, query[i]);
			int *index = malloc(doc_count * sizeof(int));
			memcpy(index, inv_idx->index, doc_count * sizeof(int));

			st_push(st, &index);
		} else if (!strcmp(query[i], "&&") || !strcmp(query[i], "||")) {
			int *v1 = *((int **)st_peek(st));
			st_pop(st);

			int *v2 = *((int **)st_peek(st));
			st_pop(st);

			int *result = operation_on_vector(v1, v2, doc_count, query[i]);
			st_push(st, &result);

			free(v1);
			free(v2);
		} else {
			int *v1 = *((int **)st_peek(st));
			st_pop(st);
			int *result = operation_on_vector(v1, v1, doc_count, query[i]);
			st_push(st, &result);
			free(v1);
		}
	}

	int *result = *((int **)st_peek(st));
	st_pop(st);
	st_free(st);

	return result;
}