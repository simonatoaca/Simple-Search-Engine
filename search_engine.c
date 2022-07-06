#include "search_engine.h"
#include "gtk_functions.h"


int main(int argc, char *argv[])
{
	Data *data = malloc(sizeof(Data));

	GtkApplication *app = gtk_application_new("org.gnome.SearchEngine",
											  G_APPLICATION_FLAGS_NONE);

	g_signal_connect(app, "activate", G_CALLBACK(set_up_app), data);

	g_application_run(G_APPLICATION(app), argc, argv);

	ht_free(data->query_words);
	free(data);

	return 0;
}