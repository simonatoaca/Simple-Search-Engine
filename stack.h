#ifndef STACK_H_
#define STACK_H_

#include "linked_list.h"

typedef struct stack_t stack_tt;
struct stack_t
{
	struct linked_list_t *list;
};

stack_tt *
st_create(unsigned int data_size);

unsigned int
st_get_size(stack_tt *st);

unsigned int
st_is_empty(stack_tt *st);

void *
st_peek(stack_tt *st);

void
st_pop(stack_tt *st);

void
st_push(stack_tt *st, void *new_data);

void
st_clear(stack_tt *st);

void
st_free(stack_tt *st);

#endif  /* STACK_H_ */
