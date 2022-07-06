#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "stack.h"
#define MAX_STRING_SIZE	256
#define MIN(x, y) ((x) < (y) ? (x) : (y))

stack_tt *st_create(unsigned int data_size)
{
	stack_tt *st = malloc(sizeof(*st));
	DIE(!st, "Failed stack allocation\n");
	st->list = ll_create(data_size);
	if (!st->list) {
		fprintf(stderr, "Failed stack allocation\n");
		free(st);
		return NULL;
	}

	return st;
}

/*
 * Functia intoarce numarul de elemente din stiva al carei pointer este trimis
 * ca parametru.
 */
unsigned int st_get_size(stack_tt *st)
{
	DIE(!st, "The stack does not exist.\n");
	if (!st->list) {
		return 0;
	}

	unsigned int st_len = 0;
	ll_node_t *curr_node = st->list->head;
	while (curr_node) {
		st_len++;
		curr_node = curr_node->next;
	}

	return st_len;
}

/*
 * Functia intoarce 1 daca stiva al carei pointer este trimis
 * ca parametru este goala si 0 in caz contrar.
 */
unsigned int st_is_empty(stack_tt *st)
{
	DIE(!st, "The stack does not exist.\n");
	if (!st_get_size(st))
		return 1;
	return 0;
}

/*
 * Functia intoarce elementul din vârful stivei al carei pointer este trimis
 * ca parametru fără a efectua nicio modificare asupra acesteia.
 */
void *st_peek(stack_tt *st)
{
	DIE(!st, "The stack does not exist.\n");
	if (!st_get_size(st)) {
		return NULL;
	}

	ll_node_t *curr_node = st->list->head;
	return curr_node->data;
}

/*
 * Functia elimina elementul din vârful stivei al carei pointer este trimis
 * ca parametru. Atentie! Este responsabilitatea acestei functii de a elibera
 * memoria pentru elementul eliminat.
 */
void st_pop(stack_tt *st)
{
	DIE(!st, "The stack does not exist.\n");
	if (st_get_size(st)) {
		ll_node_t *curr_node = ll_remove_nth_node(st->list, 0);
		free(curr_node->data);
		free(curr_node);
	}
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se adauga un nou element in
 * vârful stivei al carei pointer este trimis ca parametru.
 */
void st_push(stack_tt *st, void *new_data)
{
	DIE(!st, "The stack does not exist\n");
	if (!new_data) {
		fprintf(stderr, "The data is NULL\n");
		return;
	}
	ll_add_nth_node(st->list, 0, new_data);
}

/*
 * Functia elimina toate elementele stivei al carei pointer este trimis
 * ca parametru.
 */
void st_clear(stack_tt *st)
{
	DIE(!st, "The stack does not exist\n");

	unsigned int st_len = st_get_size(st);
	for (unsigned int i = 0; i < st_len; i++) {
		st_pop(st);
	}
}

/*
 * Functia elibereaza memoria folosita de toate elementele, iar la
 * sfarsit elibereaza memoria folosita de structura stivei.
 */
void st_free(stack_tt *st)
{
	DIE(!st, "The stack does not exist\n");
	st_clear(st);
	ll_free(&st->list);
	free(st);
}