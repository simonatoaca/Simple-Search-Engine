#include "gtk_functions.h"

void search(GtkWidget *btn, Data *data)
{
	// Get buffer from search entry and convert it to text ->the query
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(data->search_entry));
	gchar *query = (gchar *)gtk_entry_buffer_get_text(buffer);

	// Get path to the folder to be searched in
	char *path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(data->file_chooser));
	
	// Get the number of docs and their names
	int doc_count = 0;
	char **doc_names = get_doc_count_and_names(path, &doc_count);

	// Transform the query into a postfix query for expression eval
	int wd_count = 0;
	char **postfix_query = make_postfix(query, &wd_count);

	// For every term in the query
	for (int i = 0; i < wd_count; i++)
		perform_search(postfix_query[i], path, doc_count,
					   doc_names, data->query_words);

	// Evaluate the logical expression using the inverted index
	int *eval_expr = evaluate_expression(postfix_query, data->query_words,
										 wd_count, doc_count);

	// Prepare text result
	char text_results[BUF_SIZE] = "";

	for (int i = 0; i < doc_count; i++) {
		if (eval_expr[i]) {
			strcat(text_results, doc_names[i]);
			strcat(text_results, "\n");
		}
	}

	// Transfer text to buffer, then set the buffer
	GtkTextBuffer *buff_results = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buff_results),
							 text_results, strlen(text_results));
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(data->results), buff_results);

	// Free
	free(eval_expr);
	free(path);
	for (int i = 0; i < wd_count; i++)
		free(postfix_query[i]);
	free(postfix_query);
}

void set_up_app(GtkApplication *app, Data *data)
{
	// Initialize main window
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 350);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

  	GtkWidget *fixed = gtk_fixed_new();
  	gtk_container_add(GTK_CONTAINER(window), fixed);

	// Search entry
  	GtkWidget *entry = gtk_search_entry_new();
	gtk_widget_set_size_request(entry, 300, 30);
  	gtk_fixed_put(GTK_FIXED(fixed), entry, 100, 150);

	// Search button
	GtkWidget *button = gtk_button_new_with_label("Search");
	gtk_fixed_put(GTK_FIXED(fixed), button, 100, 210);

	data->search_entry = entry;

	// The results are put inside a scrolled window
	GtkWidget *scrollable_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrollable_window, 200, 135);
	gtk_fixed_put(GTK_FIXED(fixed), scrollable_window, 500, 100);

	GtkWidget *results = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(results), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(results), FALSE);

	gtk_container_add(GTK_CONTAINER(scrollable_window), results);

	data->results = results;

	// File chooser -> to get path
	GtkWidget *file_chooser_btn = gtk_file_chooser_button_new("Select Folder",
									GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser_btn),
										"/home/student");
	gtk_widget_set_size_request(file_chooser_btn, 300, 30);
	gtk_fixed_put(GTK_FIXED(fixed), file_chooser_btn, 100, 100);

	data->file_chooser = file_chooser_btn;

	data->query_words = ht_create(HMAX, hash_function_string,
										 compare_function_strings);

	g_signal_connect(button, "clicked", G_CALLBACK(search), data);

	gtk_widget_show_all(window);
}