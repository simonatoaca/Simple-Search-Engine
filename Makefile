CC=gcc
CFLAGS=-Wall -Wextra
DEBUG=-g
GTK_CFLAGS=`pkg-config --cflags gtk+-3.0`
GTK_LIBS=`pkg-config --libs gtk+-3.0`


build: search

search: linked_list.o stack.o hashtable.o gtk_functions.o search_utils.o search_engine.o
	$(CC) $^ -o $@ $(GTK_LIBS)

search_engine.o: search_engine.c
	$(CC) $(CFLAGS) $(DEBUG) $(GTK_CFLAGS) $^ -c -o $@

gtk_functions.o: gtk_functions.c
	$(CC) $(CFLAGS) $(DEBUG) $(GTK_CFLAGS) $^ -c -o $@

search_utils.o: search_utils.c
	$(CC) $(CFLAGS) $(DEBUG) $^ -c -o $@

hashtable.o: hashtable.c
	$(CC) $(CFLAGS) $(DEBUG) $^ -c -o $@

stack.o: stack.c
	$(CC) $(CFLAGS) $(DEBUG) $^ -c -o $@

linked_list.o: linked_list.c
	$(CC) $(CFLAGS) $(DEBUG) $^ -c -o $@

clean:
	rm -f *.o
	rm search
