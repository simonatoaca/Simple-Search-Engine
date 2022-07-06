#ifndef GTK_FUNCTIONS_H_
#define GTK_FUNCTIONS_H_

#include "search_utils.h"
#include <gtk/gtk.h>

typedef struct {
	GtkWidget *search_entry, *file_chooser, *results;
	hashtable_t *query_words;
} Data;

void search(GtkWidget *btn, Data *data);
void set_up_app(GtkApplication *app, Data *data);

#endif