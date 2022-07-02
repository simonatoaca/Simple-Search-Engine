#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "LinkedList.h"
#include "search_engine.h"
#include "Hashtable.h"


/*
 * Functii de comparare a cheilor:
 */
int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *ht = malloc(sizeof(hashtable_t));
	DIE(!ht, "Failed hashtable allocation\n");
	ht->buckets = malloc(hmax * sizeof(linked_list_t *));
	ht->hmax = hmax;

	if (!ht->buckets) {
		free(ht);
		fprintf(stderr, "Failed ht->buckets allocation\n");
	}

	for (unsigned int i = 0; i < ht->hmax; i++) {
		ht->buckets[i] = ll_create(sizeof(info));
		if (!ht->buckets[i]) {
			for (unsigned int j = 0; j < i; j++)
				free(ht->buckets[j]);
			free(ht->buckets);
			free(ht);
		}
	}

	ht->size = 0;
	ht->compare_function = compare_function;
	ht->hash_function = hash_function;

	return ht;
}

void ht_resize(hashtable_t *ht)
{
	// Doubling the bucket array size
	linked_list_t **aux = realloc(ht->buckets, 2 * ht->hmax * sizeof(linked_list_t *));
	DIE(!aux, "Failed memory allocation\n");

	ht->buckets = aux;
	ht->hmax *= 2;

	for (unsigned int i = ht->hmax / 2; i < ht->hmax; i++) {
		ht->buckets[i] = ll_create(sizeof(info));
	}

	for (unsigned int bucket = 0; bucket < ht->hmax / 2; bucket++) {
		ll_node_t *curr_elem = ht->buckets[bucket]->head;
		int elem_pos = 0;
		while (curr_elem) {
			unsigned int index = ht->hash_function(((info *)curr_elem->data)->key) % ht->hmax;
			curr_elem = curr_elem->next;
			if (index != bucket) {
				ll_node_t *node = ll_remove_nth_node(ht->buckets[bucket], elem_pos);
				info *relocated_info = node->data;
				ll_add_nth_node(ht->buckets[index], 0, relocated_info);
				free(node->data);
				free(node);
				continue;
			}
			elem_pos++;
		}
	}
}

int ht_has_key(hashtable_t *ht, void *key)
{
	DIE(!ht, "The hashtable doesn't exist\n");
	
	if (!ht->size)
		return 0;

	int index = ht->hash_function(key) % ht->hmax;

	ll_node_t *curr = ht->buckets[index]->head;

	while (curr) {
		if (!ht->compare_function(((info *)curr->data)->key, key)) {
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	DIE(!ht, "The hashtable doesn't exist\n");
	DIE(!ht->size, "The hashtable is empty!\n");

	int index = ht->hash_function(key) % ht->hmax;

	ll_node_t *curr = ht->buckets[index]->head;

	while (curr) {
		if (!ht->compare_function(((info *)curr->data)->key, key)) {
			return ((info *)curr->data)->value;
		}
		curr = curr->next;
	}

	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	DIE(!ht, "The hashtable doesn't exist\n");

	double load_factor = (double) (ht->size + 1) / ht->hmax;
	if (load_factor > 1) {
		ht_resize(ht);
	}

	int index = ht->hash_function(key) % ht->hmax;

	ht->size++;
	if (!ht_has_key(ht, key)) {
		void *key_copy = key;
		info new_info;
		new_info.key = malloc(key_size);
		new_info.value = malloc(value_size);
		memcpy(new_info.key, key_copy, key_size);
		memcpy(new_info.value, value, value_size);
		ll_add_nth_node(ht->buckets[index], 0, &new_info);
		return;
	}

	ll_node_t *curr = ht->buckets[index]->head;
	
	while (curr) {
		if (!ht->compare_function(((info *)curr->data)->key, key)) {
			((info *)curr->data)->value = realloc(((info *)curr->data)->value, value_size);
			memcpy(((info *)curr->data)->value, value, value_size);
		}
		curr = curr->next;
	}
}


void ht_free_entry(info *entry) {
	free(entry->key);

	inverted_index *inv_idx = (inverted_index *)entry->value;
	free(inv_idx->index);
	free(inv_idx);
	free(entry);
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
	DIE(!ht, "The hashtable doesn't exist\n");
	DIE(!ht->size, "The hashtable is empty\n");

	if (!ht_has_key(ht, key))
		return;
	
	int index = ht->hash_function(key) % ht->hmax;

	ll_node_t *curr = ht->buckets[index]->head;
	int list_index = 0;
	while (curr) {
		if (!ht->compare_function(((info *)curr->data)->key, key)) {
			break;
		}
		curr = curr->next;
		list_index++;
	}

	curr = ll_remove_nth_node(ht->buckets[index], list_index);
	ht_free_entry(curr->data);
	free(curr);
}

void ht_free(hashtable_t *ht)
{	
    if (!ht)
		return;
	
	for (unsigned int i = 0; i < ht->hmax; i++) {
		int initial_bucket_size = ll_get_size(ht->buckets[i]);
		for (int j = 0; j < initial_bucket_size; j++) {
			ll_node_t *curr = ll_remove_nth_node(ht->buckets[i], 0);
			ht_free_entry(curr->data);
			free(curr);
		}
		free(ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
