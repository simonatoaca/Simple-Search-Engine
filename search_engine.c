#include "search_engine.h"

int main(void)
{	
	char query[QUERY_LEN];
	int *eval_expr = NULL;

	char *path = strdup("docs");
	unsigned int doc_count = get_doc_count(path);

	if (!doc_count) {
		printf("No documents to search through\n");
		return -1;
	}

	hashtable_t *query_words = ht_create(HMAX, hash_function_string,
										 compare_function_strings);

	while (fgets(query, QUERY_LEN, stdin)) {
		query[strlen(query) - 1] = '\0';
		printf("Searching for '%s' in docs...\n\n", query);
		
		int wd_count = 0;
		char **postfix_query = make_postfix(query, &wd_count);

		// For every term in the query
		for (int i = 0; i < wd_count; i++)
			perform_search(postfix_query[i], path, doc_count, query_words);

		eval_expr = evaluate_expression(postfix_query, query_words,
										wd_count, doc_count);


		printf("Found in:\n");
		for (unsigned int i = 0; i < doc_count; i++) {
			if (eval_expr[i]) {
				printf(i < 9 ? "doc0%d.txt\n" : "doc%d.txt\n", i + 1);
			}
		}

		printf("\n");

		// Free
		free(eval_expr);
		for (int i = 0; i < wd_count; i++)
			free(postfix_query[i]);
		free(postfix_query);
	}

	ht_free(query_words);
	free(path);
	return 0;
}